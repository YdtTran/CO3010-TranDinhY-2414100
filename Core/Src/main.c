/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "seg7.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MATRIX_ROW_PORT GPIOB
#define MATRIX_COL_PORT GPIOA
#define DIGIT_EN_PORT GPIOA
#define SEG_PORT GPIOB
#define LED_D1_PIN GPIO_PIN_5 // PA5: Active LOW
#define DOT_PIN GPIO_PIN_4    // PA4
#define LED_NUM 4
#define DOT GPIO_PIN_4
#define TICK_INTERVAL_MS 10
#define SOFT_TIMER_NUM_MAX 10
#define MAX_LED_MATRIX 8

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
/* Row pins PB8 -> PB15 (Anodes) */
static uint16_t matrix_row_pins[8] = {
    GPIO_PIN_8,  // ROW0
    GPIO_PIN_9,  // ROW1
    GPIO_PIN_10, // ROW2
    GPIO_PIN_11, // ROW3
    GPIO_PIN_12, // ROW4
    GPIO_PIN_13, // ROW5
    GPIO_PIN_14, // ROW6
    GPIO_PIN_15  // ROW7
};

/* Column enable pins ENM0 -> ENM7 (ULN2803 inputs on GPIOA) */
static uint16_t matrix_col_pins[8] = {
    GPIO_PIN_2,  // ENM0 -> COL0
    GPIO_PIN_3,  // ENM1 -> COL1
    GPIO_PIN_10, // ENM2 -> COL2
    GPIO_PIN_11, // ENM3 -> COL3
    GPIO_PIN_12, // ENM4 -> COL4
    GPIO_PIN_13, // ENM5 -> COL5
    GPIO_PIN_14, // ENM6 -> COL6
    GPIO_PIN_15  // ENM7 -> COL7
};

/* 7-Segment segment pins PB0 -> PB7 (Cathodes for Common Anode) */
static uint16_t seg_pins[8] = {
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7};

/* 7-Segment digit enable pins EN0 -> EN3 on GPIOA (Common Anode: Active HIGH) */
static uint16_t digit_en_pins[4] = {
    GPIO_PIN_6, // EN0
    GPIO_PIN_7, // EN1
    GPIO_PIN_8, // EN2
    GPIO_PIN_9  // EN3
};

/* Digits to display on the 4 7-segment displays */
static uint8_t led_buffer[4] = {1, 2, 3, 0};

/* 8x8 LED Matrix Heart Bitmap (Column-scanned: bit r = Row r, active HIGH) */
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

/* Multiplexing and timing tracking variables */
static volatile uint8_t current_matrix_col = 0;
static volatile uint8_t current_seg7_digit = 0;
static volatile uint16_t seg7_timer_counter = 0;
static volatile uint16_t d1_blink_counter = 0;
static volatile uint8_t counter = 0;
static volatile uint8_t dot_counter = 2;
static volatile uint8_t timer_flag[SOFT_TIMER_NUM_MAX] = {0};     // Flags for software timers
static volatile uint16_t timer_counter[SOFT_TIMER_NUM_MAX] = {0}; // Counters for software timers
static seg7_handle_t seg7_handler = {0};                          // Current segment states for 7-segment display
int hour = 15, minute = 8, second = 50;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void setTimer(uint8_t timer_index, uint16_t duration_ms);
void timerRun(void);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void update7SEG(int index);
void updateClockBuffer();
void updateMatrixRows(uint8_t col_index);
void updateLedMatrix(uint8_t index);
void clearMatrix();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void setTimer(uint8_t timer_index, uint16_t duration_ms)
{
  timer_counter[timer_index] = duration_ms / TICK_INTERVAL_MS; // convert duration to ticks
  timer_flag[timer_index] = 0;                                 // clear the timer if the index is out of range
}

void timerRun(void)
{
  for (uint8_t i = 0; i < SOFT_TIMER_NUM_MAX; i++)
  {
    if (timer_counter[i] > 0)
    {
      timer_counter[i]--;
      if (timer_counter[i] == 0)
      {
        timer_flag[i] = 1; // set the flag when the timer expires
      }
    }
  }
}

void update7SEG(int index)
{
  for (uint8_t i = 0; i < LED_NUM; i++)
  {
    HAL_GPIO_WritePin(GPIOA, digit_en_pins[i], GPIO_PIN_SET);
  }

  HAL_GPIO_WritePin(GPIOA, digit_en_pins[index], GPIO_PIN_RESET);

  if (index < 0 || index >= LED_NUM)
    return;

  SEG7Set_Digit(&seg7_handler, led_buffer[index]);
}

