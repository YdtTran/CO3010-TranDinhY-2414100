/* scheduler.h - API bộ lập lịch hợp tác (dùng chung cho O(N) và O(1)) */
#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <stdint.h>
#include "stm32f1xx.h"

#define SCH_MAX_TASKS 40
#define NO_TASK_ID 0u
#define TICK_MS 10u /* chu kỳ ngắt timer (ms) */

void SCH_Init(void);
void SCH_Update(void);         /* gọi trong ISR của timer */
void SCH_Dispatch_Tasks(void); /* gọi trong while(1) của main() */
/* DELAY, PERIOD tính bằng tick; PERIOD = 0 -> tác vụ chạy một lần */
uint32_t SCH_Add_Task(void (*pFunction)(void), uint32_t DELAY, uint32_t PERIOD);
uint8_t SCH_Delete_Task(uint32_t taskID); /* 1 = đã xóa, 0 = không tìm thấy */

#endif
