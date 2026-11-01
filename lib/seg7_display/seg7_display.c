#include "seg7_display.h"
#include "seg7.h"

static uint16_t seg_pins[8] = {
    SEG_A_PIN, SEG_B_PIN, SEG_C_PIN, SEG_D_PIN,
    SEG_E_PIN, SEG_F_PIN, SEG_G_PIN, SEG_DP_PIN};

static uint16_t digit_en_pins[4] = {
    DIGIT_EN0_PIN, // EN0
    DIGIT_EN1_PIN, // EN1
    DIGIT_EN2_PIN, // EN2
    DIGIT_EN3_PIN  // EN3
};

static uint8_t led_buffer[4] = {1, 2, 3, 0};

static uint8_t current_seg7_digit = 0;
static seg7_handle_t seg7_handler = {0};

void init7SEG(void)
{
  SEG7Init(&seg7_handler, SEG7_COMMON_ANODE, SEG_PORT, seg_pins);
}

void update7SEG(int index)
{
  if (index < 0 || index >= LED_NUM)
    return;

  for (uint8_t i = 0; i < LED_NUM; i++)
  {
    HAL_GPIO_WritePin(DIGIT_EN_PORT, digit_en_pins[i], GPIO_PIN_SET);
  }

  HAL_GPIO_WritePin(DIGIT_EN_PORT, digit_en_pins[index], GPIO_PIN_RESET);

  SEG7Set_Digit(&seg7_handler, led_buffer[index]);
}

void updateClockBuffer(int hour, int minute)
{
  led_buffer[0] = hour / 10;
  led_buffer[1] = hour - 10 * led_buffer[0];
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute - 10 * led_buffer[2];
}

void scan7SEG(void)
{
  update7SEG(current_seg7_digit);
  current_seg7_digit = (current_seg7_digit + 1) % LED_NUM;
}
