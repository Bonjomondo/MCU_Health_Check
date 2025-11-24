#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Key.h"
#include "Buzzer.h"
#include "Motor.h"
#include "MAX30102.h"
#include "DS18B20.h"
#include "MPU6050.h"
#include "SHT30.h"
#include "W25Q64.h"
#include "Serial.h"
#include "Scheduler.h"
#include <stdio.h>

// 全局变量
uint8_t currentPage = 0;  // 当前显示页面
uint8_t measurementActive = 0;  // 测量激活标志
uint32_t lastActivityTime = 0;  // 上次活动时间

// 传感器数据
MAX30102_Data max30102Data;
float bodyTemp = 0;
MPU6050_Data mpu6050Data;
SHT30_Data sht30Data;
uint8_t heartRate = 0;
uint8_t spo2 = 0;

// 健康数据阈值
#define TEMP_LOW_THRESHOLD  35.0
#define TEMP_HIGH_THRESHOLD 38.0
#define HR_LOW_THRESHOLD    50
#define HR_HIGH_THRESHOLD   120
#define SPO2_LOW_THRESHOLD  90
#define IDLE_ALERT_TIME     1800000  // 30分钟(ms)

// 心率检测参数
#define MIN_HEARTBEAT_INTERVAL_MS  300   // 最小心跳间隔(ms) - 对应200 bpm
#define MAX_HEARTBEAT_INTERVAL_MS  3000  // 最大心跳间隔(ms) - 对应20 bpm

/**
  * 函    数：心率血氧测量任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_HeartRateSpO2(void)
{
    if(measurementActive)
    {
        MAX30102_ReadFIFO(&max30102Data);
        
        // 简化的心率计算（实际应用需要更复杂的算法）
        if(max30102Data.IR > 50000)
        {
            static uint32_t lastBeat = 0;
            uint32_t currentTime = Scheduler_GetTick();
            
            if(currentTime - lastBeat > MIN_HEARTBEAT_INTERVAL_MS && 
               currentTime - lastBeat < MAX_HEARTBEAT_INTERVAL_MS)  // 防止误触发和除零错误
            {
                uint32_t interval = currentTime - lastBeat;
                uint32_t hr = 60000 / interval;
                if(hr <= 255) {
                    heartRate = (uint8_t)hr;
                }
                lastBeat = currentTime;
                
                // 简化的SpO2计算
                if(max30102Data.RED > 1000 && max30102Data.IR > 1000)
                {
                    float ratio = (float)max30102Data.RED / max30102Data.IR;
                    spo2 = (uint8_t)(110 - 25 * ratio);
                    if(spo2 > 100) spo2 = 100;
                    if(spo2 < 70) spo2 = 70;
                }
            }
        }
    }
}

/**
  * 函    数：温度测量任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_Temperature(void)
{
    if(measurementActive)
    {
        bodyTemp = DS18B20_ReadTemperature();
        
        // 温度异常检测
        if(bodyTemp < TEMP_LOW_THRESHOLD || bodyTemp > TEMP_HIGH_THRESHOLD)
        {
            Buzzer_Beep(2, 100);  // 蜂鸣器报警
        }
    }
}

/**
  * 函    数：运动检测任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_MotionDetect(void)
{
    static int16_t lastAccel_X = 0;
    static int16_t lastAccel_Y = 0;
    static int16_t lastAccel_Z = 0;
    
    MPU6050_GetData(&mpu6050Data);
    
    // 检测运动变化
    int16_t deltaX = mpu6050Data.Accel_X - lastAccel_X;
    int16_t deltaY = mpu6050Data.Accel_Y - lastAccel_Y;
    int16_t deltaZ = mpu6050Data.Accel_Z - lastAccel_Z;
    
    if(deltaX > 1000 || deltaY > 1000 || deltaZ > 1000 ||
       deltaX < -1000 || deltaY < -1000 || deltaZ < -1000)
    {
        lastActivityTime = Scheduler_GetTick();
    }
    
    lastAccel_X = mpu6050Data.Accel_X;
    lastAccel_Y = mpu6050Data.Accel_Y;
    lastAccel_Z = mpu6050Data.Accel_Z;
}

/**
  * 函    数：环境监测任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_Environment(void)
{
    SHT30_ReadData(&sht30Data);
}

/**
  * 函    数：久坐提醒任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_IdleAlert(void)
{
    uint32_t currentTime = Scheduler_GetTick();
    
    if(currentTime - lastActivityTime > IDLE_ALERT_TIME)
    {
        Motor_Vibrate(3, 200);  // 震动提醒
        Buzzer_Beep(1, 100);
        lastActivityTime = currentTime;
    }
}

/**
  * 函    数：数据传输任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_DataTransmit(void)
{
    char buffer[128];
    
    // 通过串口发送数据（HC-05蓝牙）- 使用snprintf防止缓冲区溢出
    snprintf(buffer, sizeof(buffer), "HR:%d,SpO2:%d,Temp:%.1f,EnvT:%.1f,Hum:%.1f\r\n",
            heartRate, spo2, bodyTemp, sht30Data.Temperature, sht30Data.Humidity);
    Serial_SendString(1, buffer);
}

/**
  * 函    数：显示更新任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_DisplayUpdate(void)
{
    char str[32];
    
    OLED_Clear();
    
    switch(currentPage)
    {
        case 0:  // 主页 - 心率和血氧
            OLED_ShowString(1, 1, "Health Monitor");
            snprintf(str, sizeof(str), "HR:%3dbpm", heartRate);
            OLED_ShowString(2, 1, str);
            snprintf(str, sizeof(str), "SpO2:%3d%%", spo2);
            OLED_ShowString(3, 1, str);
            snprintf(str, sizeof(str), "Temp:%.1fC", bodyTemp);
            OLED_ShowString(4, 1, str);
            break;
            
        case 1:  // 环境监测
            OLED_ShowString(1, 1, "Environment");
            snprintf(str, sizeof(str), "Temp:%.1fC", sht30Data.Temperature);
            OLED_ShowString(2, 1, str);
            snprintf(str, sizeof(str), "Humi:%.1f%%", sht30Data.Humidity);
            OLED_ShowString(3, 1, str);
            break;
            
        case 2:  // 运动状态
            OLED_ShowString(1, 1, "Motion Status");
            snprintf(str, sizeof(str), "AX:%6d", mpu6050Data.Accel_X);
            OLED_ShowString(2, 1, str);
            snprintf(str, sizeof(str), "AY:%6d", mpu6050Data.Accel_Y);
            OLED_ShowString(3, 1, str);
            snprintf(str, sizeof(str), "AZ:%6d", mpu6050Data.Accel_Z);
            OLED_ShowString(4, 1, str);
            break;
            
        case 3:  // 系统信息
            OLED_ShowString(1, 1, "System Info");
            if(measurementActive)
                OLED_ShowString(2, 1, "Status:Running");
            else
                OLED_ShowString(2, 1, "Status:Stopped");
            snprintf(str, sizeof(str), "Time:%us", (unsigned int)(Scheduler_GetTick()/1000));
            OLED_ShowString(3, 1, str);
            break;
    }
}

/**
  * 函    数：按键处理任务
  * 参    数：无
  * 返 回 值：无
  */
