/* Standalone test: link seg7.c and seg7_display.c without the HAL GPIO driver. */
#include <assert.h>
#include "seg7_display.h"
#include "pin.h"

static GPIO_TypeDef segments, enables;
#define ENABLE_MASK (GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9)

void HAL_GPIO_WritePin(GPIO_TypeDef *gpio, uint16_t pins, GPIO_PinState state)
{
    if (gpio == &segments)
        assert((enables.ODR & ENABLE_MASK) == ENABLE_MASK);
    if (state == GPIO_PIN_SET)
        gpio->ODR |= pins;
    else
        gpio->ODR &= ~pins;
    if (gpio == &enables && state == GPIO_PIN_RESET)
        assert((enables.ODR & ENABLE_MASK) == (ENABLE_MASK & ~pins));
}

int main(void)
{
    const uint32_t *pins = EN;
    uint8_t digits[] = {0U, 2U, 0U, 5U};
    const uint8_t patterns[] = {0x3FU, 0x5BU, 0x3FU, 0x6DU};
    seg7_display_t display = {
        .segs = {SEG[0], SEG[1], SEG[2], SEG[3], SEG[4], SEG[5], SEG[6], 0U},
        .en_pin = pins, .seg_port = &segments, .en_port = &enables,
        .buffer = digits, .num = 4U, .com = SEG7_COMMON_ANODE};
    SEG7_Init(&display);
    assert(display.current_digit == 0U);
    assert((enables.ODR & ENABLE_MASK) == ENABLE_MASK);
    assert((segments.ODR & 0xFE00U) == 0xFE00U);
    for (unsigned cycle = 0U; cycle < 3U; ++cycle)
    {
        for (unsigned digit = 0U; digit < 4U; ++digit)
        {
            SEG7_Scan(&display);
            assert((segments.ODR & 0xFE00U) == ((~patterns[digit] & 0x7FU) << 9U));
            assert((enables.ODR & ENABLE_MASK) == (ENABLE_MASK & ~pins[digit]));
            assert(display.current_digit == (digit + 1U) % 4U);
        }
    }
    digits[0] = 1U;
    SEG7_Scan(&display);
    assert((segments.ODR & 0xFE00U) == ((~0x06U & 0x7FU) << 9U));
    return 0;
}