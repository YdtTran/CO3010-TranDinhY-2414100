/* Standalone target test: link button.c, omitting the hardware HAL GPIO driver. */
#include <assert.h>
#include "button.h"

static GPIO_PinState sample = GPIO_PIN_SET;
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    (void)port;
    (void)pin;
    return sample;
}

static void scan(button_handle_t *button, GPIO_PinState level, unsigned count)
{
    sample = level;
    while (count--) BUTTON_Scan(button);
}

int main(void)
{
    button_handle_t button;
    BUTTON_Init(&button, GPIOB, GPIO_PIN_13, GPIO_PIN_RESET, 25);
    scan(&button, GPIO_PIN_RESET, 2);
    assert(!BUTTON_GetPressFlag(&button));
    scan(&button, GPIO_PIN_SET, 1);
    scan(&button, GPIO_PIN_RESET, 2);
    assert(!BUTTON_GetPressFlag(&button));
    scan(&button, GPIO_PIN_RESET, 1);
    assert(BUTTON_GetPressFlag(&button));
    assert(BUTTON_GetPressFlag(&button));
    BUTTON_ClearPressFlag(&button);
    assert(!BUTTON_GetPressFlag(&button));
    scan(&button, GPIO_PIN_RESET, 100);
    assert(!BUTTON_GetPressFlag(&button));
    scan(&button, GPIO_PIN_SET, 3);
    assert(BUTTON_GetState(&button) == BUTTON_RELEASED);
    scan(&button, GPIO_PIN_RESET, 3);
    assert(BUTTON_GetPressFlag(&button));
    return 0;
}