void Task_KeyScan(void)
{
    uint8_t keyNum = Key_GetNum();
    
    if(keyNum == 1)  // KEY1 - 切换页面
    {
        currentPage++;
        if(currentPage > 3) currentPage = 0;
        Motor_Vibrate(1, 50);  // 触觉反馈
    }
    else if(keyNum == 2)  // KEY2 - 开始/停止测量
    {
        measurementActive = !measurementActive;
        if(measurementActive)
        {
            Buzzer_Beep(1, 100);
        }
        else
        {
            Buzzer_Beep(2, 50);
        }
    }
    else if(keyNum == 3)  // KEY3 - 保存数据到Flash
    {
        uint8_t dataToSave[16];
        dataToSave[0] = heartRate;
        dataToSave[1] = spo2;
        dataToSave[2] = (uint8_t)bodyTemp;
        dataToSave[3] = (uint8_t)((bodyTemp - dataToSave[2]) * 10);  // 正确提取小数部分
        
        W25Q64_SectorErase(0x000000);
        W25Q64_PageWrite(0x000000, dataToSave, 16);
        Motor_Vibrate(2, 100);
    }
    else if(keyNum == 4)  // KEY4 - 手动触发测量完成提醒
    {
        Motor_Vibrate(3, 200);
        Buzzer_Beep(3, 100);
    }
}

int main(void)
{
    // 系统初始化
    Delay_ms(100);
    
    // 外设初始化
    OLED_Init();
    Key_Init();
    Buzzer_Init();
    Motor_Init();
    Serial_Init(1, 9600);  // 初始化串口1用于HC-05蓝牙
    
    // 传感器初始化
    MAX30102_Init();
    DS18B20_Init();
    MPU6050_Init();
    SHT30_Init();
    W25Q64_Init();
    
    // 调度器初始化
    Scheduler_Init();
    
    // 添加任务
    Scheduler_AddTask(Task_HeartRateSpO2, 50);    // 50ms - 心率血氧采集
    Scheduler_AddTask(Task_Temperature, 1000);     // 1s - 温度测量
    Scheduler_AddTask(Task_MotionDetect, 100);     // 100ms - 运动检测
    Scheduler_AddTask(Task_Environment, 2000);     // 2s - 环境监测
    Scheduler_AddTask(Task_IdleAlert, 10000);      // 10s - 久坐检测
    Scheduler_AddTask(Task_DataTransmit, 1000);    // 1s - 数据传输
    Scheduler_AddTask(Task_DisplayUpdate, 200);    // 200ms - 显示更新
    Scheduler_AddTask(Task_KeyScan, 10);           // 10ms - 按键扫描
    
    // 欢迎界面
    OLED_ShowString(1, 1, "MCU Health");
    OLED_ShowString(2, 1, "Check System");
    OLED_ShowString(3, 1, "Initializing...");
    Delay_ms(2000);
    
    // 启动测量
    measurementActive = 1;
    lastActivityTime = Scheduler_GetTick();
    
    // 主循环
    while (1)
    {
        Scheduler_Run();
    }
}

