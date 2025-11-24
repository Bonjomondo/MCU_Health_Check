#include "stm32f10x.h"
#include "Motor.h"
#include "Delay.h"

/**
  * 函    数：震动马达初始化
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = MOTOR_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(MOTOR_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(MOTOR_PORT, MOTOR_PIN);
}

/**
  * 函    数：震动马达打开
  * 参    数：无
  * 返 回 值：无
  */
void Motor_ON(void)
{
    GPIO_SetBits(MOTOR_PORT, MOTOR_PIN);
}

/**
  * 函    数：震动马达关闭
  * 参    数：无
  * 返 回 值：无
  */
void Motor_OFF(void)
{
    GPIO_ResetBits(MOTOR_PORT, MOTOR_PIN);
}

/**
  * 函    数：震动马达翻转
  * 参    数：无
  * 返 回 值：无
  */
void Motor_Turn(void)
{
    if (GPIO_ReadOutputDataBit(MOTOR_PORT, MOTOR_PIN) == 0)
    {
        GPIO_SetBits(MOTOR_PORT, MOTOR_PIN);
    }
    else
    {
        GPIO_ResetBits(MOTOR_PORT, MOTOR_PIN);
    }
}

/**
  * 函    数：震动马达震动指定次数
  * 参    数：times 震动次数
  * 参    数：duration 每次震动持续时间(ms)
  * 返 回 值：无
  */
void Motor_Vibrate(uint16_t times, uint16_t duration)
{
    uint16_t i;
    for(i = 0; i < times; i++)
    {
        Motor_ON();
        Delay_ms(duration);
        Motor_OFF();
        Delay_ms(duration);
    }
}
