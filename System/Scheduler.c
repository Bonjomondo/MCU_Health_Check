#include "stm32f10x.h"
#include "Scheduler.h"

static Task tasks[MAX_TASKS];
static uint8_t taskCount = 0;
static volatile uint32_t systemTick = 0;

/**
  * 函    数：SysTick中断服务函数
  * 参    数：无
  * 返 回 值：无
  */
void SysTick_Handler(void)
{
    systemTick++;
}

/**
  * 函    数：获取系统时钟
  * 参    数：无
  * 返 回 值：系统时钟(ms)
  */
uint32_t Scheduler_GetTick(void)
{
    return systemTick;
}

/**
  * 函    数：调度器初始化
  * 参    数：无
  * 返 回 值：无
  */
void Scheduler_Init(void)
{
    uint8_t i;
    
    // 清空任务列表
    for(i = 0; i < MAX_TASKS; i++)
    {
        tasks[i].func = 0;
        tasks[i].period = 0;
        tasks[i].lastRun = 0;
        tasks[i].enable = 0;
    }
    
    taskCount = 0;
    
    // 配置SysTick为1ms中断
    SysTick_Config(SystemCoreClock / 1000);
}

/**
  * 函    数：添加任务
  * 参    数：func 任务函数指针
  * 参    数：period 任务周期(ms)
  * 返 回 值：无
  */
void Scheduler_AddTask(TaskFunc func, uint32_t period)
{
    if(taskCount < MAX_TASKS)
    {
        tasks[taskCount].func = func;
        tasks[taskCount].period = period;
        tasks[taskCount].lastRun = systemTick;
        tasks[taskCount].enable = 1;
        taskCount++;
    }
}

/**
  * 函    数：运行调度器
  * 参    数：无
  * 返 回 值：无
  */
void Scheduler_Run(void)
{
    uint8_t i;
    uint32_t currentTick;
    
    currentTick = systemTick;
    
    for(i = 0; i < taskCount; i++)
    {
        if(tasks[i].enable && tasks[i].func)
        {
            if((currentTick - tasks[i].lastRun) >= tasks[i].period)
            {
                tasks[i].func();
                tasks[i].lastRun = currentTick;
            }
        }
    }
}
