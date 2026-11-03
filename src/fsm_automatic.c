#include "fsm.h"
#include "software_timer.h"
static void set_phase(fsm_state_t state)
{
    app->state = state;
    uint8_t phase = state - AUTO_GREEN;
    FSM_SetPhase(phase);
    /* Preserve the display's N-1..0 convention. */
    TRAFFIC_LIGHT_DecreaseTime(&app->straight);
    TRAFFIC_LIGHT_DecreaseTime(&app->cross);
    SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
    if (phase % 2)
        SOFT_TIMER_Set(TRAFFIC_TIMER, app->yellow_s * 1000);
    else
        SOFT_TIMER_Set(TRAFFIC_TIMER, app->green_s * 1000);
}
void FSM_Automatic_Init(void)
{
    FSM_ClearButtons();
    SOFT_TIMER_Set(BLINK_TIMER, 0);
    set_phase(AUTO_GREEN);
}
void FSM_Automatic_Run(void)
{
    if (app->state > AUTO_CROSS_YELLOW)
        return;
    if (BUTTON_GetPressFlag(&app->buttons[0]))
    {
        FSM_Manual_Init();
        return;
    }
    if (SOFT_TIMER_Expired(TRAFFIC_TIMER))
        set_phase(AUTO_GREEN + (app->state - AUTO_GREEN + 1) % 4);
    if (SOFT_TIMER_Expired(COUNTDOWN_TIMER))
    {
        TRAFFIC_LIGHT_DecreaseTime(&app->straight);
        TRAFFIC_LIGHT_DecreaseTime(&app->cross);
        SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
    }
}
