#include "fsm.h"
#include "main.h"
#include "pin.h"
#include "software_timer.h"
void FSM_SetPhase(uint8_t phase)
{
    static const light_state_t colors[4][2] = {
        {LED_GREEN, LED_RED}, {LED_YELLOW, LED_RED}, {LED_RED, LED_GREEN}, {LED_RED, LED_YELLOW}};
    uint16_t times[4][2] = {
        {app->green_s, app->green_s + app->yellow_s},
        {app->yellow_s, app->yellow_s},
        {app->green_s + app->yellow_s, app->green_s},
        {app->yellow_s, app->yellow_s}};
    /* Blank both directions before enabling either new color. */
    TRAFFIC_LIGHT_SetColor(&app->straight, LED_OFF);
    TRAFFIC_LIGHT_SetColor(&app->cross, LED_OFF);
    TRAFFIC_LIGHT_SetColor(&app->straight, colors[phase][0]);
    TRAFFIC_LIGHT_SetColor(&app->cross, colors[phase][1]);
    TRAFFIC_LIGHT_SetTime(&app->straight, times[phase][0]);
    TRAFFIC_LIGHT_SetTime(&app->cross, times[phase][1]);
}
static void FSM_UpdateDisplay(void)
{
    if (app->state >= SET_RED)
        return;
    uint16_t first = app->straight.remaining_s, second = app->cross.remaining_s;
    app->digits[0] = first / 10;
    app->digits[1] = first % 10;
    app->digits[2] = second / 10;
    app->digits[3] = second % 10;
}
void FSM_Init(fsm_t *fsm)
{
    app = fsm;
    app->green_s = 2;
    app->yellow_s = 3;
    app->straight = (traffic_light_t){.port = L_PORT, .gpio = {L[0], L[1], L[2]}};
    app->cross = (traffic_light_t){.port = T_PORT, .gpio = {T[0], T[1], T[2]}};
    if (app->straight.port == app->cross.port &&
        ((L[0] | L[1] | L[2]) & (T[0] | T[1] | T[2])))
        Error_Handler();
    if (!TRAFFIC_LIGHT_Init(&app->straight) || !TRAFFIC_LIGHT_Init(&app->cross))
        Error_Handler();
    for (unsigned i = 0; i < 3; ++i)
    {
        BUTTON_Init(&app->buttons[i], BTN_PORT, BTN[i], BTN_ACTIVE_STATE, 0);
    }
    app->display = (seg7_display_t){
        .segs = {SEG[0], SEG[1], SEG[2], SEG[3], SEG[4], SEG[5], SEG[6], 0},
        .en_pin = EN,
        .seg_port = SEG_PORT,
        .en_port = EN_PORT,
        .buffer = app->digits,
        .num = 4,
        .com = SEG7_COMMON_ANODE};
    SEG7_Init(&app->display);
    SOFT_TIMER_Set(DISPLAY_TIMER, 20);
    SOFT_TIMER_Set(INDICATOR_TIMER, 250);
    SOFT_TIMER_Set(BLINK_TIMER, 0);
    FSM_Automatic_Init();
    FSM_UpdateDisplay();
}
void FSM_ServiceOutputs(void)
{
    FSM_UpdateDisplay();
    if (SOFT_TIMER_Expired(DISPLAY_TIMER))
    {
        SEG7_Scan(&app->display);
        SOFT_TIMER_Set(DISPLAY_TIMER, 20);
    }
    if (SOFT_TIMER_Expired(INDICATOR_TIMER))
    {
        HAL_GPIO_TogglePin(INDICATOR_PORT, INDICATOR);
        SOFT_TIMER_Set(INDICATOR_TIMER, 250);
    }
}
void FSM_ScanButtons(void)
{
    for (unsigned i = 0; i < 3; ++i)
        BUTTON_Scan(&app->buttons[i]);
}
void FSM_ClearButtons(void)
{
    for (unsigned i = 0; i < 3; ++i)
        BUTTON_ClearPressFlag(&app->buttons[i]);
}
