#include "app.h"
#include "led_matrix.h"
#include "seg7_display.h"
#include "software_timer.h"
#include "stm32f1xx_hal.h"

static int hour = 15, minute = 8, second = 50;

void appInit(void)
{
  init7SEG();
  for (uint8_t i = 0; i < SOFT_TIMER_NUM_MAX; i++)
  {
    setTimer(i, 0); // Reset all software timers to 0
  }
  clearMatrix();

  setTimer(0, 1000); // Set timer 0 for 1 second
  setTimer(1, 250);
  setTimer(2, 500); // Set timer 2 for 500 ms
  setTimer(3, 50);  // Set timer 3 for 50 ms
}

void appRun(void)
{
  if (timerExpired(0))
  {
    setTimer(0, 1000); // Reset timer 0 for another second
    second++;
    minute += second / 60;
    second %= 60;
    hour += minute / 60;
    minute %= 60;
    hour %= 24;
    updateClockBuffer(hour, minute);
    shiftLedMatrix();
  }
  if (timerExpired(1))
  {
    setTimer(1, 200); // Reset timer 1 for another 200 ms
    scan7SEG();
  }
  if (timerExpired(2))
  {
    setTimer(2, 500);                   // Reset timer 2 for another 500 ms
    HAL_GPIO_TogglePin(DOT_PORT, DOT_PIN); // Toggle the dot
  }
  if (timerExpired(3))
  {
    setTimer(3, 20); // Reset timer 3 for another 20 ms
    scanLedMatrix();
  }
}
