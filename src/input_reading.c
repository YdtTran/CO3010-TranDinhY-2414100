#include "input_reading.h"
#include "button.h"
#include "pin.h"

static button_handle_t mode_button;
static uint32_t last_sample_ms;

void INPUT_Init(void)
{
    BUTTON_Init(&mode_button, BTN_PORT, BTN[0], BTN_ACTIVE_STATE, 0U);
    last_sample_ms = HAL_GetTick();
}

uint8_t INPUT_ModePressed(void)
{
    uint32_t now = HAL_GetTick();
    /* Use SysTick time so TIM2 interval changes do not change debouncing. */
    if ((uint32_t)(now - last_sample_ms) >= 10U)
    {
        last_sample_ms = now;
        BUTTON_Scan(&mode_button);
    }
    return BUTTON_IsPress(&mode_button);
}