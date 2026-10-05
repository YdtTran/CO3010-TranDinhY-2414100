#include "seg7.h"

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
void SEG7Init(seg7_handle_t *led_handle, uint8_t com, GPIO_TypeDef *port, uint32_t gpio[8])
{
    if (led_handle == NULL)
    {
        return;
    }

    led_handle->com = com;
    led_handle->port = port;
    for (int i = 0; i < 8; i++)
    {
        led_handle->gpio[i] = gpio[i];
    }
    SEG7Clear(led_handle);
}

/**
 * @brief Set a decimal digit (0-9) on the 7-segment display
 * @param led_handle Pointer to the 7-segment LED handle
 * @param digit Digit to display (0-9). Values > 9 turn off all segments.
 * @retval None
 */
void SEG7Set_Digit(seg7_handle_t *led_handle, uint8_t digit)
{
    if (led_handle == NULL || led_handle->port == NULL)
    {
        return;
    }

    if (digit > 9)
    {
        SEG7Clear(led_handle);
        return; // Invalid or blank digit
    }

    if (led_handle->com == seg7_COMMON_CATHODE)
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

/**
 * @brief Turn off all segments on the 7-segment display
 * @param led_handle Pointer to the 7-segment LED handle
 * @retval None
 */
void SEG7Clear(seg7_handle_t *led_handle)
{
    if (led_handle == NULL || led_handle->port == NULL)
    {
        return;
    }

    // In Common Anode (active LOW): driving pins HIGH (SET) turns off segments.
    // In Common Cathode (active HIGH): driving pins LOW (RESET) turns off segments.
    GPIO_PinState off_state = (led_handle->com == seg7_COMMON_ANODE) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    led_handle->segments.data = (led_handle->com == seg7_COMMON_ANODE) ? 0xFF : 0x00;

    for (int i = 0; i < 8; i++)
    {
        HAL_GPIO_WritePin(led_handle->port, led_handle->gpio[i], off_state);
    }
}