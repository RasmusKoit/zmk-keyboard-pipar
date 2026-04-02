#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/split_peripheral_status_changed.h>

#if DT_NODE_EXISTS(DT_NODELABEL(ble_connection_led))

#define BLE_LED_NODE DT_NODELABEL(ble_connection_led)
#define BLINK_INTERVAL_MS 500

static const struct gpio_dt_spec ble_led = GPIO_DT_SPEC_GET(BLE_LED_NODE, gpios);
static struct k_work_delayable blink_work;
static bool connected;
static bool blink_state;

static void blink_work_handler(struct k_work *work) {
    if (connected) {
        return;
    }

    blink_state = !blink_state;
    gpio_pin_set_dt(&ble_led, blink_state ? 1 : 0);
    k_work_schedule(&blink_work, K_MSEC(BLINK_INTERVAL_MS));
}

static void start_blinking(void) {
    blink_state = false;
    k_work_schedule(&blink_work, K_NO_WAIT);
}

static void stop_blinking(void) {
    k_work_cancel_delayable(&blink_work);
}

static int ble_connection_indicator_init(void) {
    if (!gpio_is_ready_dt(&ble_led)) {
        LOG_ERR("BLE connection LED GPIO not ready");
        return -ENODEV;
    }

    int ret = gpio_pin_configure_dt(&ble_led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure BLE connection LED GPIO");
        return ret;
    }

    k_work_init_delayable(&blink_work, blink_work_handler);

    /* Start blinking — not connected yet at boot */
    connected = false;
    start_blinking();

    LOG_INF("BLE connection indicator initialized");
    return 0;
}

static int split_peripheral_status_changed_listener(const zmk_event_t *eh) {
    const struct zmk_split_peripheral_status_changed *ev =
        as_zmk_split_peripheral_status_changed(eh);
    if (ev == NULL) {
        return 0;
    }

    connected = ev->connected;

    if (connected) {
        stop_blinking();
        gpio_pin_set_dt(&ble_led, 1);
    } else {
        start_blinking();
    }

    LOG_DBG("BLE connection indicator: %s", connected ? "CONNECTED" : "BLINKING");

    return 0;
}

ZMK_LISTENER(ble_connection_indicator, split_peripheral_status_changed_listener);
ZMK_SUBSCRIPTION(ble_connection_indicator, zmk_split_peripheral_status_changed);

SYS_INIT(ble_connection_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif
