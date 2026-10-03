#ifndef SEG_7_LED_H
#define SEG_7_LED_H

#include "stm32f1xx_hal.h"

#define SEG_7_LED_COMMON_CATHODE 0
#define SEG_7_LED_COMMON_ANODE 1

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
    uint8_t data;
} seg_7_led_t;

typedef struct
{
    seg_7_led_t segments;
    uint8_t com;
    uint32_t gpio[8];
    GPIO_TypeDef *port;
} seg_7_led_handle_t;

void seg_7_led_init(seg_7_led_handle_t *led_handle, uint8_t com, GPIO_TypeDef *port, uint32_t gpio[8]);
void seg_7_led_display(seg_7_led_handle_t *led_handle, uint8_t digit);

#endif