#include "software_timer.h"

static volatile uint8_t timer_flag[SOFT_TIMER_NUM_MAX] = {0};
static volatile uint32_t timer_counter[SOFT_TIMER_NUM_MAX] = {0};

void SOFT_TIMER_Set(uint8_t timer_index, uint32_t duration_ms)
{
  if (timer_index >= SOFT_TIMER_NUM_MAX)
    return;

  timer_counter[timer_index] = (duration_ms + TICK_INTERVAL_MS - 1) / TICK_INTERVAL_MS;
  timer_flag[timer_index] = 0;                                 // Clear the expiration flag
}

uint32_t SOFT_TIMER_Remaining(uint8_t timer_index)
{
  if (timer_index >= SOFT_TIMER_NUM_MAX) return 0;
  return timer_counter[timer_index] * TICK_INTERVAL_MS;
}

void SOFT_TIMER_Run(void)
{
  for (uint8_t i = 0; i < SOFT_TIMER_NUM_MAX; i++)
  {
    if (timer_counter[i] > 0)
    {
      --timer_counter[i];
      if (timer_counter[i] == 0)
      {
        timer_flag[i] = 1; // set the flag when the timer expires
      }
    }
  }
}

uint8_t SOFT_TIMER_Expired(uint8_t timer_index)
{
  if (timer_index >= SOFT_TIMER_NUM_MAX || !timer_flag[timer_index])
    return 0;

  timer_flag[timer_index] = 0;
  return 1;
}
