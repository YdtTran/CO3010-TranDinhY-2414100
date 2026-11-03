#include <assert.h>
#include <stddef.h>
#include "traffic_light.h"
static GPIO_TypeDef port;
void HAL_GPIO_WritePin(GPIO_TypeDef *gpio, uint16_t pins, GPIO_PinState state)
{
    if (state == GPIO_PIN_SET) gpio->ODR |= pins;
    else gpio->ODR &= ~pins;
}
int main(void)
{
    traffic_light_t light = {.port = &port, .gpio = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2}};
    assert(!TRAFFIC_LIGHT_Init(NULL));
    assert(TRAFFIC_LIGHT_Init(&light));
    assert(light.state == LED_OFF && light.remaining_s == 0U && port.ODR == 0U);
    for (unsigned color = LED_GREEN; color <= LED_OFF; ++color) {
        TRAFFIC_LIGHT_SetColor(&light, color);
        assert(light.state == color);
        assert(port.ODR == (color == LED_OFF ? 0U : light.gpio[color]));
    }
    TRAFFIC_LIGHT_SetColor(&light, LED_GREEN);
    TRAFFIC_LIGHT_SetColor(&light, (light_state_t)99);
    assert(light.state == LED_GREEN && port.ODR == GPIO_PIN_0);
    TRAFFIC_LIGHT_DecreaseTime(&light);
    assert(light.remaining_s == 0U);
    TRAFFIC_LIGHT_SetTime(&light, UINT16_MAX);
    TRAFFIC_LIGHT_IncreaseTime(&light);
    assert(light.remaining_s == UINT16_MAX);
    TRAFFIC_LIGHT_DecreaseTime(&light);
    TRAFFIC_LIGHT_IncreaseTime(&light);
    assert(light.remaining_s == UINT16_MAX && port.ODR == GPIO_PIN_0);
    TRAFFIC_LIGHT_SetTime(&light, 1U);
    TRAFFIC_LIGHT_DecreaseTime(&light);
    assert(light.remaining_s == 0U && light.state == LED_GREEN);
    light.port = NULL;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.port = &port;
    light.gpio[0] = GPIO_PIN_1;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[0] = GPIO_PIN_3 | GPIO_PIN_4;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[0] = 0U;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[0] = 0x10000U;
    assert(!TRAFFIC_LIGHT_Init(&light));
    assert(port.ODR == GPIO_PIN_0);
    return 0;
}
