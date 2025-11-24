#include "stm32f10x.h"
#include "HealthAlgo.h"

/**
  * 函    数：算法初始化
  * 参    数：algo 算法结构体指针
  * 返 回 值：无
  */
void HealthAlgo_Init(HealthAlgo *algo)
{
    uint8_t i;
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        algo->irBuffer[i] = 0;
        algo->redBuffer[i] = 0;
    }
    algo->bufferIndex = 0;
    algo->bufferFull = 0;
}

/**
  * 函    数：添加采样数据
  * 参    数：algo 算法结构体指针
  * 参    数：irValue IR LED值
  * 参    数：redValue RED LED值
  * 返 回 值：无
  */
void HealthAlgo_AddSample(HealthAlgo *algo, uint32_t irValue, uint32_t redValue)
{
    algo->irBuffer[algo->bufferIndex] = irValue;
    algo->redBuffer[algo->bufferIndex] = redValue;
    
    algo->bufferIndex++;
    if(algo->bufferIndex >= BUFFER_SIZE)
    {
        algo->bufferIndex = 0;
        algo->bufferFull = 1;
    }
}

/**
  * 函    数：计算心率（简化版峰值检测）
  * 参    数：algo 算法结构体指针
  * 返 回 值：心率值(bpm)
  */
uint8_t HealthAlgo_CalculateHeartRate(HealthAlgo *algo)
{
    uint8_t i;
    uint32_t sum = 0;
    uint32_t avg;
    uint8_t peakCount = 0;
    uint8_t inPeak = 0;
    
    if(!algo->bufferFull && algo->bufferIndex < 50)
        return 0;
    
    // 计算平均值
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        sum += algo->irBuffer[i];
    }
    avg = sum / BUFFER_SIZE;
    
    // 峰值检测（简化）
    for(i = 1; i < BUFFER_SIZE - 1; i++)
    {
        if(algo->irBuffer[i] > avg + 5000)
        {
            if(!inPeak && algo->irBuffer[i] > algo->irBuffer[i-1] && 
               algo->irBuffer[i] > algo->irBuffer[i+1])
            {
                peakCount++;
                inPeak = 1;
            }
        }
        else
        {
            inPeak = 0;
        }
    }
    
    // 计算心率 (假设采样率为20Hz，BUFFER_SIZE=100对应5秒)
    if(peakCount > 0)
    {
        return (peakCount * 60) / 5;  // BPM
    }
    
    return 0;
}

/**
  * 函    数：计算血氧（R值法简化版）
  * 参    数：algo 算法结构体指针
  * 返 回 值：血氧值(%)
  */
uint8_t HealthAlgo_CalculateSpO2(HealthAlgo *algo)
{
    uint32_t irDC = 0, redDC = 0;
    uint32_t irAC = 0, redAC = 0;
    uint32_t irMax = 0, irMin = 0xFFFFFFFF;
    uint32_t redMax = 0, redMin = 0xFFFFFFFF;
    uint8_t i;
    float R;
    uint8_t spo2;
    
    if(!algo->bufferFull && algo->bufferIndex < 50)
        return 0;
    
    // 计算DC和AC分量
    for(i = 0; i < BUFFER_SIZE; i++)
    {
        irDC += algo->irBuffer[i];
        redDC += algo->redBuffer[i];
        
        if(algo->irBuffer[i] > irMax) irMax = algo->irBuffer[i];
        if(algo->irBuffer[i] < irMin) irMin = algo->irBuffer[i];
        if(algo->redBuffer[i] > redMax) redMax = algo->redBuffer[i];
        if(algo->redBuffer[i] < redMin) redMin = algo->redBuffer[i];
    }
    
    irDC /= BUFFER_SIZE;
    redDC /= BUFFER_SIZE;
    irAC = (irMax - irMin) / 2;
    redAC = (redMax - redMin) / 2;
    
    // 计算R值
    if(irDC == 0 || redAC == 0 || redDC == 0)
        return 0;
    
    R = ((float)redAC / (float)redDC) / ((float)irAC / (float)irDC);
    
    // SpO2公式（经验公式）
    spo2 = (uint8_t)(110 - 25 * R);
    
    // 限制范围
    if(spo2 > 100) spo2 = 100;
    if(spo2 < 70) spo2 = 70;
    
    return spo2;
}

/**
  * 函    数：检测运动状态
  * 参    数：ax, ay, az 加速度三轴数据
  * 返 回 值：1表示运动中，0表示静止
  */
uint8_t HealthAlgo_CheckMotion(int16_t ax, int16_t ay, int16_t az)
{
    static int16_t lastAx = 0, lastAy = 0, lastAz = 0;
    static uint8_t firstRun = 1;
    int16_t deltaX, deltaY, deltaZ;
    
    if(firstRun)
    {
        lastAx = ax;
        lastAy = ay;
        lastAz = az;
        firstRun = 0;
        return 0;
    }
    
    deltaX = ax - lastAx;
    deltaY = ay - lastAy;
    deltaZ = az - lastAz;
    
    lastAx = ax;
    lastAy = ay;
    lastAz = az;
    
    // 阈值检测
    if(deltaX > 1000 || deltaX < -1000 ||
       deltaY > 1000 || deltaY < -1000 ||
       deltaZ > 1000 || deltaZ < -1000)
    {
        return 1;  // 运动中
    }
    
    return 0;  // 静止
}
