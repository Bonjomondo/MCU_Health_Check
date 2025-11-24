#include "stm32f10x.h"
#include "Buzzer.h"
#include "Delay.h"

/**
  * 函    数：蜂鸣器初始化
  * 参    数：无
  * 返 回 值：无
  */
void Buzzer_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(BUZZER_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
  * 函    数：蜂鸣器打开
  * 参    数：无
  * 返 回 值：无
  */
void Buzzer_ON(void)
{
    GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
  * 函    数：蜂鸣器关闭
  * 参    数：无
  * 返 回 值：无
  */
void Buzzer_OFF(void)
{
    GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
}

/**
  * 函    数：蜂鸣器翻转
  * 参    数：无
  * 返 回 值：无
  */
void Buzzer_Turn(void)
{
    if (GPIO_ReadOutputDataBit(BUZZER_PORT, BUZZER_PIN) == 0)
    {
        GPIO_SetBits(BUZZER_PORT, BUZZER_PIN);
    }
    else
    {
        GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN);
    }
}

/**
  * 函    数：蜂鸣器响指定次数
  * 参    数：times 响的次数
  * 参    数：duration 每次响的持续时间(ms)
  * 返 回 值：无
  */
void Buzzer_Beep(uint16_t times, uint16_t duration)
{
    uint16_t i;
    for(i = 0; i < times; i++)
    {
        Buzzer_ON();
        Delay_ms(duration);
        Buzzer_OFF();
        Delay_ms(duration);
    }
}
