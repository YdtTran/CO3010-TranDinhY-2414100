#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "stm32f1xx_hal.h"

typedef enum
{
    LED_GREEN,
    LED_YELLOW,
    LED_RED
} light_state_t;

typedef struct
{
    uint8_t counter;
    light_state_t state;
    uint32_t gpio[3];
} light_handle_t;

#endif // TRAFFIC_LIGHT_H