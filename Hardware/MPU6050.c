#include "stm32f10x.h"
#include "MPU6050.h"
#include "MyI2C.h"
#include "Delay.h"

/**
  * 函    数：MPU6050写寄存器
  * 参    数：reg 寄存器地址
  * 参    数：data 写入的数据
  * 返 回 值：无
  */
void MPU6050_WriteReg(uint8_t reg, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

/**
  * 函    数：MPU6050读寄存器
  * 参    数：reg 寄存器地址
  * 返 回 值：读取的数据
  */
uint8_t MPU6050_ReadReg(uint8_t reg)
{
    uint8_t data;
    
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDR | 0x01);
    MyI2C_ReceiveAck();
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();
    
    return data;
}

/**
  * 函    数：MPU6050初始化
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_Init(void)
{
    MyI2C_Init();
    
    // 电源管理，解除休眠
    MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);
    // 电源管理2
    MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);
    // 采样率分频
    MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);
    // 配置寄存器
    MPU6050_WriteReg(MPU6050_CONFIG, 0x06);
    // 陀螺仪配置，±2000°/s
    MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    // 加速度计配置，±8g
    MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);
}

/**
  * 函    数：MPU6050获取数据
  * 参    数：data 数据结构指针
  * 返 回 值：无
  */
void MPU6050_GetData(MPU6050_Data *data)
{
    uint8_t DataH, DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    data->Accel_X = (DataH << 8) | DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    data->Accel_Y = (DataH << 8) | DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    data->Accel_Z = (DataH << 8) | DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    data->Gyro_X = (DataH << 8) | DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    data->Gyro_Y = (DataH << 8) | DataL;
    
    DataH = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    DataL = MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    data->Gyro_Z = (DataH << 8) | DataL;
}

/**
  * 函    数：MPU6050获取ID
  * 参    数：无
  * 返 回 值：ID值（应为0x68）
  */
uint8_t MPU6050_GetID(void)
{
    return MPU6050_ReadReg(MPU6050_WHO_AM_I);
}
