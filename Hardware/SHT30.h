#ifndef __SHT30_H
#define __SHT30_H

#include "stm32f10x.h"

// SHT30 I2C地址
#define SHT30_ADDR 0x88  // 0x44 << 1

// SHT30命令
#define SHT30_CMD_READ_STATUS   0xF32D
#define SHT30_CMD_CLEAR_STATUS  0x3041
#define SHT30_CMD_SOFT_RESET    0x30A2
#define SHT30_CMD_HEATER_ENABLE 0x306D
#define SHT30_CMD_HEATER_DISABLE 0x3066
#define SHT30_CMD_MEAS_HIGH     0x2400  // 高重复性测量

// 数据结构
typedef struct {
    float Temperature;
    float Humidity;
} SHT30_Data;

void SHT30_Init(void);
void SHT30_WriteCmd(uint16_t cmd);
void SHT30_Reset(void);
void SHT30_ReadData(SHT30_Data *data);

#endif
