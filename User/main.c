#include "stm32f10x.h" // Device header
#include "Delay.h"
#include "OLED.h"
#include "Serial.h"

uint8_t RxData; // 定义用于接收串口数据的变量
char tvoc_string[32];
char hcho_string[32];
char eco2_string[32];
int main(void)
{
    /*模块初始化*/
    OLED_Init(); // OLED初始化
    /*显示静态字符串*/
		
    while (1)
    {
			
		}
}

