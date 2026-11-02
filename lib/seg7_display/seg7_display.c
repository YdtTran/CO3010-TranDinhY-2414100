#include "seg7_display.h"
#include "seg7.h"

void SEG7_Init(seg7_display_t *display)
{
  display->current_digit = 0;
  for (uint8_t i = 0; i < display->num; i++)
  {
    HAL_GPIO_WritePin(display->en_port, display->en_pin[i], GPIO_PIN_SET);
  }
  SEG7_SetDigit(display->segs, display->seg_port, 10U, display->com);
}

void SEG7_Update(seg7_display_t *display, int index)
{
  for (uint8_t i = 0; i < display->num; i++)
  {
    HAL_GPIO_WritePin(display->en_port, display->en_pin[i], GPIO_PIN_SET);
  }

  if (index < 0 || index >= display->num)
    return;

  SEG7_SetDigit(display->segs, display->seg_port, display->buffer[index], display->com);

  HAL_GPIO_WritePin(display->en_port, display->en_pin[index], GPIO_PIN_RESET);
}

void SEG7_UpdateBuffer(seg7_display_t *display, const uint8_t *buffer)
{
  for (uint8_t i = 0; i < display->num; i++)
  {
    display->buffer[i] = buffer[i];
  }
}

void SEG7_Scan(seg7_display_t *display)
{
  if (display->num == 0)
    return;

  SEG7_Update(display, display->current_digit);
  display->current_digit = (display->current_digit + 1) % display->num;
}
