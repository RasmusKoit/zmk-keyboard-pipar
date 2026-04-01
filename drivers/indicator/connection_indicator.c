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
#define BLINK_DURATION_MS 300
#define BLINK_PAUSE_MS 200
#define STEADY_DELAY_MS 500

static const struct gpio_dt_spec connection_led = GPIO_DT_SPEC_GET(CONNECTION_LED_NODE, gpios);
static struct k_work_delayable blink_work;
static uint8_t blink_count = 0;
static uint8_t current_blink = 0;
static bool is_blinking = false;
static bool blink_state = false; // false = LED on phase, true = LED off phase
static bool waiting_for_steady = false; // true = waiting 500ms before staying on
static bool initial_phase = false; // true = doing initial 300ms on + 500ms off


static void blink_work_handler(struct k_work *work) {
    if (!is_blinking) {
        return;
    }

    if (initial_phase) {
        // Initial 300ms on is done, turn off and wait 500ms before blink sequence
        gpio_pin_set_dt(&connection_led, 1); // Off (inverted)
        initial_phase = false;
        k_work_schedule(&blink_work, K_MSEC(STEADY_DELAY_MS));
        return;
    }

    if (waiting_for_steady) {
        // 500ms delay is done, turn LED off and finish
        gpio_pin_set_dt(&connection_led, 1); // Off (inverted: 1 = off)
        is_blinking = false;
        waiting_for_steady = false;
        return;
    }

    if (blink_state) {
        // Currently off, check if we need more blinks
        if (current_blink < blink_count) {
            // Turn on for next blink
            gpio_pin_set_dt(&connection_led, 0); // On (inverted)
            blink_state = false;
            current_blink++;
            k_work_schedule(&blink_work, K_MSEC(BLINK_DURATION_MS));
        } else {
            // All blinks done, wait 500ms off, then stay on
            waiting_for_steady = true;
            k_work_schedule(&blink_work, K_MSEC(STEADY_DELAY_MS));
        }
    } else {
        // Currently on, turn off between blinks
        gpio_pin_set_dt(&connection_led, 1); // Off (inverted)
        blink_state = true;
        k_work_schedule(&blink_work, K_MSEC(BLINK_PAUSE_MS));
    }
}

static void start_blink_sequence(uint8_t profile_index) {
    if (is_blinking) {
        k_work_cancel_delayable(&blink_work);
    }

    blink_count = profile_index + 1; // Profile 0 = 1 blink, Profile 3 = 4 blinks
    current_blink = 0;
    is_blinking = true;
    blink_state = false;
    waiting_for_steady = false;
    initial_phase = true;

    LOG_DBG("Starting blink sequence: %d blinks for BT profile %d", blink_count, profile_index);

    // Start with initial 300ms on
    gpio_pin_set_dt(&connection_led, 0); // On (inverted)
    k_work_schedule(&blink_work, K_MSEC(BLINK_DURATION_MS));
}

static void update_connection_led(bool usb_active, uint8_t bt_profile_index) {
    if (usb_active) {
        // USB mode - turn LED off immediately
        if (is_blinking) {
            k_work_cancel_delayable(&blink_work);
            is_blinking = false;
        }
        gpio_pin_set_dt(&connection_led, 1); // Off (inverted: 1 = off)
        LOG_DBG("Connection indicator: USB (OFF)");
    } else {
        // BT mode - start blink sequence
        start_blink_sequence(bt_profile_index);
        LOG_DBG("Connection indicator: BT profile %d", bt_profile_index);
    }
}

static int connection_indicator_init(void) {
    if (!gpio_is_ready_dt(&connection_led)) {
        LOG_ERR("Connection LED GPIO not ready");
        return -ENODEV;
    }

    // Start with LED off until first endpoint change event
    int ret = gpio_pin_configure_dt(&connection_led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        LOG_ERR("Failed to configure connection LED GPIO");
        return ret;
    }

    k_work_init_delayable(&blink_work, blink_work_handler);

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
    uint8_t bt_profile = 0;

    if (!usb_active && ev->endpoint.transport == ZMK_TRANSPORT_BLE) {
        // Extract BT profile index (0-4)
        bt_profile = ev->endpoint.ble.profile_index;
    }

    update_connection_led(usb_active, bt_profile);

    return 0;
}

ZMK_LISTENER(connection_indicator, endpoint_changed_listener);
ZMK_SUBSCRIPTION(connection_indicator, zmk_endpoint_changed);

SYS_INIT(connection_indicator_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);

#endif // DT_NODE_EXISTS(DT_NODELABEL(connection_led))
