#ifndef __DS18B20_H
#define __DS18B20_H

#include "stm32f10x.h"

// DS18B20引脚配置
#define DS18B20_PORT GPIOA
#define DS18B20_PIN GPIO_Pin_1

// DS18B20命令
#define DS18B20_SKIP_ROM        0xCC
#define DS18B20_CONVERT_T       0x44
#define DS18B20_READ_SCRATCHPAD 0xBE

// DS18B20时序定义
#define DS18B20_CONVERSION_TIME_MS  750  // 温度转换时间

void DS18B20_Init(void);
uint8_t DS18B20_Reset(void);
void DS18B20_WriteByte(uint8_t Byte);
uint8_t DS18B20_ReadByte(void);
float DS18B20_ReadTemperature(void);

#endif
