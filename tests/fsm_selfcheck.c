#include <assert.h>
#include "fsm_automatic.h"
#include "fsm_manual.h"
#include "fsm_setting.h"
#include "software_timer.h"

static uint16_t outputs;

void HAL_GPIO_WritePin(GPIO_TypeDef *port, uint16_t pins, GPIO_PinState level)
{
    if (port != GPIOB) return;
    if (level == GPIO_PIN_SET) outputs |= pins;
    else outputs &= ~pins;
    if (state < SET_RED)
        assert((outputs & (L[0] | T[0])) != (L[0] | T[0]));
}

static void expire(unsigned seconds)
{
    while (seconds--)
        for (unsigned tick = 0; tick < 1000 / TICK_INTERVAL_MS; ++tick)
            SOFT_TIMER_Run();
}

int main(void)
{
    const fsm_state_t automatic[] = {
        AUTO_GREEN, AUTO_YELLOW, AUTO_CROSS_GREEN, AUTO_CROSS_YELLOW};
    const light_state_t straight_colors[] = {
        LED_GREEN, LED_YELLOW, LED_RED, LED_RED};
    const light_state_t cross_colors[] = {
        LED_RED, LED_RED, LED_GREEN, LED_YELLOW};

    for (unsigned i = 0; i < 4; ++i)
    {
        state = automatic[i];
        SOFT_TIMER_Set(TRAFFIC_TIMER, 1000);
        FSM_Automatic_Run();
        assert(state == automatic[i]);
        SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
        expire(1);
        FSM_Automatic_Run();
        unsigned next = (i + 1) % 4;
        assert(state == automatic[next]);
        assert(SOFT_TIMER_Remaining(COUNTDOWN_TIMER) == 0);
        assert(SOFT_TIMER_Expired(COUNTDOWN_TIMER));
        /* Outputs follow the new state on the next FSM call. */
        FSM_Automatic_Run();
        assert(light_system.straight_color == straight_colors[next]);
        assert(light_system.cross_color == cross_colors[next]);
        assert(SOFT_TIMER_Remaining(TRAFFIC_TIMER) ==
               (next % 2 ? yellow_s : green_s) * 1000);

        /* A mode press wins even when the state timer has expired. */
        state = automatic[i];
        SOFT_TIMER_Set(TRAFFIC_TIMER, 10);
        SOFT_TIMER_Run();
        buttons[0].press_flag = 1;
        buttons[1].press_flag = 1;
        buttons[2].press_flag = 1;
        FSM_Automatic_Run();
        assert(state == MAN_GREEN);
        FSM_Manual_Run();
        assert(digits[0] == 10 && digits[1] == 10 && digits[2] == 10 && digits[3] == 10);
        assert(light_system.straight_color == LED_GREEN && light_system.cross_color == LED_RED);
        assert(!buttons[0].press_flag && !buttons[1].press_flag && !buttons[2].press_flag);
        assert(SOFT_TIMER_Remaining(TRAFFIC_TIMER) == 0);
        assert(SOFT_TIMER_Remaining(COUNTDOWN_TIMER) == 0);
        assert(!SOFT_TIMER_Expired(TRAFFIC_TIMER));
    }
    const fsm_state_t manual[] = {
        MAN_GREEN, MAN_YELLOW, MAN_CROSS_GREEN, MAN_CROSS_YELLOW};
    for (unsigned i = 0; i < 4; ++i)
    {
        state = manual[i];
        FSM_Manual_Run();
        assert(state == manual[i]);
        buttons[1].press_flag = 1;
        FSM_Manual_Run();
        unsigned next = (i + 1) % 4;
        assert(state == manual[next] && !buttons[1].press_flag);
        assert(light_system.straight_color == straight_colors[next]);
        assert(light_system.cross_color == cross_colors[next]);
        assert(digits[0] == 10 && digits[1] == 10 && digits[2] == 10 && digits[3] == 10);

        state = manual[i];
        buttons[0].press_flag = buttons[1].press_flag = 1;
        FSM_Manual_Run();
        assert(state == SET_RED && blink_on == 1);
        FSM_Setting_Run();
        assert(draft == green_s + yellow_s);
        assert(light_system.straight_color == LED_RED && light_system.cross_color == LED_RED);
        assert(!buttons[0].press_flag && !buttons[1].press_flag);
        assert(SOFT_TIMER_Remaining(BLINK_TIMER) == 250);
        assert(digits[0] == 0 && digits[1] == 2 && digits[2] == 0 && digits[3] == 5);
    }
    state = SET_RED;
    draft = 7;
    buttons[2].press_flag = 1;
    FSM_Setting_Run();
    assert(state == SET_YELLOW && green_s == 4 && draft == 3);
    draft = 99;
    buttons[2].press_flag = 1;
    FSM_Setting_Run();
    assert(state == SET_YELLOW && yellow_s == 3);
    draft = 2;
    buttons[2].press_flag = 1;
    FSM_Setting_Run();
    assert(state == SET_GREEN && yellow_s == 2 && draft == 4);
    blink_on = 0;
    expire(1);
    FSM_Setting_Run();
    FSM_Setting_Run();
    assert(light_system.straight_color == LED_GREEN && light_system.cross_color == LED_GREEN);
    draft = 5;
    buttons[2].press_flag = 1;
    FSM_Setting_Run();
    assert(state == SET_RED && green_s == 5 && draft == 7);
    draft = 99;
    buttons[1].press_flag = 1;
    FSM_Setting_Run();
    assert(draft == yellow_s + 1);
    state = SET_YELLOW;
    draft = 99 - green_s;
    buttons[1].press_flag = 1;
    FSM_Setting_Run();
    assert(draft == 1);
    buttons[1].press_flag = 1;
    FSM_Setting_Run();
    assert(draft == 2);
    state = SET_GREEN;
    draft = 99 - yellow_s;
    buttons[1].press_flag = 1;
    FSM_Setting_Run();
    assert(draft == 1);
    buttons[0].press_flag = 1;
    FSM_Setting_Run();
    assert(state == AUTO_GREEN && green_s == 5 && yellow_s == 2);
    assert(SOFT_TIMER_Remaining(TRAFFIC_TIMER) == 5000);
    assert(SOFT_TIMER_Remaining(COUNTDOWN_TIMER) == 1000);
    assert(SOFT_TIMER_Remaining(BLINK_TIMER) == 0);
    return 0;
}
