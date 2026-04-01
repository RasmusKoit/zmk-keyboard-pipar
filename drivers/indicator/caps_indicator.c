#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/hid_indicators_changed.h>
#include <zmk/events/caps_word_state_changed.h>
#include <zmk/hid_indicators.h>
#include <zmk/keymap.h>

#if DT_NODE_EXISTS(DT_NODELABEL(caps_led))

#define CAPS_LED_NODE DT_NODELABEL(caps_led)

static const struct gpio_dt_spec caps_led = GPIO_DT_SPEC_GET(CAPS_LED_NODE, gpios);

static bool caps_lock_active = false;
static bool caps_word_active = false;

static void update_caps_led(void) {
    bool led_on = caps_lock_active || caps_word_active;
    // Invert the logic: 0 = LED on, 1 = LED off
    gpio_pin_set_dt(&caps_led, led_on ? 0 : 1);
    LOG_DBG("Caps indicator: %s (caps_lock=%d, caps_word=%d)",
            led_on ? "ON" : "OFF", caps_lock_active, caps_word_active);
}

static int caps_indicator_init(void) {
    if (!gpio_is_ready_dt(&caps_led)) {
        LOG_ERR("Caps LED GPIO not ready");
        return -ENODEV;
    }

    // Start with LED off (inverted logic: 1 = off)
    int ret = gpio_pin_configure_dt(&caps_led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure caps LED GPIO");
        return ret;
    }

    LOG_INF("Caps indicator initialized");
    return 0;
}

static int hid_indicators_changed_listener(const zmk_event_t *eh) {
    const struct zmk_hid_indicators_changed *ev = as_zmk_hid_indicators_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    // Check if caps lock bit is set (bit 1)
    caps_lock_active = (ev->indicators & BIT(1)) != 0;
    update_caps_led();

    return 0;
}

static int caps_word_state_changed_listener(const zmk_event_t *eh) {
    const struct zmk_caps_word_state_changed *ev = as_zmk_caps_word_state_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    caps_word_active = ev->active;
    update_caps_led();

    return 0;
}

ZMK_LISTENER(caps_indicator, hid_indicators_changed_listener);
ZMK_SUBSCRIPTION(caps_indicator, zmk_hid_indicators_changed);

ZMK_LISTENER(caps_word_indicator, caps_word_state_changed_listener);
ZMK_SUBSCRIPTION(caps_word_indicator, zmk_caps_word_state_changed);

SYS_INIT(caps_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // DT_NODE_EXISTS(DT_NODELABEL(caps_led))
