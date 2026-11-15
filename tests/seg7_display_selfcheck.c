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
    uint8_t digits[] = {0, 2, 0, 5};
    const uint8_t patterns[] = {0x3F, 0x5B, 0x3F, 0x6D};
    seg7_display_t display = {
        .segs = {SEG[0], SEG[1], SEG[2], SEG[3], SEG[4], SEG[5], SEG[6], 0},
        .en_pin = pins, .seg_port = &segments, .en_port = &enables,
        .buffer = digits, .num = 4, .com = SEG7_COMMON_ANODE};
    SEG7_Init(&display);
    assert(display.current_digit == 0);
    assert((enables.ODR & ENABLE_MASK) == ENABLE_MASK);
    assert((segments.ODR & 0xFE00) == 0xFE00);
    for (unsigned cycle = 0; cycle < 3; ++cycle)
    {
        for (unsigned digit = 0; digit < 4; ++digit)
        {
            SEG7_Scan(&display);
            assert((segments.ODR & 0xFE00) == ((~patterns[digit] & 0x7F) << 9));
            assert((enables.ODR & ENABLE_MASK) == (ENABLE_MASK & ~pins[digit]));
            assert(display.current_digit == (digit + 1) % 4);
        }
    }
    SEG7_SetNumbers(&display, 0, 99);
    assert(digits[0] == 0 && digits[1] == 0 && digits[2] == 9 && digits[3] == 9);
    SEG7_SetNumbers(&display, 12, 34);
    assert(digits[0] == 1 && digits[1] == 2 && digits[2] == 3 && digits[3] == 4);
    SEG7_SetNumbers(&display, 100, 5);
    SEG7_SetNumbers(&display, 5, 100);
    display.num = 2;
    SEG7_SetNumbers(&display, 56, 78);
    assert(digits[0] == 1 && digits[1] == 2 && digits[2] == 3 && digits[3] == 4);
    display.num = 4;
    SEG7_Scan(&display);
    assert((segments.ODR & 0xFE00) == ((~0x06 & 0x7F) << 9));
    return 0;
}
