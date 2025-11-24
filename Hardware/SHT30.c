#include "stm32f10x.h"
#include "SHT30.h"
#include "MyI2C.h"
#include "Delay.h"

/**
  * 函    数：SHT30写命令
  * 参    数：cmd 命令
  * 返 回 值：无
  */
void SHT30_WriteCmd(uint16_t cmd)
{
    MyI2C_Start();
    MyI2C_SendByte(SHT30_ADDR);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(cmd >> 8);
    MyI2C_ReceiveAck();
    MyI2C_SendByte(cmd & 0xFF);
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

/**
  * 函    数：SHT30复位
  * 参    数：无
  * 返 回 值：无
  */
void SHT30_Reset(void)
{
    SHT30_WriteCmd(SHT30_CMD_SOFT_RESET);
    Delay_ms(50);
}

/**
  * 函    数：SHT30初始化
  * 参    数：无
  * 返 回 值：无
  */
void SHT30_Init(void)
{
    MyI2C_Init();
    SHT30_Reset();
}

/**
  * 函    数：计算CRC8校验
  * 参    数：data 数据
  * 参    数：len 数据长度
  * 返 回 值：CRC值
  */
static uint8_t SHT30_CalcCRC(uint8_t *data, uint8_t len)
{
    uint8_t crc = 0xFF;
    uint8_t i, j;
    
    for(i = 0; i < len; i++)
    {
        crc ^= data[i];
        for(j = 0; j < 8; j++)
        {
            if(crc & 0x80)
                crc = (crc << 1) ^ 0x31;
            else
                crc = crc << 1;
        }
    }
    
    return crc;
}

/**
  * 函    数：SHT30读取数据
  * 参    数：data 数据结构指针
  * 返 回 值：无
  */
void SHT30_ReadData(SHT30_Data *data)
{
    uint8_t buffer[6];
    uint16_t temp, humi;
    uint8_t i;
    
    // 发送测量命令
    SHT30_WriteCmd(SHT30_CMD_MEAS_HIGH);
    Delay_ms(20);
    
    // 读取数据
    MyI2C_Start();
    MyI2C_SendByte(SHT30_ADDR | 0x01);
    MyI2C_ReceiveAck();
    
    for(i = 0; i < 6; i++)
    {
        buffer[i] = MyI2C_ReceiveByte();
        if(i < 5)
            MyI2C_SendAck(0);
        else
            MyI2C_SendAck(1);
    }
    MyI2C_Stop();
    
    // 校验温度数据
    if(SHT30_CalcCRC(&buffer[0], 2) == buffer[2])
    {
        temp = (buffer[0] << 8) | buffer[1];
        data->Temperature = -45.0 + 175.0 * temp / 65535.0;
    }
    
    // 校验湿度数据
    if(SHT30_CalcCRC(&buffer[3], 2) == buffer[5])
    {
        humi = (buffer[3] << 8) | buffer[4];
        data->Humidity = 100.0 * humi / 65535.0;
    }
}
