#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/i2c.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#define BATTERY_LED_NODE      DT_NODELABEL(npm1300_leds)
#define I2C_BUS_NODE          DT_NODELABEL(i2c0)
#define BATTERY_LOW_THRESHOLD CONFIG_PIPAR_BATTERY_LOW_THRESHOLD
#define LED_WRITE_RETRIES     3

#if DT_NODE_EXISTS(BATTERY_LED_NODE)

static const struct device *led_dev = DEVICE_DT_GET(BATTERY_LED_NODE);
static const struct device *i2c_dev = DEVICE_DT_GET(I2C_BUS_NODE);

/* -1 = unknown, 0 = LED off (battery OK), 1 = LED on (battery low).
 * Tracks the last *successfully written* state so we only do I2C
 * traffic when the state actually changes.
 */
static int8_t last_led_state = -1;

/* Wraps led_on/led_off with retries and bus recovery.
 * Returns 0 on success, error code on unrecoverable failure.
 */
static int led_safe_set(bool on) {
    int ret = 0;

    for (int attempt = 0; attempt < LED_WRITE_RETRIES; attempt++) {
        ret = on ? led_on(led_dev, 0) : led_off(led_dev, 0);
        if (ret == 0) {
            return 0;
        }

        LOG_WRN("npm1300 LED %s failed (%d), attempt %d/%d",
                on ? "on" : "off", ret, attempt + 1, LED_WRITE_RETRIES);

        /* -EAGAIN comes from CONFIG_I2C_NRFX_TRANSFER_TIMEOUT firing;
         * -EIO from NRFX_ERROR_BUSY or general I2C errors. Both are
         * worth attempting bus recovery.
         */
        if ((ret == -EAGAIN || ret == -EIO) && device_is_ready(i2c_dev)) {
            int rec = i2c_recover_bus(i2c_dev);
            if (rec != 0) {
                LOG_WRN("i2c_recover_bus failed: %d", rec);
            }
            k_msleep(10);
        } else {
            /* Not a bus issue — retry won't help. */
            break;
        }
    }

    LOG_ERR("npm1300 LED unrecoverable after %d retries", LED_WRITE_RETRIES);
    return ret;
}

static int battery_indicator_init(void) {
    if (!device_is_ready(led_dev)) {
        LOG_ERR("Battery LED device not ready");
        return -ENODEV;
    }

    /* No I2C write at init. The bus may not be fully settled in early
     * APPLICATION_INIT and a hung transfer here would still block the
     * init thread despite the transfer timeout. The first battery event
     * will reassert the correct state within seconds of boot anyway.
     */

    LOG_INF("Battery indicator initialized (threshold: %d%%)", BATTERY_LOW_THRESHOLD);
    return 0;
}

static int battery_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    int8_t new_state = (ev->state_of_charge <= BATTERY_LOW_THRESHOLD) ? 1 : 0;

    /* Edge-detect: skip the I2C write entirely if nothing changed.
     * This collapses the per-minute-forever traffic into a transition-only
     * pattern, dramatically reducing exposure to TWIM hang conditions.
     */
    if (new_state == last_led_state) {
        return 0;
    }

    if (led_safe_set(new_state != 0) == 0) {
        last_led_state = new_state;
        LOG_DBG("Battery: %d%% - LED %s",
                ev->state_of_charge, new_state ? "ON" : "OFF");
    }
    /* If the write failed: don't update last_led_state. Next event
     * (assuming the bus recovers) will see the mismatch and retry.
     */

    return 0;
}

ZMK_LISTENER(battery_indicator, battery_state_changed_listener);
ZMK_SUBSCRIPTION(battery_indicator, zmk_battery_state_changed);

SYS_INIT(battery_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
