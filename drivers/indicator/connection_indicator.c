#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/endpoint_changed.h>
#include <zmk/endpoints.h>

#if DT_NODE_EXISTS(DT_NODELABEL(connection_led))

#define CONNECTION_LED_NODE DT_NODELABEL(connection_led)

static const struct gpio_dt_spec connection_led = GPIO_DT_SPEC_GET(CONNECTION_LED_NODE, gpios);

static void update_connection_led(bool usb_active) {
    // Invert the logic: 0 = LED on (BT), 1 = LED off (USB)
    gpio_pin_set_dt(&connection_led, usb_active ? 1 : 0);
    LOG_DBG("Connection indicator: %s", usb_active ? "USB (OFF)" : "BT (ON)");
}

static int connection_indicator_init(void) {
    if (!gpio_is_ready_dt(&connection_led)) {
        LOG_ERR("Connection LED GPIO not ready");
        return -ENODEV;
    }

    // Start with LED on (assuming BT, inverted logic: 0 = on)
    int ret = gpio_pin_configure_dt(&connection_led, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure connection LED GPIO");
        return ret;
    }

    LOG_INF("Connection indicator initialized");
    return 0;
}

static int endpoint_changed_listener(const zmk_event_t *eh) {
    const struct zmk_endpoint_changed *ev = as_zmk_endpoint_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    // Check if the active endpoint is USB
    bool usb_active = (ev->endpoint.transport == ZMK_TRANSPORT_USB);
    update_connection_led(usb_active);

    return 0;
}

ZMK_LISTENER(connection_indicator, endpoint_changed_listener);
ZMK_SUBSCRIPTION(connection_indicator, zmk_endpoint_changed);

SYS_INIT(connection_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // DT_NODE_EXISTS(DT_NODELABEL(connection_led))
