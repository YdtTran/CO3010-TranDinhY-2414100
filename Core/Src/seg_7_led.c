#include "seg_7_led.h"

// Common Cathode (active HIGH, bit order: DP-g-f-e-d-c-b-a)
const uint8_t digit_to_cc[10] = {
    0x3F, // 0 = 00111111
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Common Anode (active LOW, bit order: DP-g-f-e-d-c-b-a)
const uint8_t digit_to_ca[10] = {
    0xC0, // 0 = 11000000
    0xF9, // 1
    0xA4, // 2
    0xB0, // 3
    0x99, // 4
    0x92, // 5
    0x82, // 6
    0xF8, // 7
    0x80, // 8
    0x90  // 9
};

/**
 * @brief Initialize the 7-segment LED
 * @param led_handle Pointer to the 7-segment LED handle
 * @param com Common cathode (0) or common anode (1)
 * @param port GPIO port
 * @param gpio GPIO pins a, b, c, d, e, f, g, dp
 * @retval None
 */
void seg_7_led_init(seg_7_led_handle_t *led_handle, uint8_t com, GPIO_TypeDef *port, uint32_t gpio[8])
{
    led_handle->com = com;
    led_handle->port = port;
    for (int i = 0; i < 8; i++)
    {
        led_handle->gpio[i] = gpio[i];
        HAL_GPIO_WritePin(led_handle->port, led_handle->gpio[i], GPIO_PIN_RESET);
    }
}

void seg_7_led_display(seg_7_led_handle_t *led_handle, uint8_t digit)
{
    if (digit > 9)
        return; // Invalid digit

    if (led_handle->com == SEG_7_LED_COMMON_CATHODE)
    {
        // Common Cathode
        led_handle->segments.data = digit_to_cc[digit];
    }
    else
    {
        // Common Anode
        led_handle->segments.data = digit_to_ca[digit];
    }

    for (int i = 0; i < 8; i++)
    {
        if (led_handle->segments.data & (1 << i))
        {
            HAL_GPIO_WritePin(led_handle->port, led_handle->gpio[i], GPIO_PIN_SET);
        }
        else
        {
            HAL_GPIO_WritePin(led_handle->port, led_handle->gpio[i], GPIO_PIN_RESET);
        }
    }
}