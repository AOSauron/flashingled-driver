#ifndef __FLASHLED_H__
#define __FLASHLED_H__

struct flashled {
    unsigned int 1_color_pin;     /*!< PIN to handle the first color of the led. */
    unsigned int 2_color_pin;     /*!< PIN to handle the second color of the led.*/
    unsigned int 3_color_pin;     /*!< PIN to handle the third color of the led. */

    uint64_t power_on;      	  /*!< If = 0, turn off the light. Else, turn it on : the bigger 
					it is the lower will be the frequency of the flash.*/
};

struct flashled* get_flashled_inst(void);
void free_flashled_inst(void);

int flashled_init(void);
void flashled_free(void);

void display_color_1(const struct flashled *inst);
void display_color_2(const struct flashled *inst);
void display_color_3(const struct flashled *inst);

void display_off(const struct flashled *inst);

#endif // __FLASHLED_H__
