#ifndef __KEY_H
#define __KEY_H

// 引脚配置宏
#define KEY_PORT GPIOB
#define KEY_PIN1 GPIO_Pin_1
#define KEY_PIN2 GPIO_Pin_11

void Key_Init(void);
uint8_t Key_GetNum(void);

#endif
