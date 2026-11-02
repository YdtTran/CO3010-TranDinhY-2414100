#include "output_display.h"
#include "pin.h"
#include "seg7.h"

void OUTPUT_ShowMode(uint8_t mode)
{
    const uint32_t segments[8] = {
        SEG[0], SEG[1], SEG[2], SEG[3], SEG[4], SEG[5], SEG[6], 0U};
    HAL_GPIO_WritePin(EN_PORT, EN[0] | EN[1], GPIO_PIN_SET);
    SEG7_SetDigit(segments, SEG_PORT, mode, SEG7_COMMON_ANODE);
    /* Shared enables show the mode on digits 0 and 2 together. */
    HAL_GPIO_WritePin(EN_PORT, EN[0], EN_ACTIVE_STATE);
}