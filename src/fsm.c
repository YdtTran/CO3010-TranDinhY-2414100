#include "fsm.h"
#include "main.h"
#include "pin.h"
#include "software_timer.h"
void FSM_SetPhase(uint8_t phase)
{
    static const light_state_t colors[4][2] = {
        {LED_GREEN, LED_RED}, {LED_YELLOW, LED_RED},
        {LED_RED, LED_GREEN}, {LED_RED, LED_YELLOW}};
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
void FSM_UpdateDisplay(void)
{
    if (app->mode == MODE_SETTING) return;
    uint16_t first = app->straight.remaining_s, second = app->cross.remaining_s;
    /* Preserve AUTO's existing N-1..0 display convention. */
    if (app->mode == MODE_AUTOMATIC) {
        if (first) --first;
        if (second) --second;
    }
    app->digits[0] = first / 10U;
    app->digits[1] = first % 10U;
    app->digits[2] = second / 10U;
    app->digits[3] = second % 10U;
}
void FSM_Init(fsm_t *fsm)
{
    app = fsm;
    app->mode = MODE_AUTOMATIC;
    app->green_s = 2U;
    app->yellow_s = 3U;
    app->straight = (traffic_light_t){.port = L_PORT, .gpio = {L[0], L[1], L[2]}};
    app->cross = (traffic_light_t){.port = T_PORT, .gpio = {T[0], T[1], T[2]}};
    if (app->straight.port == app->cross.port &&
        ((L[0] | L[1] | L[2]) & (T[0] | T[1] | T[2]))) Error_Handler();
    if (!TRAFFIC_LIGHT_Init(&app->straight) || !TRAFFIC_LIGHT_Init(&app->cross)) Error_Handler();
    for (unsigned i = 0; i < 3U; ++i) {
        BUTTON_Init(&app->buttons[i], BTN_PORT, BTN[i], BTN_ACTIVE_STATE, 0U);
        app->events[i] = 0U;
    }
    app->display = (seg7_display_t){
        .segs = {SEG[0], SEG[1], SEG[2], SEG[3], SEG[4], SEG[5], SEG[6], 0U},
        .en_pin = EN, .seg_port = SEG_PORT, .en_port = EN_PORT,
        .buffer = app->digits, .num = 4U, .com = SEG7_COMMON_ANODE};
    SEG7_Init(&app->display);
    SOFT_TIMER_Set(BUTTON_TIMER, TICK_INTERVAL_MS);
    SOFT_TIMER_Set(BLINK_TIMER, 0U);
    FSM_Automatic_Init();
    FSM_UpdateDisplay();
}
void FSM_CollectEvents(void)
{
    if (SOFT_TIMER_Expired(BUTTON_TIMER)) {
        for (unsigned i = 0; i < 3U; ++i) BUTTON_Scan(&app->buttons[i]);
        SOFT_TIMER_Set(BUTTON_TIMER, TICK_INTERVAL_MS);
    }
    for (unsigned i = 0; i < 3U; ++i) {
        app->events[i] = BUTTON_GetPressFlag(&app->buttons[i]);
        BUTTON_ClearPressFlag(&app->buttons[i]);
    }
}
void FSM_Dispatch(void)
{
    if (!app->events[0]) return;
    app->mode = app->mode == MODE_SETTING ? MODE_AUTOMATIC : app->mode + 1;
    SOFT_TIMER_Set(TRAFFIC_TIMER, 0U);
    SOFT_TIMER_Set(BLINK_TIMER, 0U);
    app->buttons[1].repeat_ticks = app->mode == MODE_SETTING ?
        (250U + TICK_INTERVAL_MS - 1U) / TICK_INTERVAL_MS : 0U;
    app->buttons[1].timer_for_key_press = 0U;
    for (unsigned i = 0; i < 3U; ++i) app->events[i] = 0U;
    switch (app->mode) {
    case MODE_AUTOMATIC: FSM_Automatic_Init(); break;
    case MODE_MANUAL: FSM_Manual_Init(); break;
    case MODE_SETTING: FSM_Setting_Init(); break;
    }
}
