#include "fsm.h"
#include "software_timer.h"
static light_state_t selected;
static uint8_t draft, blink_on;
static void show_lights(void)
{
    TRAFFIC_LIGHT_SetColor(&app->straight, LED_OFF);
    TRAFFIC_LIGHT_SetColor(&app->cross, LED_OFF);
    if (blink_on) {
        TRAFFIC_LIGHT_SetColor(&app->straight, selected);
        TRAFFIC_LIGHT_SetColor(&app->cross, selected);
    }
}
static void enter_edit(void)
{
    switch (selected) {
    case LED_RED: draft = app->green_s + app->yellow_s; break;
    case LED_YELLOW: draft = app->yellow_s; break;
    default: draft = app->green_s; break;
    }
    blink_on = 1U;
    show_lights();
    SOFT_TIMER_Set(BLINK_TIMER, 250U);
}
void FSM_Setting_Init(void) { selected = LED_RED; enter_edit(); }
void FSM_Setting_Run(void)
{
    if (app->mode != MODE_SETTING) return;
    if (app->events[1]) draft = draft == 99U ? 1U : draft + 1U;
    if (app->events[2]) {
        uint16_t green = app->green_s, yellow = app->yellow_s;
        switch (selected) {
        case LED_RED:
            green = 0U;
            if (draft > yellow) green = draft - yellow;
            break;
        case LED_YELLOW: yellow = draft; break;
        default: green = draft; break;
        }
        if (green >= 1U && yellow >= 1U && green + yellow <= 99U) {
            app->green_s = green;
            app->yellow_s = yellow;
            switch (selected) {
            case LED_RED: selected = LED_YELLOW; break;
            case LED_YELLOW: selected = LED_GREEN; break;
            default: selected = LED_RED; break;
            }
            enter_edit();
        }
    }
    if (SOFT_TIMER_Expired(BLINK_TIMER)) {
        blink_on = !blink_on;
        show_lights();
        SOFT_TIMER_Set(BLINK_TIMER, 250U);
    }
    /* Preserve setting identifiers 02/03/04 for red/yellow/green. */
    app->digits[0] = 0U;
    switch (selected) {
    case LED_RED: app->digits[1] = 2U; break;
    case LED_YELLOW: app->digits[1] = 3U; break;
    default: app->digits[1] = 4U; break;
    }
    app->digits[2] = draft / 10U;
    app->digits[3] = draft % 10U;
}
