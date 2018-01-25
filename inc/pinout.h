#ifndef __PINOUT_H__
#define __PINOUT_H__

#define PIN_13      0x4       /*!< Alias on PIN 13. */
#define PIN_14      0x5       /*!< Alias on PIN 14. */
#define PIN_15      0x6       /*!< Alias on PIN 15. */

#define COLOR_1     PIN_13    /*!< Color 1 */
#define COLOR_2     PIN_14    /*!< Color 2 */
#define COLOR_3     PIN_15    /*!< Color 3 */

/*
 * Warning : common cathode, so those PINs are anodes
 */

#define VALUE_OFF   0x1       /*!< Alias on OFF state of a pin. */
#define VALUE_ON    0x0       /*!< Alias on ON state of a pin. */

int pinout_init(void);
void pinout_free(void);

void reset_gpios(void);

#endif // __PINOUT_H__
