#include "traffic_light.h"
#include <stddef.h>
uint8_t TRAFFIC_LIGHT_Init(traffic_light_t *light)
{
    if (light == NULL || light->port == NULL) return 0;
    uint32_t used = 0U;
    for (unsigned i = 0; i < 3U; ++i) {
        uint32_t pin = light->gpio[i];
        if (pin == 0U || pin > GPIO_PIN_15 || (pin & (pin - 1U)) || (used & pin)) return 0;
        used |= pin;
    }
    light->remaining_s = 0U;
    TRAFFIC_LIGHT_SetColor(light, LED_OFF);
    return 1;
}
void TRAFFIC_LIGHT_SetColor(traffic_light_t *light, light_state_t color)
{
    if ((unsigned)color > LED_OFF) return;
    HAL_GPIO_WritePin(light->port, light->gpio[0] | light->gpio[1] | light->gpio[2], GPIO_PIN_RESET);
    if (color != LED_OFF) HAL_GPIO_WritePin(light->port, light->gpio[color], GPIO_PIN_SET);
    light->state = color;
}
void TRAFFIC_LIGHT_SetTime(traffic_light_t *light, uint16_t seconds) { light->remaining_s = seconds; }
void TRAFFIC_LIGHT_IncreaseTime(traffic_light_t *light)
{
    if (light->remaining_s < UINT16_MAX) ++light->remaining_s;
}
void TRAFFIC_LIGHT_DecreaseTime(traffic_light_t *light)
{
    if (light->remaining_s > 0U) --light->remaining_s;
}
