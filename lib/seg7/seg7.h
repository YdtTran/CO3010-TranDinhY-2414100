#ifndef SEG7_H
#define SEG7_H

#include "stm32f1xx_hal.h"

#define SEG7_COMMON_CATHODE 0
#define SEG7_COMMON_ANODE 1

/* segs contains eight pins in A, B, C, D, E, F, G, DP order.
 * Use 0 for an unconnected segment. Values above 9 blank the display.
 */
void SEG7_SetDigit(const uint32_t segs[8], GPIO_TypeDef *port,
                   uint8_t value, uint8_t com);

#endif /* SEG7_H */
