#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/led.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

#define BATTERY_LED_NODE DT_NODELABEL(npm1300_leds)
#define BATTERY_LOW_THRESHOLD CONFIG_PIPAR_BATTERY_LOW_THRESHOLD

#if DT_NODE_EXISTS(BATTERY_LED_NODE)

static const struct device *led_dev = DEVICE_DT_GET(BATTERY_LED_NODE);

static int battery_indicator_init(void) {
    if (!device_is_ready(led_dev)) {
        LOG_ERR("Battery LED device not ready");
        return -ENODEV;
    }

    led_off(led_dev, 0);

    LOG_INF("Battery indicator initialized (threshold: %d%%)", BATTERY_LOW_THRESHOLD);
    return 0;
}

static int battery_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    bool low = ev->state_of_charge <= BATTERY_LOW_THRESHOLD;

    if (low) {
        led_on(led_dev, 0);
    } else {
        led_off(led_dev, 0);
    }

    LOG_DBG("Battery: %d%% - LED %s", ev->state_of_charge, low ? "ON" : "OFF");

    return 0;
}

ZMK_LISTENER(battery_indicator, battery_state_changed_listener);
ZMK_SUBSCRIPTION(battery_indicator, zmk_battery_state_changed);

SYS_INIT(battery_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
