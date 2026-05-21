#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/sys_io.h>
#include <zephyr/devicetree.h>

/*
 * Bring-up helper: drive every named-but-otherwise-unused breakout pin HIGH
 * at boot so a multimeter on the corresponding J10/J11 pad reads ~3.3V when
 * the trace from the ISP1807 to the pad is intact.
 *
 * LED_1 (P1.14) and LED_2 (P1.12) are also driven HIGH here; the matching
 * gpio-leds nodes in the DTS leave them unowned at boot, so this init
 * function is what actually lights them.
 */
static int pipar_dev_pin_test_init(void) {
#if CONFIG_BOARD_PIPAR_DEV
    const struct device *p0 = DEVICE_DT_GET(DT_NODELABEL(gpio0));
    const struct device *p1 = DEVICE_DT_GET(DT_NODELABEL(gpio1));

    /* LEDs */
    gpio_pin_configure(p1, 14, GPIO_OUTPUT_HIGH);  /* LED_1 */
    gpio_pin_configure(p1, 12, GPIO_OUTPUT_HIGH);  /* LED_2 */

    /* Trackpoint header (TPRST / TPData / TPClock) */
    gpio_pin_configure(p0, 9,  GPIO_OUTPUT_HIGH);
    gpio_pin_configure(p0, 12, GPIO_OUTPUT_HIGH);
    gpio_pin_configure(p0, 14, GPIO_OUTPUT_HIGH);

    /* General-purpose breakout pin */
    gpio_pin_configure(p1, 11, GPIO_OUTPUT_HIGH);
#endif
    return 0;
}

SYS_INIT(pipar_dev_pin_test_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
