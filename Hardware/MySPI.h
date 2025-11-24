#ifndef __MYSPI_H
#define __MYSPI_H

#include "stm32f10x.h"

// SPI引脚配置
#define SPI_CS_PORT  GPIOA
#define SPI_CS_PIN   GPIO_Pin_4
#define SPI_SCK_PORT GPIOA
#define SPI_SCK_PIN  GPIO_Pin_5
#define SPI_MISO_PORT GPIOA
#define SPI_MISO_PIN GPIO_Pin_6
#define SPI_MOSI_PORT GPIOA
#define SPI_MOSI_PIN GPIO_Pin_7

void MySPI_Init(void);
void MySPI_Start(void);
void MySPI_Stop(void);
uint8_t MySPI_SwapByte(uint8_t ByteSend);

#endif
