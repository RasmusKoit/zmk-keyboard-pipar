#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include "battery_profile.h"

LOG_MODULE_REGISTER(fuel_gauge, CONFIG_ZMK_LOG_LEVEL);

#define CHARGER_NODE DT_NODELABEL(charger)

#if DT_NODE_EXISTS(CHARGER_NODE)

static const struct device *charger_dev = DEVICE_DT_GET(CHARGER_NODE);
static uint8_t cached_battery_percent = 100;
static int64_t last_update_time = 0;

#define UPDATE_INTERVAL_MS 30000  // Update every 30 seconds

// Linear interpolation helper
static float lerp(float x0, float y0, float x1, float y1, float x) {
    if (x1 == x0) return y0;
    return y0 + (y1 - y0) * (x - x0) / (x1 - x0);
}

// Estimate SOC from voltage using OCV table
static uint8_t voltage_to_soc(float voltage_v) {
    const struct battery_profile *profile = &ydl375678_profile;

    // Clamp voltage to table range
    if (voltage_v <= profile->ocv_table[0]) {
        return 0;
    }
    if (voltage_v >= profile->ocv_table[profile->ocv_count - 1]) {
        return 100;
    }

    // Find voltage in OCV table and interpolate SOC
    for (size_t i = 0; i < profile->ocv_count - 1; i++) {
        if (voltage_v >= profile->ocv_table[i] && voltage_v <= profile->ocv_table[i + 1]) {
            float soc_fraction = lerp(
                profile->ocv_table[i], profile->soc_points[i],
                profile->ocv_table[i + 1], profile->soc_points[i + 1],
                voltage_v
            );
            return (uint8_t)(soc_fraction * 100.0f);
        }
    }

    return 50;  // Fallback
}

static int read_battery_voltage(float *voltage_v) {
    struct sensor_value voltage;
    int ret;

    if (!device_is_ready(charger_dev)) {
        LOG_ERR("Charger device not ready");
        return -ENODEV;
    }

    ret = sensor_sample_fetch(charger_dev);
    if (ret < 0) {
        LOG_DBG("Failed to fetch sensor sample: %d", ret);
        return ret;
    }

    ret = sensor_channel_get(charger_dev, SENSOR_CHAN_GAUGE_VOLTAGE, &voltage);
    if (ret < 0) {
        LOG_DBG("Failed to get voltage channel: %d", ret);
        return ret;
    }

    *voltage_v = sensor_value_to_float(&voltage);
    return 0;
}

uint8_t zmk_battery_state_of_charge(void) {
    int64_t now = k_uptime_get();

    // Rate limit updates
    if (now - last_update_time < UPDATE_INTERVAL_MS && cached_battery_percent != 100) {
        return cached_battery_percent;
    }

    float voltage;
    int ret = read_battery_voltage(&voltage);
    if (ret < 0) {
        LOG_DBG("Failed to read battery voltage, using cached value");
        return cached_battery_percent;
    }

    uint8_t soc = voltage_to_soc(voltage);

    // Sanity check - don't allow huge jumps
    if (cached_battery_percent != 100) {
        int diff = (int)soc - (int)cached_battery_percent;
        if (diff > 10 || diff < -10) {
            LOG_WRN("Large SOC jump detected: %d%% -> %d%%, limiting change",
                    cached_battery_percent, soc);
            if (diff > 0) {
                soc = cached_battery_percent + 10;
            } else {
                soc = cached_battery_percent - 10;
            }
        }
    }

    cached_battery_percent = soc;
    last_update_time = now;

    LOG_INF("Battery: %.2fV -> %d%%", (double)voltage, soc);

    return soc;
}

static int fuel_gauge_init(void) {
    if (!device_is_ready(charger_dev)) {
        LOG_ERR("Charger device not ready for fuel gauge");
        return -ENODEV;
    }

    LOG_INF("Fuel gauge initialized with profile: %s", ydl375678_profile.name);

    // Do initial reading
    zmk_battery_state_of_charge();

    return 0;
}

SYS_INIT(fuel_gauge_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY_DEFAULT);

#endif // DT_NODE_EXISTS(CHARGER_NODE)
