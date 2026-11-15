#include "seg7.h"

/* Common cathode patterns: bit 0 = A, bit 7 = DP. */
static const uint8_t digit_to_cc[10] = {
    0x3F, 0x06, 0x5B, 0x4F, 0x66,
    0x6D, 0x7D, 0x07, 0x7F, 0x6F
};

void SEG7_SetDigit(const uint32_t segs[8], GPIO_TypeDef *port,
                   uint8_t value, uint8_t com)
{
    if (segs == NULL || port == NULL)
    {
        return;
    }

    uint8_t pattern = (value < 10) ? digit_to_cc[value] : 0;
    if (com == SEG7_COMMON_ANODE)
    {
        pattern = (uint8_t)~pattern;
    }

    for (uint8_t i = 0; i < 8; i++)
    {
        if (segs[i] != 0)
        {
            GPIO_PinState state = (pattern & (1 << i)) ? GPIO_PIN_SET : GPIO_PIN_RESET;
            HAL_GPIO_WritePin(port, segs[i], state);
        }
    }
}
