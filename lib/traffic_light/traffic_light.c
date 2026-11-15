#include "traffic_light.h"
#include <stddef.h>

uint8_t TRAFFIC_LIGHT_Init(traffic_light_t *light)
{
    if (light == NULL || light->port == NULL) return 0;
    uint32_t used = 0;
    for (unsigned i = 0; i < 6; ++i) {
        uint32_t pin = light->gpio[i];
        if (pin == 0 || pin > GPIO_PIN_15 || (pin & (pin - 1)) || (used & pin)) return 0;
        used |= pin;
    }
    light->straight_color = LED_OFF;
    light->cross_color = LED_OFF;
    HAL_GPIO_WritePin(light->port, used, GPIO_PIN_RESET);
    return 1;
}

void TRAFFIC_LIGHT_SetColor(traffic_light_t *light, light_state_t straight, light_state_t cross)
{
    if ((unsigned)straight > LED_OFF || (unsigned)cross > LED_OFF) return;

    if (light->straight_color == straight && light->cross_color == cross) return;

    uint32_t all = 0;
    for (unsigned i = 0; i < 6; ++i)
        all |= light->gpio[i];
    HAL_GPIO_WritePin(light->port, all, GPIO_PIN_RESET);

    uint32_t on = 0;
    if (straight != LED_OFF) on |= light->gpio[straight];
    if (cross != LED_OFF) on |= light->gpio[3 + cross];
    if (on) HAL_GPIO_WritePin(light->port, on, GPIO_PIN_SET);
    light->straight_color = straight;
    light->cross_color = cross;
}
