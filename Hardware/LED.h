#ifndef __LED_H
#define __LED_H

// 引脚配置宏
#define LED_PORT GPIOA
#define LED_PIN1 GPIO_Pin_1
#define LED_PIN2 GPIO_Pin_2

void LED_Init(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED1_Turn(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED2_Turn(void);

#endif
