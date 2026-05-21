/*
 * Recovery watchdog feeder.
 *
 * Feeds a task watchdog from the system workqueue. If the workqueue
 * blocks for longer than the configured timeout (e.g., because an I2C
 * transfer is stuck or some other handler never returns), the watchdog
 * fires and resets the device — so the keyboard recovers automatically
 * instead of silently hanging.
 *
 * Note: if you later enable CONFIG_ZMK_SLEEP=y, you'll want to suspend
 * or reconfigure the watchdog around sys_poweroff(); the hardware
 * watchdog by default keeps counting during sleep on nRF52840.
 */

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/watchdog.h>
#include <zephyr/task_wdt/task_wdt.h>
#include <zephyr/logging/log.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define WDT_NODE             DT_NODELABEL(wdt)
#define WDT_TASK_TIMEOUT_MS  CONFIG_PIPAR_WATCHDOG_TASK_TIMEOUT_MS
#define WDT_FEED_INTERVAL_MS CONFIG_PIPAR_WATCHDOG_FEED_INTERVAL_MS

static int wdt_channel_id;
static struct k_work_delayable feed_work;

static void feed_handler(struct k_work *work) {
    int ret = task_wdt_feed(wdt_channel_id);
    if (ret != 0) {
        LOG_WRN("task_wdt_feed failed: %d", ret);
    }
    k_work_schedule(&feed_work, K_MSEC(WDT_FEED_INTERVAL_MS));
}

static int pipar_watchdog_init(void) {
    const struct device *wdt = DEVICE_DT_GET(WDT_NODE);

    if (!device_is_ready(wdt)) {
        LOG_ERR("Watchdog device not ready");
        return -ENODEV;
    }

    int ret = task_wdt_init(wdt);
    if (ret != 0 && ret != -EALREADY) {
        LOG_ERR("task_wdt_init failed: %d", ret);
        return ret;
    }

    wdt_channel_id = task_wdt_add(WDT_TASK_TIMEOUT_MS, NULL, NULL);
    if (wdt_channel_id < 0) {
        LOG_ERR("task_wdt_add failed: %d", wdt_channel_id);
        return wdt_channel_id;
    }

    k_work_init_delayable(&feed_work, feed_handler);
    k_work_schedule(&feed_work, K_MSEC(WDT_FEED_INTERVAL_MS));

    LOG_INF("Pipar watchdog initialized (task timeout %dms, feed every %dms)",
            WDT_TASK_TIMEOUT_MS, WDT_FEED_INTERVAL_MS);
    return 0;
}

SYS_INIT(pipar_watchdog_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
