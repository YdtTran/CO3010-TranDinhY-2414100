#include "fsm.h"

void FSM_Init(fsm_t *fsm)
{
    fsm->mode = FSM_NORMAL;
}

void FSM_Run(fsm_t *fsm, uint8_t mode_pressed)
{
    if (!mode_pressed)
        return;

    switch (fsm->mode)
    {
    case FSM_NORMAL:
        fsm->mode = FSM_SET_RED;
        break;
    case FSM_SET_RED:
        fsm->mode = FSM_SET_AMBER;
        break;
    case FSM_SET_AMBER:
        fsm->mode = FSM_SET_GREEN;
        break;
    case FSM_SET_GREEN:
    default:
        fsm->mode = FSM_NORMAL;
        break;
    }
}