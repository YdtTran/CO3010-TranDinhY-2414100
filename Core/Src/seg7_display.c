#include "seg7_display.h"

/**
 * @brief Initialize the multi-digit multiplexed 7-segment display controller
 * @param handle Pointer to display controller handle
 * @param seg_handle Pointer to underlying seg7_handle_t managing the segment bus
 * @param type Display topology (SEG7DISPLAY_TYPE_COMMON_ANODE or SEG7DISPLAY_TYPE_COMMON_CATHODE)
 * @param digit_count Number of multiplexed digits (up to SEG7DISPLAY_MAX_DIGITS)
 * @param com_pins Array of digit COM enable pins (length equal to digit_count)
 */
void SEG7Display_Init(SEG7Display_Handle_t *handle,
                      seg7_handle_t *seg_handle,
                      SEG7Display_Type_t type,
                      uint8_t digit_count,
                      const SEG7Display_Com_Pin_t *com_pins)
{
    if (handle == NULL)
    {
        return;
    }

    handle->seg_handle = seg_handle;
    handle->type = type;
    handle->digit_count = (digit_count > SEG7DISPLAY_MAX_DIGITS) ? SEG7DISPLAY_MAX_DIGITS : digit_count;
    // Set scan index to the last digit so that the first call to SEG7Display_Update advances to digit 0
    handle->current_digit = (handle->digit_count > 0) ? (handle->digit_count - 1) : 0;
    handle->dp_mask = 0;

    // Ensure the underlying 7-segment bus driver matches the display polarity
    if (handle->seg_handle != NULL)
    {
        handle->seg_handle->com = (type == SEG7DISPLAY_TYPE_COMMON_ANODE) ? seg7_COMMON_ANODE : seg7_COMMON_CATHODE;
        SEG7Clear(handle->seg_handle);
    }

    // Initialize per-digit handles, COM pins, and clear the display buffer
    for (uint8_t i = 0; i < SEG7DISPLAY_MAX_DIGITS; i++)
    {
        handle->buffer[i] = SEG7DISPLAY_BLANK;
        handle->seg_handles[i] = seg_handle;

        if (com_pins != NULL && i < handle->digit_count)
        {
            handle->com_pins[i] = com_pins[i];
            // Initialize COM pin to OFF (inactive state)
            SEG7Display_Set_Com_State(handle, i, 0);
        }
        else
        {
            handle->com_pins[i].port = NULL;
            handle->com_pins[i].pin = 0;
        }
    }
}

/**
 * @brief Set the active/inactive state of a digit's COM enable pin
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 to digit_count - 1)
 * @param state 1 = Turn ON COM rail, 0 = Turn OFF COM rail
 */
