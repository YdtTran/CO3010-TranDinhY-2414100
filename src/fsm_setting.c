#include "fsm.h"
#include "software_timer.h"
static uint8_t draft, blink_on;
static void show_lights(void)
{
    static const light_state_t colors[] = {LED_RED, LED_YELLOW, LED_GREEN};
    TRAFFIC_LIGHT_SetColor(&app->straight, LED_OFF);
    TRAFFIC_LIGHT_SetColor(&app->cross, LED_OFF);
    if (blink_on)
    {
        TRAFFIC_LIGHT_SetColor(&app->straight, colors[app->state - SET_RED]);
        TRAFFIC_LIGHT_SetColor(&app->cross, colors[app->state - SET_RED]);
    }
}
static void enter_edit(fsm_state_t state)
{
    app->state = state;
    switch (state)
    {
    case SET_RED:
        draft = app->green_s + app->yellow_s;
        break;
    case SET_YELLOW:
        draft = app->yellow_s;
        break;
    default:
        draft = app->green_s;
        break;
    }
    blink_on = 1;
    show_lights();
    SOFT_TIMER_Set(BLINK_TIMER, 250);
}
void FSM_Setting_Init(void)
{
    FSM_ClearButtons();
    SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
    enter_edit(SET_RED);
}
void FSM_Setting_Run(void)
{
    switch (app->state)
    {
    case SET_RED:
        if (BUTTON_GetPressFlag(&app->buttons[0]))
        {
            FSM_Automatic_Init();
            return;
        }
        if (BUTTON_GetPressFlag(&app->buttons[1]))
        {
            BUTTON_ClearPressFlag(&app->buttons[1]);
            draft = draft % 99 + 1;
        }
        if (BUTTON_GetPressFlag(&app->buttons[2]))
        {
            BUTTON_ClearPressFlag(&app->buttons[2]);
            app->green_s = draft > app->yellow_s ? draft - app->yellow_s : 0;
            enter_edit(SET_YELLOW);
        }
        break;
    case SET_YELLOW:
        if (BUTTON_GetPressFlag(&app->buttons[0]))
        {
            FSM_Automatic_Init();
            return;
        }
        if (BUTTON_GetPressFlag(&app->buttons[1]))
        {
            BUTTON_ClearPressFlag(&app->buttons[1]);
            draft = draft % 99 + 1;
        }
        if (BUTTON_GetPressFlag(&app->buttons[2]))
        {
            BUTTON_ClearPressFlag(&app->buttons[2]);
            app->yellow_s = draft;
            enter_edit(SET_GREEN);
        }
        break;
    case SET_GREEN:
        if (BUTTON_GetPressFlag(&app->buttons[0]))
        {
            FSM_Automatic_Init();
            return;
        }
        if (BUTTON_GetPressFlag(&app->buttons[1]))
        {
            BUTTON_ClearPressFlag(&app->buttons[1]);
            draft = draft % 99 + 1;
        }
        if (BUTTON_GetPressFlag(&app->buttons[2]))
        {
            BUTTON_ClearPressFlag(&app->buttons[2]);
            app->green_s = draft;
            enter_edit(SET_RED);
        }
        break;
    default:
        return;
    }
    if (SOFT_TIMER_Expired(BLINK_TIMER))
    {
        blink_on = !blink_on;
        show_lights();
        SOFT_TIMER_Set(BLINK_TIMER, 250);
    }
    /* Preserve setting identifiers 02/03/04 for red/yellow/green. */
    app->digits[0] = 0;
    app->digits[1] = 2 + app->state - SET_RED;
    app->digits[2] = draft / 10;
    app->digits[3] = draft % 10;
}
