#include "fsm.h"
#include "software_timer.h"
void FSM_Manual_Init(void)
{
    FSM_ClearButtons();
    SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
    SOFT_TIMER_Set(COUNTDOWN_TIMER, 0);
    SOFT_TIMER_Set(BLINK_TIMER, 0);
    app->state = MAN_GREEN;
    FSM_SetPhase(0);
}
void FSM_Manual_Run(void)
{
    if (app->state < MAN_GREEN || app->state > MAN_CROSS_YELLOW)
        return;
    if (BUTTON_GetPressFlag(&app->buttons[0]))
    {
        FSM_Setting_Init();
        return;
    }
    if (BUTTON_GetPressFlag(&app->buttons[1]))
    {
        BUTTON_ClearPressFlag(&app->buttons[1]);
        app->state = MAN_GREEN + (app->state - MAN_GREEN + 1) % 4;
        FSM_SetPhase(app->state - MAN_GREEN);
    }
}
