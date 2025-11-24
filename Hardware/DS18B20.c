#include "stm32f10x.h"
#include "DS18B20.h"
#include "Delay.h"

/**
  * 函    数：DS18B20设置IO为输出模式
  * 参    数：无
  * 返 回 值：无
  */
static void DS18B20_Mode_Out(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

/**
  * 函    数：DS18B20设置IO为输入模式
  * 参    数：无
  * 返 回 值：无
  */
static void DS18B20_Mode_In(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = DS18B20_PIN;
    GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);
}

/**
  * 函    数：DS18B20初始化
  * 参    数：无
  * 返 回 值：无
  */
void DS18B20_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    DS18B20_Mode_Out();
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
}

/**
  * 函    数：DS18B20复位
  * 参    数：无
  * 返 回 值：0表示成功，1表示失败
  */
uint8_t DS18B20_Reset(void)
{
    uint8_t retry = 0;
    
    DS18B20_Mode_Out();
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    Delay_us(480);
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    Delay_us(60);
    
    DS18B20_Mode_In();
    while(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && retry < 200)
    {
        retry++;
        Delay_us(1);
    }
    
    if(retry >= 200)
        return 1;
    else
        retry = 0;
    
    while(!GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN) && retry < 240)
    {
        retry++;
        Delay_us(1);
    }
    
    if(retry >= 240)
        return 1;
    
    return 0;
}

/**
  * 函    数：DS18B20写一位
  * 参    数：bit 要写的位
  * 返 回 值：无
  */
static void DS18B20_WriteBit(uint8_t bit)
{
    DS18B20_Mode_Out();
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    Delay_us(2);
    
    if(bit)
        GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    else
        GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    
    Delay_us(60);
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
}

/**
  * 函    数：DS18B20读一位
  * 参    数：无
  * 返 回 值：读到的位
  */
static uint8_t DS18B20_ReadBit(void)
{
    uint8_t data;
    
    DS18B20_Mode_Out();
    GPIO_ResetBits(DS18B20_PORT, DS18B20_PIN);
    Delay_us(2);
    GPIO_SetBits(DS18B20_PORT, DS18B20_PIN);
    
    DS18B20_Mode_In();
    Delay_us(12);
    
    if(GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_PIN))
        data = 1;
    else
        data = 0;
    
    Delay_us(50);
    return data;
}

/**
  * 函    数：DS18B20写一个字节
  * 参    数：Byte 要写的字节
  * 返 回 值：无
  */
void DS18B20_WriteByte(uint8_t Byte)
{
    uint8_t i;
    
    for(i = 0; i < 8; i++)
    {
        DS18B20_WriteBit(Byte & 0x01);
        Byte >>= 1;
    }
}

/**
  * 函    数：DS18B20读一个字节
  * 参    数：无
  * 返 回 值：读到的字节
  */
uint8_t DS18B20_ReadByte(void)
{
    uint8_t i, byte = 0;
    
    for(i = 0; i < 8; i++)
    {
        byte >>= 1;
        if(DS18B20_ReadBit())
            byte |= 0x80;
    }
    
    return byte;
}

/**
  * 函    数：DS18B20读取温度
  * 参    数：无
  * 返 回 值：温度值（摄氏度）
  */
float DS18B20_ReadTemperature(void)
{
    uint8_t temp_L, temp_H;
    int16_t temp;
    float temperature;
    
    DS18B20_Reset();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_CONVERT_T);
    Delay_ms(750);
    
    DS18B20_Reset();
    DS18B20_WriteByte(DS18B20_SKIP_ROM);
    DS18B20_WriteByte(DS18B20_READ_SCRATCHPAD);
    
    temp_L = DS18B20_ReadByte();
    temp_H = DS18B20_ReadByte();
    
    temp = (temp_H << 8) | temp_L;
    temperature = temp * 0.0625;
    
    return temperature;
}
