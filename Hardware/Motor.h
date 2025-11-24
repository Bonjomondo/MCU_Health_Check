#ifndef __MOTOR_H
#define __MOTOR_H

#include "stm32f10x.h"

// 震动马达引脚配置
#define MOTOR_PORT GPIOA
#define MOTOR_PIN GPIO_Pin_2

void Motor_Init(void);
void Motor_ON(void);
void Motor_OFF(void);
void Motor_Turn(void);
void Motor_Vibrate(uint16_t times, uint16_t duration);

#endif
