#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "Key.h"

/**
  * 函    数：按键初始化
  * 参    数：无
  * 返 回 值：无
  */
void Key_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_InitStructure.GPIO_Pin = KEY1_PIN | KEY2_PIN | KEY3_PIN;
	GPIO_Init(KEY1_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = KEY4_PIN;
	GPIO_Init(KEY4_PORT, &GPIO_InitStructure);
}

/**
  * 函    数：按键获取键码
  * 参    数：无
  * 返 回 值：按下按键的键码值，范围：0~4，返回0代表没有按键按下
  * 注意事项：此函数是阻塞式操作，当按键按住不放时，函数会卡住，直到按键松手
  */
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum = 0;		//定义变量，默认键码值为0
	
	if (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY1_PORT, KEY1_PIN) == 0);
		Delay_ms(20);
		KeyNum = 1;
	}
	
	if (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY2_PORT, KEY2_PIN) == 0);
		Delay_ms(20);
		KeyNum = 2;
	}
	
	if (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY3_PORT, KEY3_PIN) == 0);
		Delay_ms(20);
		KeyNum = 3;
	}
	
	if (GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == 0)
	{
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY4_PORT, KEY4_PIN) == 0);
		Delay_ms(20);
		KeyNum = 4;
	}
	
	return KeyNum;
}
