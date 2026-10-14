#ifndef SEG7_H
#define SEG7_H

#include "stm32f1xx_hal.h"

#define SEG7_COMMON_CATHODE 0
#define SEG7_COMMON_ANODE 1

typedef union
{
    struct
    {
        /* data */
        uint8_t dp : 1;
        uint8_t g : 1;
        uint8_t f : 1;
        uint8_t e : 1;
        uint8_t d : 1;
        uint8_t c : 1;
        uint8_t b : 1;
        uint8_t a : 1;
    };
    uint16_t data;
} seg7_t;

typedef struct
{
    seg7_t segments;
    uint8_t com;
    uint16_t gpio[8];
    GPIO_TypeDef *port;
} seg7_handle_t;

void SEG7Init(seg7_handle_t *led_handle, uint8_t com, GPIO_TypeDef *port, uint16_t gpio[8]);
void SEG7Set_Digit(seg7_handle_t *led_handle, uint8_t digit);
void SEG7Clear(seg7_handle_t *led_handle);

#endif /* SEG7_H */