void updateClockBuffer()
{
  led_buffer[0] = hour / 10;
  led_buffer[1] = hour - 10 * led_buffer[0];
  led_buffer[2] = minute / 10;
  led_buffer[3] = minute - 10 * led_buffer[2];
}

// Use for column scanning

void updateMatrixRows(uint8_t col_index)
{
  if (col_index >= MAX_LED_MATRIX)
    return;

  // Turn off all columns first
  clearMatrix();

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

void clearMatrix()
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

void updateLedMatrix(uint8_t index)
{
  switch (index)
  {
  case 0:
    updateMatrixRows(0);
    break;

  case 1:
    updateMatrixRows(1);
    break;

  case 2:
    updateMatrixRows(2);
    break;

  case 3:
    updateMatrixRows(3);
    break;

  case 4:
    updateMatrixRows(4);
    break;

  case 5:
    updateMatrixRows(5);
    break;

  case 6:
    updateMatrixRows(6);
    break;

  case 7:
    updateMatrixRows(7);
    break;

  default:
    break;
  }
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  /* Start TIM2 in Interrupt mode (1 ms tick) */
  if (HAL_TIM_Base_Start_IT(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  SEG7Init(&seg7_handler, SEG7_COMMON_ANODE, SEG_PORT, seg_pins);
  for (uint8_t i = 0; i < SOFT_TIMER_NUM_MAX; i++)
  {
    setTimer(i, 0); // Reset all software timers to 0
  }
  clearMatrix();

  setTimer(0, 1000); // Set timer 0 for 1 second
  setTimer(1, 250);
  setTimer(2, 500); // Set timer 1 for 500 ms
  setTimer(3, 50);  // Set timer 2 for 1 second
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    if (timer_flag[0])
    {
      timer_flag[0] = 0; // Clear the flag
      setTimer(0, 1000); // Reset timer 0 for another second
      second++;
      if (second >= 60)
      {
        second = 0;
        minute++;
        if (minute >= 60)
        {
          minute = 0;
          hour++;
          if (hour >= 24)
          {
            hour = 0;
          }
        }
      }
      updateClockBuffer();
      // shift right the bitmap
      uint8_t tmp = matrix_bitmap[7];
      for (uint8_t i = 7; i > 0; i--)
      {
        matrix_bitmap[i] = matrix_bitmap[i - 1];
      }
      matrix_bitmap[0] = tmp;
    }
    if (timer_flag[1])
    {
      timer_flag[1] = 0; // Clear the flag
      setTimer(1, 250);  // Reset timer 1 for another 250 ms
      update7SEG(current_seg7_digit);
      current_seg7_digit = (current_seg7_digit + 1) % LED_NUM;
    }
    if (timer_flag[2])
    {
      timer_flag[2] = 0;                  // Clear the flag
      setTimer(2, 500);                   // Reset timer 2 for another 500 ms
      HAL_GPIO_TogglePin(GPIOA, DOT_PIN); // Toggle the dot
    }
    if (timer_flag[3])
    {
      timer_flag[3] = 0; // Clear the flag
      setTimer(3, 50);   // Reset timer 3 for another 100 ms
      updateLedMatrix(current_matrix_col);
      current_matrix_col = (current_matrix_col + 1) % MAX_LED_MATRIX;
    }
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 35999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */
  /* Enable AFIO clock and disable JTAG/SWD to release PA13, PA14, PA15, PB3, PB4 */
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_AFIO_REMAP_SWJ_DISABLE();
  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13 | GPIO_PIN_14, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC13 PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA2 PA3 PA4 PA5
                           PA6 PA7 PA8 PA9
                           PA10 PA11 PA12 PA13
                           PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB0 PB1 PB2 PB10
                           PB11 PB12 PB13 PB14
                           PB15 PB3 PB4 PB5
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */
  /* Initialize PA2 & PA3 (ENM0, ENM1) which are missing from default configuration */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Set default safe pin states:
   * - Columns ENM0..ENM7: LOW (ULN2803 is OFF)
   * - Rows ROW0..ROW7: LOW (Matrix LEDs OFF)
   * - Digit enables EN0..EN3: LOW (7-seg OFF)
   * - Segments SEG0..SEG6: HIGH (Common Anode segments OFF)
   * - Single LED D1: HIGH (Active LOW -> D1 is OFF)
   * - Dot: HIGH (OFF)
   */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_PIN_RESET);
  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  timerRun();
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
