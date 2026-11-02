#include "led_matrix.h"
#include "stm32f1xx_hal.h"

static uint16_t matrix_row_pins[8] = {
    MATRIX_ROW0_PIN,  // ROW0
    MATRIX_ROW1_PIN,  // ROW1
    MATRIX_ROW2_PIN, // ROW2
    MATRIX_ROW3_PIN, // ROW3
    MATRIX_ROW4_PIN, // ROW4
    MATRIX_ROW5_PIN, // ROW5
    MATRIX_ROW6_PIN, // ROW6
    MATRIX_ROW7_PIN  // ROW7
};

static uint16_t matrix_col_pins[8] = {
    MATRIX_COL0_PIN,  // ENM0 -> COL0
    MATRIX_COL1_PIN,  // ENM1 -> COL1
    MATRIX_COL2_PIN, // ENM2 -> COL2
    MATRIX_COL3_PIN, // ENM3 -> COL3
    MATRIX_COL4_PIN, // ENM4 -> COL4
    MATRIX_COL5_PIN, // ENM5 -> COL5
    MATRIX_COL6_PIN, // ENM6 -> COL6
    MATRIX_COL7_PIN  // ENM7 -> COL7
};

static uint8_t matrix_bitmap[8] = {
    0x00, // Col 0: 00000000
    0xFC, // Col 1: 11111100
    0x12, // Col 2: 00010010
    0x11, // Col 3: 00010001
    0x11, // Col 4: 00010001
    0x12, // Col 5: 00010010
    0xFC, // Col 6: 11111100
    0x00  // Col 7: 00000000
};

static uint8_t current_matrix_col = 0;

void LED_MATRIX_UpdateRows(uint8_t col_index)
{
  if (col_index >= MAX_LED_MATRIX)
    return;

  // Turn off all columns first
  LED_MATRIX_Clear();

  // Set the rows based on the bitmap for the current column
  HAL_GPIO_WritePin(MATRIX_COL_PORT, matrix_col_pins[col_index], GPIO_PIN_RESET);
  uint8_t row_data = matrix_bitmap[col_index];
  for (uint8_t row = 0; row < MAX_LED_MATRIX; row++)
  {
    GPIO_PinState pin_state = (row_data & (1 << row)) ? GPIO_PIN_RESET : GPIO_PIN_SET;
    HAL_GPIO_WritePin(MATRIX_ROW_PORT, matrix_row_pins[row], pin_state);
  }

  // Enable the current column
}

void LED_MATRIX_Clear(void)
{
  for (uint8_t i = 0; i < MAX_LED_MATRIX; i++)
  {
    HAL_GPIO_WritePin(MATRIX_COL_PORT, matrix_col_pins[i], GPIO_PIN_SET);
  }
  for (uint8_t row = 0; row < MAX_LED_MATRIX; row++)
  {
    HAL_GPIO_WritePin(MATRIX_ROW_PORT, matrix_row_pins[row], GPIO_PIN_SET);
  }
}

void LED_MATRIX_Update(uint8_t index)
{
  switch (index)
  {
  case 0:
    LED_MATRIX_UpdateRows(0);
    break;

  case 1:
    LED_MATRIX_UpdateRows(1);
    break;

  case 2:
    LED_MATRIX_UpdateRows(2);
    break;

  case 3:
    LED_MATRIX_UpdateRows(3);
    break;

  case 4:
    LED_MATRIX_UpdateRows(4);
    break;

  case 5:
    LED_MATRIX_UpdateRows(5);
    break;

  case 6:
    LED_MATRIX_UpdateRows(6);
    break;

  case 7:
    LED_MATRIX_UpdateRows(7);
    break;

  default:
    break;
  }
}

void LED_MATRIX_Shift(void)
{
  uint8_t tmp = matrix_bitmap[MAX_LED_MATRIX - 1];
  for (uint8_t i = MAX_LED_MATRIX - 1; i > 0; i--)
  {
    matrix_bitmap[i] = matrix_bitmap[i - 1];
  }
  matrix_bitmap[0] = tmp;
}

void LED_MATRIX_Scan(void)
{
  LED_MATRIX_Update(current_matrix_col);
  current_matrix_col = (current_matrix_col + 1) % MAX_LED_MATRIX;
}
