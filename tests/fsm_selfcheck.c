#include <assert.h>
#include "fsm.h"

int main(void)
{
    fsm_t fsm;
    FSM_Init(&fsm);
    assert(fsm.mode == FSM_NORMAL);
    const fsm_mode_t modes[] = {FSM_SET_RED, FSM_SET_AMBER, FSM_SET_GREEN, FSM_NORMAL};
    for (unsigned cycle = 0; cycle < 3U; ++cycle)
    {
        for (unsigned i = 0; i < 4U; ++i)
        {
            fsm_mode_t previous = fsm.mode;
            FSM_Run(&fsm, 0U);
            assert(fsm.mode == previous);
            FSM_Run(&fsm, 1U);
            assert(fsm.mode == modes[i]);
        }
    }
    fsm.mode = (fsm_mode_t)99;
    FSM_Run(&fsm, 1U);
    assert(fsm.mode == FSM_NORMAL);
    FSM_Run(&fsm, 1U);
    FSM_Init(&fsm);
    assert(fsm.mode == FSM_NORMAL);
    return 0;
}
