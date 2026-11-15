#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

#ifndef TICK_INTERVAL_MS
#define TICK_INTERVAL_MS 10
#endif

_Static_assert(TICK_INTERVAL_MS >= 1 && TICK_INTERVAL_MS <= 100,
               "TIM2 interval must be between 1 and 100 ms");
#define SOFT_TIMER_NUM_MAX 10

void SOFT_TIMER_Set(uint8_t timer_index, uint32_t duration_ms);
uint32_t SOFT_TIMER_Remaining(uint8_t timer_index);
void SOFT_TIMER_Run(void);
/* Return and clear a timer's expiration flag. Call from the main loop. */
uint8_t SOFT_TIMER_Expired(uint8_t timer_index);

#endif /* SOFTWARE_TIMER_H */
