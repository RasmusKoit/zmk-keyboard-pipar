#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/hid_indicators.h>

#if DT_NODE_EXISTS(DT_NODELABEL(hid_indicator_led))

#define HID_LED_NODE DT_NODELABEL(hid_indicator_led)

static const struct gpio_dt_spec hid_led = GPIO_DT_SPEC_GET(HID_LED_NODE, gpios);
static uint8_t last_indicators = 0xFF;

static int hid_indicator_init(void) {
    if (!gpio_is_ready_dt(&hid_led)) {
        LOG_ERR("HID indicator LED GPIO not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure(hid_led.port, hid_led.pin, GPIO_OUTPUT_LOW);
    if (ret < 0) {
        LOG_ERR("Failed to configure HID indicator LED GPIO");
        return ret;
    }

    LOG_INF("HID indicator initialized (bit %d, inverted %s)",
            CONFIG_PIPAR_HID_INDICATOR_BIT,
            IS_ENABLED(CONFIG_PIPAR_HID_INDICATOR_INVERTED) ? "yes" : "no");
    return 0;
}

static int hid_indicator_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    /* Skip the first event — it's the host sync on connect, not a user action.
     * LED stays off until the user explicitly toggles the indicator. */
    if (last_indicators == 0xFF) {
        last_indicators = ev->indicators;
        LOG_DBG("HID indicator: stored initial state 0x%02X, LED stays off", ev->indicators);
        return 0;
    }

    last_indicators = ev->indicators;

    bool indicator_on = (ev->indicators & BIT(CONFIG_PIPAR_HID_INDICATOR_BIT)) != 0;
    bool led_on = IS_ENABLED(CONFIG_PIPAR_HID_INDICATOR_INVERTED) ? !indicator_on : indicator_on;
    gpio_pin_set(hid_led.port, hid_led.pin, led_on ? 1 : 0);

    LOG_DBG("HID indicator: %s (bit %d)", led_on ? "ON" : "OFF", CONFIG_PIPAR_HID_INDICATOR_BIT);

    return 0;
}

ZMK_LISTENER(hid_indicator, hid_indicator_listener);
ZMK_SUBSCRIPTION(hid_indicator, zmk_hid_indicators_changed);

SYS_INIT(hid_indicator_init, POST_KERNEL, 50);

#endif
