#include <assert.h>
#include <stddef.h>
#include "traffic_light.h"

static GPIO_TypeDef port;
static unsigned writes;

void HAL_GPIO_WritePin(GPIO_TypeDef *gpio, uint16_t pins, GPIO_PinState state)
{
    if (state == GPIO_PIN_SET) {
        assert(gpio->ODR == 0);
        gpio->ODR |= pins;
    } else gpio->ODR &= ~pins;
    ++writes;
}

int main(void)
{
    traffic_light_t light = {
        .port = &port,
        .gpio = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2,
                 GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5}};
    assert(!TRAFFIC_LIGHT_Init(NULL));
    assert(TRAFFIC_LIGHT_Init(&light));
    assert(light.straight_color == LED_OFF && light.cross_color == LED_OFF && port.ODR == 0);

    for (unsigned straight = LED_GREEN; straight <= LED_OFF; ++straight) {
        for (unsigned cross = LED_GREEN; cross <= LED_OFF; ++cross) {
            TRAFFIC_LIGHT_SetColor(&light, straight, cross);
            uint32_t expected = (straight == LED_OFF ? 0 : light.gpio[straight]) |
                                (cross == LED_OFF ? 0 : light.gpio[3 + cross]);
            assert(port.ODR == expected);
            assert(light.straight_color == straight && light.cross_color == cross);
        }
    }
    TRAFFIC_LIGHT_SetColor(&light, LED_GREEN, LED_RED);
    unsigned before = writes;
    TRAFFIC_LIGHT_SetColor(&light, LED_GREEN, LED_RED);
    assert(writes == before);
    TRAFFIC_LIGHT_SetColor(&light, (light_state_t)99, LED_RED);
    TRAFFIC_LIGHT_SetColor(&light, LED_RED, (light_state_t)-1);
    assert(writes == before && port.ODR == (GPIO_PIN_0 | GPIO_PIN_5));
    assert(light.straight_color == LED_GREEN && light.cross_color == LED_RED);

    light.port = NULL;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.port = &port;
    light.gpio[5] = GPIO_PIN_0;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[5] = GPIO_PIN_6 | GPIO_PIN_7;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[5] = 0;
    assert(!TRAFFIC_LIGHT_Init(&light));
    light.gpio[5] = 0x10000;
    assert(!TRAFFIC_LIGHT_Init(&light));
    assert(writes == before);
    return 0;
}
