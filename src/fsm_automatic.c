#include "fsm_automatic.h"
#include "software_timer.h"

void FSM_Automatic_Run(void)
{
    switch (state)
    {
    case AUTO_GREEN:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_GREEN, LED_RED);
        if (SOFT_TIMER_Expired(TRAFFIC_TIMER))
        {
            state = AUTO_YELLOW;

            SEG7_SetNumbers(&seg7_display, yellow_s, yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, yellow_s * 1000);
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = MAN_GREEN;
            SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 0);

        }
        break;

    case AUTO_YELLOW:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_YELLOW, LED_RED);
        if (SOFT_TIMER_Expired(TRAFFIC_TIMER))
        {
            state = AUTO_CROSS_GREEN;

            SEG7_SetNumbers(&seg7_display, green_s + yellow_s, green_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, green_s * 1000);
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = MAN_GREEN;
            SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 0);

        }
        break;

    case AUTO_CROSS_GREEN:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_RED, LED_GREEN);
        if (SOFT_TIMER_Expired(TRAFFIC_TIMER))
        {
            state = AUTO_CROSS_YELLOW;

            SEG7_SetNumbers(&seg7_display, yellow_s, yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, yellow_s * 1000);
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = MAN_GREEN;
            SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 0);

        }
        break;

    case AUTO_CROSS_YELLOW:
        TRAFFIC_LIGHT_SetColor(&light_system, LED_RED, LED_YELLOW);
        if (SOFT_TIMER_Expired(TRAFFIC_TIMER))
        {
            state = AUTO_GREEN;

            SEG7_SetNumbers(&seg7_display, green_s, green_s + yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, green_s * 1000);
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = MAN_GREEN;
            SOFT_TIMER_Set(TRAFFIC_TIMER, 0);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 0);

        }
        break;

    default:
        break;
    }
}
