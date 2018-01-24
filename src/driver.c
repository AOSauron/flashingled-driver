#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#
#include "../inc/pinout.h"
#include "../inc/flashled.h"

/**
 * @brief Initialize the driver that handles the flashing led.
 */
static int __init flashled_driver_init(void) {
    printk("Starting FlashingLED driver !\n");

    if (pinout_init() != 0x0) {
        return -1;
    }

    if (flashled_init() != 0x0) {
        pinout_free();

        return -1;
    }

    get_flashled_inst();

    return 0;
}

/**
 * @brief Release the driver that handles the flashing led and
 * free all used resources.
 */
static void __exit flashled_driver_exit(void) {
    printk("Exiting FlashingLED driver !\n");
    pinout_free();
    flashled_free();

    free_flashled_inst();
}

module_init(flashled_driver_init);
module_exit(flashled_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Guillaume Garcia & Alexandre V. Jamet");
MODULE_DESCRIPTION("Driver de commande d'une LED clignotante (sur 3 couleurs)");
