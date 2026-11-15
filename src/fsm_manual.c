#include "fsm_manual.h"
#include "software_timer.h"

void FSM_Manual_Run(void)
{
    switch (state)
    {
    case MAN_GREEN:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_GREEN, LED_RED);
        SEG7_Clear(&seg7_display);
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            state = MAN_YELLOW;

        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = SET_RED;
            draft = green_s + yellow_s;
            blink_on = 1;

            SEG7_SetNumbers(&seg7_display, 2, draft);
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        break;

    case MAN_YELLOW:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_YELLOW, LED_RED);
        SEG7_Clear(&seg7_display);
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            state = MAN_CROSS_GREEN;

        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = SET_RED;
            draft = green_s + yellow_s;
            blink_on = 1;

            SEG7_SetNumbers(&seg7_display, 2, draft);
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        break;

    case MAN_CROSS_GREEN:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_RED, LED_GREEN);
        SEG7_Clear(&seg7_display);
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            state = MAN_CROSS_YELLOW;

        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = SET_RED;
            draft = green_s + yellow_s;
            blink_on = 1;

            SEG7_SetNumbers(&seg7_display, 2, draft);
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        break;

    case MAN_CROSS_YELLOW:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_RED, LED_YELLOW);
        SEG7_Clear(&seg7_display);
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            state = MAN_GREEN;

        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = SET_RED;
            draft = green_s + yellow_s;
            blink_on = 1;

            SEG7_SetNumbers(&seg7_display, 2, draft);
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        break;

    default:
        break;
    }
}
