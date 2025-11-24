#include "stm32f10x.h"                  // Device header
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"

uint8_t Serial_RxData[4];		//定义串口接收的数据变量，索引1,2,3
uint8_t Serial_RxFlag[4];		//定义串口接收的标志位变量，索引1,2,3

/**
  * 函    数：串口初始化
  * 参    数：usart_num 串口编号 (1,2,3)
  * 参    数：baud_rate 波特率
  * 返 回 值：无
  */
void Serial_Init(uint8_t usart_num, uint32_t baud_rate)
{
    USART_TypeDef* usart;
    GPIO_TypeDef* gpio_port;
    uint16_t tx_pin, rx_pin;
    uint32_t rcc_usart, rcc_gpio;
    IRQn_Type irq_channel;
    switch(usart_num){
        case 1:
            usart = USART1;
            gpio_port = GPIOA;
            tx_pin = GPIO_Pin_9;
            rx_pin = GPIO_Pin_10;
            rcc_usart = RCC_APB2Periph_USART1;
            rcc_gpio = RCC_APB2Periph_GPIOA;
            irq_channel = USART1_IRQn;
            break;
        case 2:
            usart = USART2;
            gpio_port = GPIOA;
            tx_pin = GPIO_Pin_2;
            rx_pin = GPIO_Pin_3;
            rcc_usart = RCC_APB1Periph_USART2;
            rcc_gpio = RCC_APB2Periph_GPIOA;
            irq_channel = USART2_IRQn;
            break;
        case 3:
            usart = USART3;
            gpio_port = GPIOB;
            tx_pin = GPIO_Pin_10;
            rx_pin = GPIO_Pin_11;
            rcc_usart = RCC_APB1Periph_USART3;
            rcc_gpio = RCC_APB2Periph_GPIOB;
            irq_channel = USART3_IRQn;
            break;
        default:
            return;
    }
    /*开启时钟*/
    if(usart_num == 1){
        RCC_APB2PeriphClockCmd(rcc_usart, ENABLE);
    }else{
        RCC_APB1PeriphClockCmd(rcc_usart, ENABLE);
    }
    RCC_APB2PeriphClockCmd(rcc_gpio, ENABLE);
    
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = tx_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpio_port, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = rx_pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(gpio_port, &GPIO_InitStructure);
    
    /*USART初始化*/
    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = baud_rate;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(usart, &USART_InitStructure);
    
    /*中断输出配置*/
    USART_ITConfig(usart, USART_IT_RXNE, ENABLE);
    
    /*NVIC中断分组*/
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    
    /*NVIC配置*/
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = irq_channel;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);
    
    /*USART使能*/
    USART_Cmd(usart, ENABLE);
}

/**
  * 函    数：获取USART指针
  * 参    数：usart_num 串口编号
  * 返 回 值：USART_TypeDef*
  */
USART_TypeDef* Serial_GetUSART(uint8_t usart_num)
{
    switch(usart_num){
        case 1: return USART1;
        case 2: return USART2;
        case 3: return USART3;
        default: return NULL;
    }
}

/**
  * 函    数：串口发送一个字节
  * 参    数：usart_num 串口编号
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t usart_num, uint8_t Byte)
{
    USART_TypeDef* usart = Serial_GetUSART(usart_num);
    if(usart == NULL) return;
    USART_SendData(usart, Byte);
    while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET);
}

/**
  * 函    数：串口发送一个数组
  * 参    数：usart_num 串口编号
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
void Serial_SendArray(uint8_t usart_num, uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(usart_num, Array[i]);
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：usart_num 串口编号
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
void Serial_SendString(uint8_t usart_num, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(usart_num, String[i]);
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将X累乘到结果
	}
	return Result;
}

/**
  * 函    数：串口发送数字
  * 参    数：usart_num 串口编号
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
void Serial_SendNumber(uint8_t usart_num, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(usart_num, Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(1, ch);			//将printf的底层重定向到自己的发送字节函数，默认USART1
	return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：usart_num 串口编号
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(uint8_t usart_num, char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(usart_num, String);
}

/**
  * 函    数：获取串口接收标志位
  * 参    数：usart_num 串口编号
  * 返 回 值：串口接收标志位，范围：0~1，接收到数据后，标志位置1，读取后标志位自动清零
  */
uint8_t Serial_GetRxFlag(uint8_t usart_num)
{
	if (Serial_RxFlag[usart_num] == 1)
	{
		Serial_RxFlag[usart_num] = 0;
		return 1;
	}
	return 0;
}

/**
  * 函    数：获取串口接收的数据
  * 参    数：usart_num 串口编号
  * 返 回 值：接收的数据，范围：0~255
  */
uint8_t Serial_GetRxData(uint8_t usart_num)
{
	return Serial_RxData[usart_num];
}

/**
  * 函    数：USART1中断函数
  * 参    数：无
  * 返 回 值：无
  * 注意事项：此函数为中断函数，无需调用，中断触发后自动执行
  *           函数名为预留的指定名称，可以从启动文件复制
  *           请确保函数名正确，不能有任何差异，否则中断函数将不能进入
  */
void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxData[1] = USART_ReceiveData(USART1);
		Serial_RxFlag[1] = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

/**
  * 函    数：USART2中断函数
  * 参    数：无
  * 返 回 值：无
  */
//void USART2_IRQHandler(void)
//{
//	if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
//	{
//		Serial_RxData[2] = USART_ReceiveData(USART2);
//		Serial_RxFlag[2] = 1;
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
//	}
//}

/**
  * 函    数：USART3中断函数
  * 参    数：无
  * 返 回 值：无
  */
void USART3_IRQHandler(void)
{
	if (USART_GetITStatus(USART3, USART_IT_RXNE) == SET)
	{
		Serial_RxData[3] = USART_ReceiveData(USART3);
		Serial_RxFlag[3] = 1;
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

