#include "scheduler.h"
#include <string.h>

typedef struct
{
    void (*pTask)(void);
    uint32_t Delay;  /* số tick SAU tác vụ đứng trước        */
    uint32_t Period; /* số tick giữa hai lần chạy, 0 = một lần */
    uint8_t RunMe;   /* chỉ có nghĩa với phần tử đầu         */
    uint32_t TaskID;
} sTask;

static sTask SCH_tasks_G[SCH_MAX_TASKS]; /* [0] = tác vụ đến hạn kế tiếp */
static uint8_t SCH_count = 0;
static uint32_t newTaskID = 0;

uint32_t SCH_Lock(void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();
    return primask;
}

void SCH_Unlock(uint32_t previous_state)
{
    __set_PRIMASK(previous_state);
}

static uint32_t Get_New_Task_ID(void)
{
    if (++newTaskID == NO_TASK_ID)
        newTaskID++;
    return newTaskID;
}

void SCH_Init(void)
{
    memset(SCH_tasks_G, 0, sizeof(SCH_tasks_G));
    SCH_count = 0;
    newTaskID = 0;
}

uint32_t SCH_Add_Task(void (*pF)(void), uint32_t DELAY,
                      uint32_t PERIOD)
{
    uint8_t i = 0;
    uint32_t m = SCH_Lock();
    while (i < SCH_MAX_TASKS && SCH_tasks_G[i].pTask)
        i++;
    if (i == SCH_MAX_TASKS) // mảng đầy
    {
        SCH_Unlock(m);
        return NO_TASK_ID;
    }
    sTask *t = &SCH_tasks_G[i];
    t->pTask = pF;
    t->Delay = DELAY;
    t->Period = PERIOD;
    t->RunMe = 0;
    t->TaskID = Get_New_Task_ID();
    if (DELAY == 0)
    {
        t->RunMe = 1;
        t->Delay = PERIOD;
    }
    SCH_Unlock(m);
    return t->TaskID;
}

void SCH_Update(void)
{
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++)
    { // O(N)
        sTask *t = &SCH_tasks_G[i];
        if (t->pTask == 0)
            continue;
        if (t->Delay > 0)
            t->Delay--; // giảm trước ...
        if (t->Delay == 0)
        { // ... rồi mới kiểm tra
            t->RunMe++;
            t->Delay = t->Period; // nạp lại chu kỳ
        }
    }
}

void SCH_Dispatch_Tasks(void)
{
    for (uint8_t i = 0; i < SCH_MAX_TASKS; i++)
    {
        if (SCH_tasks_G[i].RunMe == 0)
            continue;
        uint32_t m = SCH_Lock();
        SCH_tasks_G[i].RunMe--; // ISR cũng ghi RunMe
        void (*f)(void) = SCH_tasks_G[i].pTask;
        uint32_t id = SCH_tasks_G[i].TaskID;
        uint32_t period = SCH_tasks_G[i].Period;
        SCH_Unlock(m);
        f(); // ngoài vùng khóa
        if (period == 0)
            SCH_Delete_Task(id); // một lần
    }
}

/* O(N): tìm theo ID; tác vụ phía sau nhận lại Delay của tác vụ bị xóa */
uint8_t SCH_Delete_Task(uint32_t taskID)
{
    if (taskID == NO_TASK_ID)
        return 0;
    uint8_t found = 0;
    uint32_t m = SCH_Lock();
    for (uint8_t i = 0; i < SCH_count; i++)
    {
        if (SCH_tasks_G[i].TaskID != taskID)
            continue;
        if (i + 1 < SCH_count)
        {
            SCH_tasks_G[i + 1].Delay += SCH_tasks_G[i].Delay; /* trao lại delay */
            SCH_tasks_G[i + 1].RunMe = (i == 0 && SCH_tasks_G[i + 1].Delay == 0);
        }
        for (uint8_t j = i; j + 1 < SCH_count; j++)
            SCH_tasks_G[j] = SCH_tasks_G[j + 1];
        SCH_count--;
        memset(&SCH_tasks_G[SCH_count], 0, sizeof(sTask));
        found = 1;
        break;
    }
    SCH_Unlock(m);
    return found;
}
