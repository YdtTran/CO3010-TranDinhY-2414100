#ifndef SEG7DISPLAY_H
#define SEG7DISPLAY_H

#include "stm32f1xx_hal.h"
#include "seg7.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Maximum supported multiplexed digits
 */
#define SEG7DISPLAY_MAX_DIGITS 8

/**
 * @brief Blank character indicator for display buffer
 */
#define SEG7DISPLAY_BLANK 0xFF

/**
 * @brief Display topology configuration enum
 */
typedef enum
{
    SEG7DISPLAY_TYPE_COMMON_CATHODE = 0, /* COM rail: GND via Low-Side NPN/N-MOS (Active-HIGH EN, Active-HIGH SEG) */
    SEG7DISPLAY_TYPE_COMMON_ANODE   = 1  /* COM rail: +VCC via High-Side PNP/P-MOS (Active-LOW EN, Active-LOW SEG) */
} SEG7Display_Type_t;

/**
 * @brief GPIO pin descriptor for digit COM enable
 */
typedef struct
{
    GPIO_TypeDef *port; /* GPIO port (e.g. GPIOA, GPIOB) */
    uint16_t pin;       /* GPIO pin mask (e.g. GPIO_PIN_0) */
} SEG7Display_Com_Pin_t;

/**
 * @brief Multiplexed 7-segment display controller handle
 */
typedef struct SEG7Display_Handle
{
    seg7_handle_t *seg_handle;                          /* Pointer to shared 7-segment bus handle */
    seg7_handle_t *seg_handles[SEG7DISPLAY_MAX_DIGITS]; /* Array of pointers to seg7_handle_t per digit */
    SEG7Display_Com_Pin_t com_pins[SEG7DISPLAY_MAX_DIGITS]; /* COM select GPIOs per digit */
    uint8_t buffer[SEG7DISPLAY_MAX_DIGITS];             /* Display digit buffer (0-9, SEG7DISPLAY_BLANK) */
    SEG7Display_Type_t type;                            /* Display mode: COM-A or COM-K */
    uint8_t digit_count;                                /* Number of digits configured */
    uint8_t current_digit;                              /* Active digit scan index */
    uint8_t dp_mask;                                    /* Decimal point bitmask (bit N corresponds to digit N) */
} SEG7Display_Handle_t;

/* Backward-compatible / alternate typedef matching lowercase requirement */
typedef SEG7Display_Handle_t seg7_display_handle_t;

/* Exported Functions --------------------------------------------------------*/

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
                      const SEG7Display_Com_Pin_t *com_pins);

/**
 * @brief Non-blocking display refresh function designed for periodic timer ISR (1ms - 5ms)
 *        Execution sequence: Turn off current COM -> Advance digit -> Update segments -> Turn on new COM.
 * @param handle Pointer to display controller handle
 */
void SEG7Display_Update(SEG7Display_Handle_t *handle);

/**
 * @brief Set the active/inactive state of a digit's COM enable pin
 *        Automatically handles polarity:
 *        - COM-A (PNP / P-MOSFET): state != 0 -> LOW (0V ON), state == 0 -> HIGH (3.3V OFF)
 *        - COM-K (NPN / N-MOSFET): state != 0 -> HIGH (3.3V ON), state == 0 -> LOW (0V OFF)
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 to digit_count - 1)
 * @param state 1 = Turn ON COM rail, 0 = Turn OFF COM rail
 */
void SEG7Display_Set_Com_State(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t state);

/**
 * @brief Set an individual digit value in the display buffer
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 = MSD/leftmost ... digit_count - 1 = LSD/rightmost)
 * @param value Digit value (0-9) or SEG7DISPLAY_BLANK
 */
void SEG7Display_Set_Digit(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t value);

/**
 * @brief Format and display a multi-digit integer (right-aligned, blank-padded)
 * @param handle Pointer to display controller handle
 * @param number Integer value to display
 */
void SEG7Display_Set_Number(SEG7Display_Handle_t *handle, int32_t number);

/**
 * @brief Format and display a multi-digit integer with optional zero padding
 * @param handle Pointer to display controller handle
 * @param number Integer value to display
 * @param pad_zero 1 = pad leading digits with '0', 0 = blank leading digits
 */
void SEG7Display_Set_Number_Padded(SEG7Display_Handle_t *handle, int32_t number, uint8_t pad_zero);

/**
 * @brief Enable or disable decimal point for a specific digit position
 * @param handle Pointer to display controller handle
 * @param digit_idx Digit index (0 to digit_count - 1)
 * @param enable 1 = turn ON decimal point, 0 = turn OFF decimal point
 */
void SEG7Display_Set_Decimal_Point(SEG7Display_Handle_t *handle, uint8_t digit_idx, uint8_t enable);

/**
 * @brief Clear the display buffer and turn off all digit COM outputs
 * @param handle Pointer to display controller handle
 */
void SEG7Display_Clear(SEG7Display_Handle_t *handle);

#ifdef __cplusplus
}
#endif

#endif /* SEG7DISPLAY_H */
