#include "fsm.h"
#include "software_timer.h"
static uint8_t phase;
void FSM_Automatic_Init(void)
{
    phase = 0U;
    FSM_SetPhase(phase);
    SOFT_TIMER_Set(TRAFFIC_TIMER, 1000U);
}
void FSM_Automatic_Run(void)
{
    if (app->mode != MODE_AUTOMATIC) return;
    if (SOFT_TIMER_Expired(TRAFFIC_TIMER)) {
        TRAFFIC_LIGHT_DecreaseTime(&app->straight);
        TRAFFIC_LIGHT_DecreaseTime(&app->cross);
        if (app->straight.remaining_s == 0U || app->cross.remaining_s == 0U) {
            phase = (phase + 1U) % 4U;
            FSM_SetPhase(phase);
        }
        SOFT_TIMER_Set(TRAFFIC_TIMER, 1000U);
    }
}
