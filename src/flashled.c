
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/string.h>
#include <linux/ktime.h>
#include <linux/timer.h>
#
#include "../inc/pinout.h"
#include "../inc/flashled.h"

// ========================================================================= //
//                      INTERFACE FUNCTION WITH USER SPACE                   //
// ========================================================================= //
static int              flashled_major;
static struct device*   flashled_device;
static struct class*    flashled_class;
static dev_t            flashled_devt;

static ssize_t flashled_write(struct file *file, const char __user *data, size_t size, loff_t *loff);

static struct file_operations flashled_fops = {
    .read = NULL,
    .write = &flashled_write,
    .open = NULL,
    .release = NULL
};

static ssize_t flashled_write(struct file *file, const char __user *data, size_t size, loff_t *loff) {
    char *buf = kmalloc(size, GFP_KERNEL);

    if (copy_from_user(buf, data, 8) > 0) {
        return -1;
    }

    get_flashled_inst()->power_on = ((uint64_t) *buf);

    kfree(buf);

    return size;
}
// ========================================================================= //
//                             KERNEL SPACE FUNCTIONS                        //
// ========================================================================= //
#define INTERVAL 1e6  // Basic interval obtained when power_on = 1. Will be mux by power_on to get a greater interval.

static struct flashled* flashled_inst = NULL;
static struct hrtimer flashled_timer;
static char current_color = 0; // 0 is color 1, 1 is color 2, 2 is color 3.

/**
 * @brief Array of function pointers to the different functions used to
 * display a color (or not) with the led.
 */
static void (*flashled_helper []) (const struct flashled *) = {
    &display_color_1,
    &display_color_2,
    &display_color_3,
    &display_off
};

/**
 * @brief This function is a callback used to update the led with
 * the appropriate color.
 */
static enum hrtimer_restart flashled_timer_callback(struct hrtimer *arg) { 
    
    // Adjusting the interval with the coef power_on
    uint64_t adjusted_interval;
    uint64_t coef = flashled_inst->power_on;
    if (coef > 0) {
	adjusted_interval = INTERVAL * coef;
    }
    else adjusted_interval = INTERVAL;

    ktime_t curr_time = ktime_get(), interval = ktime_set(0, adjusted_interval);
    uint8_t unit, decade, hundred;

    // Reset the led to avoid color superposition
    display_off(flashled_inst);

    // Display the appropriate digit at the appropriate place, only IF power_on > 0 !
    if (coef > 0) {
	    switch (current_color) {
		case 0: // Color 1
		    display_color_1(display_inst);
		    break;
		case 1: // Color 2
		    display_color_2(display_inst);
		    break;
		case 2: // Color 3
		    display_color_3(display_inst);
		    break;
		default:
		    break;
	    }
    }

    // Moving to the next color, even if this one was not displayed
    current_color = (current_color + 1) % 3;

    hrtimer_forward(arg, curr_time, interval);

    return HRTIMER_RESTART;
}

/**
 * @brief Get the unique instance of the flashing led handle.
 */
struct flashled* get_flashled_inst(void) {
    if (flashled_inst == NULL) {
        ktime_t interval;

        flashled_inst = kmalloc(sizeof(struct flashled), GFP_KERNEL);

        // PIN numbers affectation
        flashled_inst->1_color_pin = COLOR_1;
        flashled_inst->2_color_pin = COLOR_2;
        flashled_inst->3_color_pin = COLOR_3;

        // Initialize the state of the led (to light off).
        flashled_inst->power_on = 0;

	// Initializing the led timer (with a fixed interval for the moment).
        interval = ktime_set(0, INTERVAL);

        hrtimer_init(&flashled_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
        flashled_timer.function = &flashled_timer_callback;

        hrtimer_start(&flashled_timer, interval, HRTIMER_MODE_REL);
    }

    return flashled_inst;
}

/**
 * @brief Free the unique instance of the display handle.
 */
void free_flashled_inst(void) {
    if (dflashled_inst != NULL) {
        hrtimer_cancel(&flashled_timer);

        kfree(flashled_inst);
    }
}

/**
 * Initialize the virtual file associated to the flashled device to create an
 * interface in the user space.
 */
int flashled_init(void) {
    // Getting a major for the peripheral.
    if ((flashled_major = register_chrdev(0, "flashled", &flashled_fops)) <= 0) {
        printk(KERN_ERR"(1) Error while initializing the peripheral... :(\n");

        return -1;
    }

    // Creating a class for the device and so on...
    flashled_class = class_create(THIS_MODULE, "flashled");
    if (!flashled_class) {
        printk(KERN_ERR"(2) Error while initializing the peripheral... :(\n");

        unregister_chrdev(flashled_major, "flashled");

        return -1;
    }

    flashled_devt = MKDEV(flashled_major, 0);
    flashled_device = device_create(flashled_class, NULL, flashled_devt, NULL, "flashled");

    return 0;
}

/**
 * @brief Free the virtual file associated to the flashing led.
 */
void flashled_free(void) {
    device_destroy(flashled_class, flashled_devt);
    class_destroy(flashled_class);
    unregister_chrdev(flashled_major, "flashled");
}

/**
 *
 * 	Lighting on Functions
 * 
 */


// Display the first color (disable others)
void display_color_1(const struct flashled *inst)
{
    gpio_set_value(inst->1_color_pin, VALUE_ON);
    gpio_set_value(inst->2_color_pin, VALUE_OFF);
    gpio_set_value(inst->3_color_pin, VALUE_OFF);
}

// Display the first color (disable others)
void display_color_2(const struct flashled *inst)
{
    gpio_set_value(inst->1_color_pin, VALUE_OFF);
    gpio_set_value(inst->2_color_pin, VALUE_ON);
    gpio_set_value(inst->3_color_pin, VALUE_OFF);
}

// Display the first color (disable others)
void display_color_3(const struct flashled *inst)
{
    gpio_set_value(inst->1_color_pin, VALUE_OFF);
    gpio_set_value(inst->2_color_pin, VALUE_OFF);
    gpio_set_value(inst->3_color_pin, VALUE_ON);
}

// Disable all colors
void display_off(const struct flashled *inst)
{
    gpio_set_value(inst->1_color_pin, VALUE_OFF);
    gpio_set_value(inst->2_color_pin, VALUE_OFF);
    gpio_set_value(inst->3_color_pin, VALUE_OFF);
}
