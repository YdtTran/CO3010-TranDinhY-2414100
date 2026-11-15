#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include "stm32f1xx_hal.h"

typedef enum { LED_GREEN, LED_YELLOW, LED_RED, LED_OFF } light_state_t;

typedef struct {
    GPIO_TypeDef *port;
    uint32_t gpio[6]; /* Straight G/Y/R, then cross G/Y/R; active HIGH. */
    light_state_t straight_color, cross_color;
} traffic_light_t;

/* Invalid configuration leaves GPIO unchanged. Init blanks all six LEDs. */
uint8_t TRAFFIC_LIGHT_Init(traffic_light_t *light);
/* Call after Init, from main. Invalid colors leave outputs unchanged. Both greens are supported for setting mode. */
void TRAFFIC_LIGHT_SetColor(traffic_light_t *light, light_state_t straight, light_state_t cross);
#endif
