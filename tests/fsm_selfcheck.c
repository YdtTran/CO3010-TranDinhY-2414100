#include <assert.h>
#include "fsm.h"
#include "pin.h"
#include "software_timer.h"
static uint16_t pressed, outputs;
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    assert(port == BTN_PORT);
    return pressed & pin ? GPIO_PIN_RESET : GPIO_PIN_SET;
}
void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pins, GPIO_PinState state)
{
    if (port != GPIOB) return;
    if (state == GPIO_PIN_SET) outputs |= pins;
    else outputs &= ~pins;
    if (app && app->mode != MODE_SETTING)
        assert((outputs & (L[0] | T[0])) != (L[0] | T[0]));
}
void Error_Handler(void) { assert(0); }
static void run(void)
{
    FSM_CollectEvents();
    FSM_Dispatch();
    FSM_Automatic_Run();
    FSM_Manual_Run();
    FSM_Setting_Run();
    FSM_UpdateDisplay();
}
static void ticks(unsigned count)
{
    while (count--) { SOFT_TIMER_Run(); run(); }
}
static void press(unsigned button)
{
    pressed = BTN[button]; ticks(3U);
    pressed = 0U; ticks(3U);
}
static unsigned draft(void) { return app->digits[2] * 10U + app->digits[3]; }
int main(void)
{
    fsm_t fsm;
    FSM_Init(&fsm);
    assert(app->mode == MODE_AUTOMATIC);
    assert(app->digits[1] == 1U && app->digits[3] == 4U);
    for (unsigned cycle = 0; cycle < 2U; ++cycle) {
        for (unsigned second = 0; second < 10U; ++second) {
            assert(app->straight.state == (second < 2U ? LED_GREEN : second < 5U ? LED_YELLOW : LED_RED));
            assert(app->cross.state == (second < 5U ? LED_RED : second < 7U ? LED_GREEN : LED_YELLOW));
            ticks(1000U / TICK_INTERVAL_MS);
        }
    }
    press(0U);
    assert(app->mode == MODE_MANUAL && app->straight.state == LED_GREEN);
    assert(app->digits[1] == 2U && app->digits[3] == 5U);
    ticks(200U);
    assert(app->straight.remaining_s == 2U);
    pressed = BTN[1]; ticks(200U);
    assert(app->straight.state == LED_YELLOW && app->cross.state == LED_RED);
    pressed = 0U; ticks(3U);
    press(1U); assert(app->cross.state == LED_GREEN);
    press(1U); assert(app->cross.state == LED_YELLOW);
    press(1U); assert(app->straight.state == LED_GREEN);
    /* Mode changes suppress simultaneous increment/confirm events. */
    pressed = BTN[0] | BTN[1] | BTN[2]; ticks(3U);
    assert(app->mode == MODE_SETTING && app->digits[1] == 2U && draft() == 5U);
    pressed = 0U; ticks(3U);
    ticks(25U - 3U);
    assert(app->straight.state == LED_OFF && app->cross.state == LED_OFF);
    press(1U); press(2U);
    assert(app->green_s == 3U && app->yellow_s == 3U);
    assert(app->digits[1] == 3U && draft() == 3U);
    press(1U); press(2U);
    assert(app->yellow_s == 4U && app->digits[1] == 4U);
    press(2U);
    assert(app->digits[1] == 2U && draft() == 7U);
    /* Red wraps to 1; invalid confirmation must keep the saved configuration. */
    for (unsigned i = 0; i < 93U; ++i) press(1U);
    assert(draft() == 1U);
    press(2U);
    assert(app->digits[1] == 2U && app->green_s == 3U && app->yellow_s == 4U);
    pressed = BTN[1]; ticks(3U);
    assert(draft() == 2U);
    ticks(25U); assert(draft() == 3U);
    pressed = 0U; ticks(3U);
    press(0U);
    assert(app->mode == MODE_AUTOMATIC && app->straight.remaining_s == 3U && app->cross.remaining_s == 7U);
    ticks(99U - 3U);
    assert(app->straight.remaining_s == 3U);
    ticks(1U); assert(app->straight.remaining_s == 2U);
    /* Re-entry loads saved red, not the discarded draft. */
    press(0U); press(0U);
    assert(draft() == 7U && app->digits[1] == 2U);
    /* Yellow+green above 99 is rejected. */
    press(2U);
    for (unsigned i = 4U; i < 99U; ++i) press(1U);
    assert(draft() == 99U);
    press(2U);
    assert(app->digits[1] == 3U && app->yellow_s == 4U);
    return 0;
}