void SEG7Display_Set_Com_State(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t state)
{
    if (handle == NULL || digit_idx >= handle->digit_count)
    {
        return;
    }

    if (handle->com_pins[digit_idx].port == NULL)
    {
        return;
    }

    GPIO_PinState pin_state;
    if (handle->type == SEG7DISPLAY_TYPE_COMMON_ANODE)
    {
        /* Common Anode (High-Side PNP / P-MOSFET Switch):
         * - COM Rail connects to +VCC.
         * - Digit Enable is Active-LOW:
         *     0V on base/gate forward-biases PNP -> Turns ON COM rail.
         *     3.3V on base/gate shuts off PNP    -> Turns OFF COM rail.
         */
        pin_state = (state != 0) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }
    else
    {
        /* Common Cathode (Low-Side NPN / N-MOSFET Switch):
         * - COM Rail connects to GND.
         * - Digit Enable is Active-HIGH:
         *     3.3V on base/gate turns ON NPN -> Pulls COM rail to GND.
         *     0V on base/gate shuts off NPN   -> Turns OFF COM rail.
         */
        pin_state = (state != 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;
    }

    HAL_GPIO_WritePin(handle->com_pins[digit_idx].port, handle->com_pins[digit_idx].pin, pin_state);
}

/**
 * @brief Non-blocking display refresh function designed for periodic timer ISR (1ms - 5ms)
 *        Execution sequence:
 *        1. Turn off current COM (anti-ghosting)
 *        2. Advance to target digit
 *        3. Update segment data on shared bus
 *        4. Turn on target COM
 * @param handle Pointer to display controller handle
 */
void SEG7Display_Update(SEG7Display_Handle_t *handle)
{
    if (handle == NULL || handle->digit_count == 0 || handle->seg_handle == NULL)
    {
        return;
    }

    // 1. Anti-Ghosting Step 1: Turn off current COM before changing segment bus
    SEG7Display_Set_Com_State(handle, handle->current_digit, 0);

    // 2. Advance scan index to next digit
    handle->current_digit++;
    if (handle->current_digit >= handle->digit_count)
    {
        handle->current_digit = 0;
    }

    // 3. Anti-Ghosting Step 2: Drive segment data for the new digit onto the shared bus
    uint8_t digit_val = handle->buffer[handle->current_digit];
    if (digit_val <= 9)
    {
        SEG7Set_Digit(handle->seg_handle, digit_val);

        // Drive decimal point segment (gpio[7]) if configured
        if (handle->seg_handle->gpio[7] != 0)
        {
            uint8_t dp_active = (handle->dp_mask & (1 << handle->current_digit)) ? 1 : 0;
            GPIO_PinState dp_state;
            if (handle->type == SEG7DISPLAY_TYPE_COMMON_ANODE)
            {
                // Active-LOW for Common Anode
                dp_state = dp_active ? GPIO_PIN_RESET : GPIO_PIN_SET;
            }
            else
            {
                // Active-HIGH for Common Cathode
                dp_state = dp_active ? GPIO_PIN_SET : GPIO_PIN_RESET;
            }
            HAL_GPIO_WritePin(handle->seg_handle->port, handle->seg_handle->gpio[7], dp_state);
        }

        // 4. Anti-Ghosting Step 3: Turn on target digit COM rail after segment bus is stable
        SEG7Display_Set_Com_State(handle, handle->current_digit, 1);
    }
    else
    {
        // Blank digit: clear segments and keep COM turned off
        SEG7Clear(handle->seg_handle);
        SEG7Display_Set_Com_State(handle, handle->current_digit, 0);
    }
}

/**
 * @brief Set an individual digit value in the display buffer
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 = MSD/leftmost ... digit_count - 1 = LSD/rightmost)
 * @param value Digit value (0-9) or SEG7DISPLAY_BLANK
 */
void SEG7Display_Set_Digit(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t value)
{
    if (handle == NULL || digit_idx >= handle->digit_count)
    {
        return;
    }

    handle->buffer[digit_idx] = value;
}

/**
 * @brief Format and display a multi-digit integer (right-aligned, blank-padded)
 * @param handle Pointer to display controller handle
 * @param number Integer value to display
 */
void SEG7Display_Set_Number(SEG7Display_Handle_t *handle, int32_t number)
{
    SEG7Display_Set_Number_Padded(handle, number, 0);
}

/**
 * @brief Format and display a multi-digit integer with optional zero padding
 * @param handle Pointer to display controller handle
 * @param number Integer value to display
 * @param pad_zero 1 = pad leading digits with '0', 0 = blank leading digits
 */
void SEG7Display_Set_Number_Padded(SEG7Display_Handle_t *handle, int32_t number, uint8_t pad_zero)
{
    if (handle == NULL || handle->digit_count == 0)
    {
        return;
    }

    uint32_t temp = (number < 0) ? (uint32_t)(-number) : (uint32_t)number;

    // Fill digits from rightmost (LSD, index = digit_count - 1) to leftmost (MSD, index = 0)
    for (int8_t i = (int8_t)(handle->digit_count - 1); i >= 0; i--)
    {
        if (temp > 0 || (i == handle->digit_count - 1 && number == 0))
        {
            handle->buffer[i] = (uint8_t)(temp % 10);
            temp /= 10;
        }
        else if (pad_zero)
        {
            handle->buffer[i] = 0;
        }
        else
        {
            handle->buffer[i] = SEG7DISPLAY_BLANK;
        }
    }
}

/**
 * @brief Enable or disable decimal point for a specific digit position
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 to digit_count - 1)
 * @param enable 1 = turn ON decimal point, 0 = turn OFF decimal point
 */
void SEG7Display_Set_Decimal_Point(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t enable)
{
    if (handle == NULL || digit_idx >= handle->digit_count)
    {
        return;
    }

    if (enable)
    {
        handle->dp_mask |= (1 << digit_idx);
    }
    else
    {
        handle->dp_mask &= ~(1 << digit_idx);
    }
}

/**
 * @brief Clear the display buffer and turn off all digit COM outputs
 * @param handle Pointer to display controller handle
 */
void SEG7Display_Clear(SEG7Display_Handle_t *handle)
{
    if (handle == NULL)
    {
        return;
    }

    for (uint8_t i = 0; i < handle->digit_count; i++)
    {
        handle->buffer[i] = SEG7DISPLAY_BLANK;
        SEG7Display_Set_Com_State(handle, i, 0);
    }

    handle->dp_mask = 0;

    if (handle->seg_handle != NULL)
    {
        SEG7Clear(handle->seg_handle);
    }
}
