#include "stm32f10x.h"
#include "MAX30102.h"
#include "MyI2C.h"
#include "Delay.h"

/**
  * 函    数：MAX30102写寄存器
  * 参    数：reg 寄存器地址
  * 参    数：data 写入的数据
  * 返 回 值：无
  */
void MAX30102_WriteReg(uint8_t reg, uint8_t data)
{
    MyI2C_Start();
    MyI2C_SendByte(MAX30102_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(data);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

/**
  * 函    数：MAX30102读寄存器
  * 参    数：reg 寄存器地址
  * 返 回 值：读取的数据
  */
uint8_t MAX30102_ReadReg(uint8_t reg)
{
    uint8_t data;
    
    MyI2C_Start();
    MyI2C_SendByte(MAX30102_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(reg);
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(MAX30102_ADDR | 0x01);
    MyI2C_ReceiveAck();
    data = MyI2C_ReceiveByte();
    MyI2C_SendAck(1);
    MyI2C_Stop();
    
    return data;
}

/**
  * 函    数：MAX30102复位
  * 参    数：无
  * 返 回 值：无
  */
void MAX30102_Reset(void)
{
    MAX30102_WriteReg(MAX30102_MODE_CONFIG, 0x40);
    Delay_ms(100);
}

/**
  * 函    数：MAX30102初始化
  * 参    数：无
  * 返 回 值：无
  */
void MAX30102_Init(void)
{
    MyI2C_Init();
    
    MAX30102_Reset();
    
    // FIFO配置：采样平均4次，FIFO几乎满时触发中断
    MAX30102_WriteReg(MAX30102_FIFO_CONFIG, 0x4F);
    
    // 模式配置：SpO2模式
    MAX30102_WriteReg(MAX30102_MODE_CONFIG, 0x03);
    
    // SpO2配置：ADC量程4096nA，采样率100Hz，LED脉宽411us，分辨率18位
    MAX30102_WriteReg(MAX30102_SPO2_CONFIG, 0x27);
    
    // LED电流配置：RED LED = 7.6mA, IR LED = 7.6mA
    MAX30102_WriteReg(MAX30102_LED1_PA, 0x24);
    MAX30102_WriteReg(MAX30102_LED2_PA, 0x24);
    
    // 清空FIFO
    MAX30102_WriteReg(MAX30102_FIFO_WR_PTR, 0x00);
    MAX30102_WriteReg(MAX30102_FIFO_OVF_CTR, 0x00);
    MAX30102_WriteReg(MAX30102_FIFO_RD_PTR, 0x00);
}

/**
  * 函    数：从FIFO读取数据
  * 参    数：data 数据结构指针
  * 返 回 值：无
  */
void MAX30102_ReadFIFO(MAX30102_Data *data)
{
    uint8_t temp[6];
    uint8_t i;
    
    MyI2C_Start();
    MyI2C_SendByte(MAX30102_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(MAX30102_FIFO_DATA);
    MyI2C_ReceiveAck();
    
    MyI2C_Start();
    MyI2C_SendByte(MAX30102_ADDR | 0x01);
    MyI2C_ReceiveAck();
    
    for(i = 0; i < 6; i++)
    {
        temp[i] = MyI2C_ReceiveByte();
        if(i < 5)
            MyI2C_SendAck(0);
        else
            MyI2C_SendAck(1);
    }
    MyI2C_Stop();
    
    // RED LED数据（3字节）
    data->RED = ((uint32_t)temp[0] << 16) | ((uint32_t)temp[1] << 8) | temp[2];
    data->RED &= 0x03FFFF;
    
    // IR LED数据（3字节）
    data->IR = ((uint32_t)temp[3] << 16) | ((uint32_t)temp[4] << 8) | temp[5];
    data->IR &= 0x03FFFF;
}

/**
  * 函    数：读取温度
  * 参    数：无
  * 返 回 值：温度值（摄氏度）
  */
float MAX30102_ReadTemperature(void)
{
    int8_t tempInt;
    uint8_t tempFrac;
    
    // 启动温度转换
    MAX30102_WriteReg(MAX30102_TEMP_CONFIG, 0x01);
    Delay_ms(30);
    
    tempInt = (int8_t)MAX30102_ReadReg(MAX30102_TEMP_INT);
    tempFrac = MAX30102_ReadReg(MAX30102_TEMP_FRAC);
    
    return (float)tempInt + ((float)tempFrac * 0.0625);
}

/**
  * 函    数：获取芯片ID
  * 参    数：无
  * 返 回 值：芯片ID（应为0x15）
  */
uint8_t MAX30102_GetPartID(void)
{
    return MAX30102_ReadReg(MAX30102_PART_ID);
}
