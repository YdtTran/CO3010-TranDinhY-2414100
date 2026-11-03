#include "fsm.h"
static uint8_t phase;
void FSM_Manual_Init(void) { phase = 0U; FSM_SetPhase(phase); }
void FSM_Manual_Run(void)
{
    if (app->mode != MODE_MANUAL) return;
    if (app->events[1]) {
        phase = (phase + 1U) % 4U;
        FSM_SetPhase(phase);
    }
}
