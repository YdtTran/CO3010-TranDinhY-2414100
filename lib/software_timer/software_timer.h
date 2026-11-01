#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>

#define TICK_INTERVAL_MS 10
#define SOFT_TIMER_NUM_MAX 10

void setTimer(uint8_t timer_index, uint16_t duration_ms);
void timerRun(void);
/* Return and clear a timer's expiration flag. Call from the main loop. */
uint8_t timerExpired(uint8_t timer_index);

#endif /* SOFTWARE_TIMER_H */
