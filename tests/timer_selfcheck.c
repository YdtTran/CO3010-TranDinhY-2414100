/* Target self-check: call before starting TIM2 in a test firmware. */
#include <assert.h>
#include "software_timer.h"

void timer_selfcheck(void)
{
    const uint32_t durations[] = {1, 50, 250, 1000, 99000};
    for (unsigned i = 0; i < sizeof durations / sizeof durations[0]; ++i)
    {
        SOFT_TIMER_Set(0, durations[i]);
        unsigned ticks = (durations[i] + TICK_INTERVAL_MS - 1) / TICK_INTERVAL_MS;
        assert(SOFT_TIMER_Remaining(0) == ticks * TICK_INTERVAL_MS);
        for (unsigned n = 1; n < ticks; ++n)
        {
            SOFT_TIMER_Run();
            assert(!SOFT_TIMER_Expired(0));
        }
        SOFT_TIMER_Run();
        assert(SOFT_TIMER_Expired(0));
        assert(SOFT_TIMER_Remaining(0) == 0);
        assert(!SOFT_TIMER_Expired(0));
    }
    SOFT_TIMER_Set(0, 0);
    SOFT_TIMER_Run();
    assert(!SOFT_TIMER_Expired(0));
    SOFT_TIMER_Set(SOFT_TIMER_NUM_MAX, 10);
    assert(!SOFT_TIMER_Expired(SOFT_TIMER_NUM_MAX));
}
