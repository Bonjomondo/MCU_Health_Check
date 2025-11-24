#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f10x.h"

// 蜂鸣器引脚配置
#define BUZZER_PORT GPIOA
#define BUZZER_PIN GPIO_Pin_3

void Buzzer_Init(void);
void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Turn(void);
void Buzzer_Beep(uint16_t times, uint16_t duration);

#endif
