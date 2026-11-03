#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f1xx_hal.h"

#define BUTTON_DEBOUNCE_SAMPLES 3U

typedef enum
{
    BUTTON_RELEASED = 0,
    BUTTON_PRESSED
} button_state_t;

typedef struct
{
    GPIO_TypeDef *port;
    uint32_t gpio;
    GPIO_PinState active_level;
    GPIO_PinState buffer[4]; // 0, 1, 2 for sampling, 3 for stable state
    uint16_t timer_for_key_press;
    uint16_t repeat_ticks; // Number of scan calls between repeats; 0 disables repeats.
    volatile uint8_t press_flag;
    volatile button_state_t state;
} button_handle_t;

/* Call before starting the scan timer; initial state is BUTTON_RELEASED.
 * repeat_ticks counts scan calls; 0 disables held-button repeats.
 * Configure the GPIO as an input with the appropriate pull-up/down first.
 */
void BUTTON_Init(button_handle_t *button, GPIO_TypeDef *port, uint32_t gpio,
                 GPIO_PinState active_level, uint16_t repeat_ticks);

/* Call once per button at a fixed timer interrupt interval (e.g. 10 ms).
 * Change state only after three consecutive identical GPIO samples,
 * for both press and release. Held buttons repeat every repeat_ticks scans.
 */
void BUTTON_Scan(button_handle_t *button);

/* Read the debounced state from the main loop; reading does not clear it. */
button_state_t BUTTON_GetState(const button_handle_t *button);

/* Read without clearing; scanning sets the flag on press/repeat. */
uint8_t BUTTON_GetPressFlag(const button_handle_t *button);
void BUTTON_ClearPressFlag(button_handle_t *button);

#endif /* BUTTON_H */
