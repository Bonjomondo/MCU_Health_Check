#ifndef __SCHEDULER_H
#define __SCHEDULER_H

#include "stm32f10x.h"

// 任务最大数量
#define MAX_TASKS 10

// 任务函数类型
typedef void (*TaskFunc)(void);

// 任务结构体
typedef struct {
    TaskFunc func;      // 任务函数指针
    uint32_t period;    // 任务周期(ms)
    uint32_t lastRun;   // 上次运行时间(ms)
    uint8_t enable;     // 任务使能标志
} Task;

void Scheduler_Init(void);
void Scheduler_AddTask(TaskFunc func, uint32_t period);
void Scheduler_Run(void);
uint32_t Scheduler_GetTick(void);

#endif
