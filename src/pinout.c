#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#
#include "../inc/pinout.h"

 /**
  * @brief List of GPIO PIN initialization with parameters.
  */
 static struct gpio init_gpios[] = {
    {COLOR_1, GPIOF_OUT_INIT_LOW, "Color 1"},          /* Color 1, default to OFF. */
    {COLOR_2, GPIOF_OUT_INIT_LOW, "Color 2"},          /* Color 2, default to OFF. */
    {COLOR_3, GPIOF_OUT_INIT_LOW, "Color 3"},          /* Color 3, default to OFF. */
};

/**
 * @brief Initialize all required GPIO PINs and set the them
 * all.
 *
 * @return Status code.
 */
int pinout_init(void) {
    int err_code;

    // Initializing the GPIOs...
    err_code = gpio_request_array(init_gpios, ARRAY_SIZE(init_gpios));

    if (err_code != 0x0) {
        printk(KERN_ERR"Error while requesting GPIOs... :(");

        gpio_free_array(init_gpios, ARRAY_SIZE(init_gpios));

        return -1;
    }

    // Ensure that all pins are at 0
    reset_gpios();

    return 0;
}

/**
 * @brief Releases all reserved GPIO PINs and reset the state
 * of all output PINs.
 */
void pinout_free(void) {
    // Initial state reset of all output GPIO PINs.
    reset_gpios();

    // Releasing reserved GPIOs...
    gpio_free_array(init_gpios, ARRAY_SIZE(init_gpios));
}


/*
 * @brief Reset the GPIOs to initial state (VALUE_OFF)
 */
void reset_gpios()
{
    int index;
    for (index = 0; index < ARRAY_SIZE(init_gpios); index++) {
        gpio_set_value(init_gpios[index].gpio, VALUE_OFF);
    }
}
