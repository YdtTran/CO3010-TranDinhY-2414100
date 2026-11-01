#ifndef SEG7_DISPLAY_H
#define SEG7_DISPLAY_H

#include "stm32f1xx_hal.h"

#define DIGIT_EN_PORT GPIOA
#define SEG_PORT GPIOB
#define LED_NUM 4

#define SEG_A_PIN GPIO_PIN_0
#define SEG_B_PIN GPIO_PIN_1
#define SEG_C_PIN GPIO_PIN_2
#define SEG_D_PIN GPIO_PIN_3
#define SEG_E_PIN GPIO_PIN_4
#define SEG_F_PIN GPIO_PIN_5
#define SEG_G_PIN GPIO_PIN_6
#define SEG_DP_PIN GPIO_PIN_7

#define DIGIT_EN0_PIN GPIO_PIN_6
#define DIGIT_EN1_PIN GPIO_PIN_7
#define DIGIT_EN2_PIN GPIO_PIN_8
#define DIGIT_EN3_PIN GPIO_PIN_9

void init7SEG(void);
void update7SEG(int index);
void updateClockBuffer(int hour, int minute);
void scan7SEG(void);

#endif /* SEG7_DISPLAY_H */
