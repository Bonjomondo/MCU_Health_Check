#ifndef __KEY_H
#define __KEY_H

// 引脚配置宏 - 根据原理图更新
#define KEY1_PORT GPIOC
#define KEY1_PIN GPIO_Pin_13
#define KEY2_PORT GPIOC
#define KEY2_PIN GPIO_Pin_14
#define KEY3_PORT GPIOC
#define KEY3_PIN GPIO_Pin_15
#define KEY4_PORT GPIOA
#define KEY4_PIN GPIO_Pin_0

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif
