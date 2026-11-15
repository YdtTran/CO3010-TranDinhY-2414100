#include "fsm_setting.h"
#include "software_timer.h"

void FSM_Setting_Run(void)
{
    switch (state)
    {
    case SET_RED:
        if (blink_on)
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_RED, LED_RED);
        }
        else
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_OFF, LED_OFF);
        }
        if (SOFT_TIMER_Expired(BLINK_TIMER))
        {
            blink_on = !blink_on;
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            ++draft;
            if (draft > 99)
                draft = yellow_s + 1;
            SEG7_SetNumbers(&seg7_display, 2, draft);
        }
        if (BUTTON_GetPressFlag(&buttons[2]))
        {
            BUTTON_ClearPressFlag(&buttons[2]);
            if (draft > yellow_s && draft <= 99)
            {
                green_s = draft - yellow_s;
                state = SET_YELLOW;
                draft = yellow_s;
                blink_on = 1;

                SEG7_SetNumbers(&seg7_display, 3, draft);
                SOFT_TIMER_Set(BLINK_TIMER, 250);
            }
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = AUTO_GREEN;
            SOFT_TIMER_Set(BLINK_TIMER, 0);
            SEG7_SetNumbers(&seg7_display, green_s, green_s + yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, green_s * 1000);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
        }
        break;

    case SET_YELLOW:
        if (blink_on)
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_YELLOW, LED_YELLOW);
        }
        else
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_OFF, LED_OFF);
        }
        if (SOFT_TIMER_Expired(BLINK_TIMER))
        {
            blink_on = !blink_on;
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            ++draft;
            if (draft > 99 - green_s)
                draft = 1;
            SEG7_SetNumbers(&seg7_display, 3, draft);
        }
        if (BUTTON_GetPressFlag(&buttons[2]))
        {
            BUTTON_ClearPressFlag(&buttons[2]);
            if (draft > 0 && green_s + draft <= 99)
            {
                yellow_s = draft;
                state = SET_GREEN;
                draft = green_s;
                blink_on = 1;

                SEG7_SetNumbers(&seg7_display, 4, draft);
                SOFT_TIMER_Set(BLINK_TIMER, 250);
            }
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = AUTO_GREEN;
            SOFT_TIMER_Set(BLINK_TIMER, 0);
            SEG7_SetNumbers(&seg7_display, green_s, green_s + yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, green_s * 1000);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
        }
        break;

    case SET_GREEN:
        if (blink_on)
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_GREEN, LED_GREEN);
        }
        else
        {
            TRAFFIC_LIGHT_SetColor(&light_system, LED_OFF, LED_OFF);
        }
        if (SOFT_TIMER_Expired(BLINK_TIMER))
        {
            blink_on = !blink_on;
            SOFT_TIMER_Set(BLINK_TIMER, 250);
        }
        if (BUTTON_GetPressFlag(&buttons[1]))
        {
            BUTTON_ClearPressFlag(&buttons[1]);
            ++draft;
            if (draft > 99 - yellow_s)
                draft = 1;
            SEG7_SetNumbers(&seg7_display, 4, draft);
        }
        if (BUTTON_GetPressFlag(&buttons[2]))
        {
            BUTTON_ClearPressFlag(&buttons[2]);
            if (draft > 0 && draft + yellow_s <= 99)
            {
                green_s = draft;
                state = SET_RED;
                draft = green_s + yellow_s;
                blink_on = 1;

                SEG7_SetNumbers(&seg7_display, 2, draft);
                SOFT_TIMER_Set(BLINK_TIMER, 250);
            }
        }
        if (BUTTON_GetPressFlag(&buttons[0]))
        {
            BUTTON_ClearPressFlags(buttons, 3);
            state = AUTO_GREEN;
            SOFT_TIMER_Set(BLINK_TIMER, 0);
            SEG7_SetNumbers(&seg7_display, green_s, green_s + yellow_s);
            SOFT_TIMER_Set(TRAFFIC_TIMER, green_s * 1000);
            SOFT_TIMER_Set(COUNTDOWN_TIMER, 1000);
        }
        break;

    default:
        break;
    }
}
