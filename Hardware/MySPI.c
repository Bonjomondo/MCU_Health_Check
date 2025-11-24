#include "stm32f10x.h"
#include "MySPI.h"

/**
  * 函    数：SPI初始化
  * 参    数：无
  * 返 回 值：无
  */
void MySPI_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = SPI_CS_PIN | SPI_SCK_PIN | SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	MySPI_Stop();
}

/**
  * 函    数：SPI起始
  * 参    数：无
  * 返 回 值：无
  */
void MySPI_Start(void)
{
	GPIO_ResetBits(SPI_CS_PORT, SPI_CS_PIN);
}

/**
  * 函    数：SPI停止
  * 参    数：无
  * 返 回 值：无
  */
void MySPI_Stop(void)
{
	GPIO_SetBits(SPI_CS_PORT, SPI_CS_PIN);
}

/**
  * 函    数：SPI交换传输一个字节，使用软件模拟SPI协议
  * 参    数：ByteSend 要发送的一个字节
  * 返 回 值：接收的一个字节
  */
uint8_t MySPI_SwapByte(uint8_t ByteSend)
{
	uint8_t i, ByteReceive = 0x00;
	
	for (i = 0; i < 8; i++)
	{
		GPIO_WriteBit(SPI_MOSI_PORT, SPI_MOSI_PIN, (BitAction)(ByteSend & (0x80 >> i)));
		GPIO_SetBits(SPI_SCK_PORT, SPI_SCK_PIN);
		if (GPIO_ReadInputDataBit(SPI_MISO_PORT, SPI_MISO_PIN) == 1){ByteReceive |= (0x80 >> i);}
		GPIO_ResetBits(SPI_SCK_PORT, SPI_SCK_PIN);
	}
	
	return ByteReceive;
}
