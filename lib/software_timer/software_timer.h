#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

#ifndef TICK_INTERVAL_MS
#define TICK_INTERVAL_MS 10U
#endif

_Static_assert(TICK_INTERVAL_MS >= 1U && TICK_INTERVAL_MS <= 100U,
               "TIM2 interval must be between 1 and 100 ms");
#define SOFT_TIMER_NUM_MAX 10

void SOFT_TIMER_Set(uint8_t timer_index, uint16_t duration_ms);
void SOFT_TIMER_Run(void);
/* Return and clear a timer's expiration flag. Call from the main loop. */
uint8_t SOFT_TIMER_Expired(uint8_t timer_index);

#endif /* SOFTWARE_TIMER_H */
