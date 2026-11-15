#include "button.h"
#include "stm32f1xx_hal.h"

void BUTTON_Init(button_handle_t *button, GPIO_TypeDef *port, uint32_t gpio,
                 GPIO_PinState active_level, uint16_t repeat_ticks)
{
    button->port = port;
    button->gpio = gpio;
    button->active_level = active_level;
    button->timer_for_key_press = 0;
    button->repeat_ticks = repeat_ticks;
    button->press_flag = 0;
    button->state = BUTTON_RELEASED;
    for (uint8_t i = 0; i < 4; i++)
    {
        button->buffer[i] = (active_level == GPIO_PIN_RESET) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }
}

void BUTTON_Scan(button_handle_t *button)
{
    button->buffer[0] = button->buffer[1];
    button->buffer[1] = button->buffer[2];
    button->buffer[2] = HAL_GPIO_ReadPin(button->port, button->gpio);

    if ((button->buffer[0] == button->buffer[1]) && (button->buffer[1] == button->buffer[2]))
    {
        if (button->buffer[3] != button->buffer[2])
        {
            button->buffer[3] = button->buffer[2];
            if (button->buffer[2] == button->active_level)
            {
                // Pressed
                button->state = BUTTON_PRESSED;
                button->press_flag = 1;
            }
            else
            {
                button->state = BUTTON_RELEASED;
                button->timer_for_key_press = 0;
            }
        }
    }
}

button_state_t BUTTON_GetState(const button_handle_t *button)
{
    return button->state;
}

uint8_t BUTTON_GetPressFlag(const button_handle_t *button)
{
    return button->press_flag;
}

void BUTTON_ClearPressFlag(button_handle_t *button)
{
    button->press_flag = 0;
}

void BUTTON_ClearPressFlags(button_handle_t *buttons, uint8_t count)
{
    for (uint8_t i = 0; i < count; ++i)
        BUTTON_ClearPressFlag(&buttons[i]);
}
