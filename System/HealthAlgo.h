#ifndef __HEALTHALGO_H
#define __HEALTHALGO_H

#include "stm32f10x.h"

// 缓冲区大小
#define BUFFER_SIZE 100

// 算法结构体
typedef struct {
    uint32_t irBuffer[BUFFER_SIZE];
    uint32_t redBuffer[BUFFER_SIZE];
    uint8_t bufferIndex;
    uint8_t bufferFull;
} HealthAlgo;

void HealthAlgo_Init(HealthAlgo *algo);
void HealthAlgo_AddSample(HealthAlgo *algo, uint32_t irValue, uint32_t redValue);
uint8_t HealthAlgo_CalculateHeartRate(HealthAlgo *algo);
uint8_t HealthAlgo_CalculateSpO2(HealthAlgo *algo);
uint8_t HealthAlgo_CheckMotion(int16_t ax, int16_t ay, int16_t az);

#endif
