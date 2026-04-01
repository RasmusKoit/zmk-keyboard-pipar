#pragma once

#include <zephyr/kernel.h>

struct battery_profile {
    const float *soc_points;      // State of charge grid (0.0 to 1.0)
    size_t soc_count;
    const float *temps;           // Temperature points
    size_t temp_count;
    const float *ocv_table;       // Open circuit voltage vs SOC
    size_t ocv_count;
    const float *r0_table;        // Series resistance
    size_t r0_count;
    const float *r1_table;        // RC pair resistance
    size_t r1_count;
    const float *c1_table;        // RC pair capacitance
    size_t c1_count;
    const char *name;
    uint16_t capacity_mah;        // Battery capacity in mAh
};

// YDL375678 battery profile at 22°C
extern const struct battery_profile ydl375678_profile;
