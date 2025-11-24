#ifndef __MAX30102_H
#define __MAX30102_H

#include "stm32f10x.h"

// MAX30102 I2C地址
#define MAX30102_ADDR 0xAE  // 0x57 << 1

// MAX30102寄存器地址
#define MAX30102_INT_STATUS_1   0x00
#define MAX30102_INT_STATUS_2   0x01
#define MAX30102_INT_ENABLE_1   0x02
#define MAX30102_INT_ENABLE_2   0x03
#define MAX30102_FIFO_WR_PTR    0x04
#define MAX30102_FIFO_OVF_CTR   0x05
#define MAX30102_FIFO_RD_PTR    0x06
#define MAX30102_FIFO_DATA      0x07
#define MAX30102_FIFO_CONFIG    0x08
#define MAX30102_MODE_CONFIG    0x09
#define MAX30102_SPO2_CONFIG    0x0A
#define MAX30102_LED1_PA        0x0C
#define MAX30102_LED2_PA        0x0D
#define MAX30102_PILOT_PA       0x10
#define MAX30102_TEMP_INT       0x1F
#define MAX30102_TEMP_FRAC      0x20
#define MAX30102_TEMP_CONFIG    0x21
#define MAX30102_REV_ID         0xFE
#define MAX30102_PART_ID        0xFF

// 数据结构
typedef struct {
    uint32_t IR;
    uint32_t RED;
} MAX30102_Data;

void MAX30102_Init(void);
uint8_t MAX30102_ReadReg(uint8_t reg);
void MAX30102_WriteReg(uint8_t reg, uint8_t data);
void MAX30102_Reset(void);
void MAX30102_ReadFIFO(MAX30102_Data *data);
float MAX30102_ReadTemperature(void);
uint8_t MAX30102_GetPartID(void);

#endif
