#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H
#include "stm32f1xx_hal.h"
typedef enum { LED_GREEN, LED_YELLOW, LED_RED, LED_OFF } light_state_t;
typedef struct {
    GPIO_TypeDef *port;
    uint32_t gpio[3]; /* Green/yellow/red; active HIGH. */
    light_state_t state;
    uint16_t remaining_s;
} traffic_light_t;
/* Invalid configuration leaves GPIO unchanged. Init blanks a valid light. */
uint8_t TRAFFIC_LIGHT_Init(traffic_light_t *light);
/* Other operations require successful Init; call from main. */
void TRAFFIC_LIGHT_SetColor(traffic_light_t *light, light_state_t color);
void TRAFFIC_LIGHT_SetTime(traffic_light_t *light, uint16_t seconds);
void TRAFFIC_LIGHT_IncreaseTime(traffic_light_t *light);
void TRAFFIC_LIGHT_DecreaseTime(traffic_light_t *light);
#endif
