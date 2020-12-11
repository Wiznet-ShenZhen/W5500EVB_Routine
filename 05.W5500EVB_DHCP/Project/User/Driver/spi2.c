
/*STM32的SPI串行外围总线接口，本程序，是将STM32的SPI配置为全双工模式，且NSS使用的软件模式。
在使用SPI前，下面的这个过程我们必须理解，即STM32作为主机发送一个字节数据时，必然能接收到一个数据，
至于数据是否处理，由程序操作。
● 全双工模式(BIDIMODE=0并且RXONLY=0) 
─  当写入数据到SPI_DR寄存器(发送缓冲器)后，传输开始； 
─  在传送第一位数据的同时，数据被并行地从发送缓冲器传送到8位的移位寄存器中，
然后按顺序被串行地移位送到MOSI引脚上； 
─  与此同时，在MISO引脚上接收到的数据，按顺序被串行地移位进入8位的移位寄存器
中，然后被并行地传送到SPI_DR寄存器(接收缓冲器)中。 
注意：也就是说，在主机模式下，发送和接收是同时进行的，所以我们发送了一个数据，也就能接收到一个数据。
而STM32内部硬件是这个过程的支撑*/


#include "stm32f10x.h"
#include "config.h"
#include "socket.h"
#include "w5500.h"
//#include "ult.h"
#include "sockutil.h"
#include "util.h"//delay
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void WIZ_SPI_Init(void)
{
	// 初始化GPIO管脚和SPI的参数设置：建立SPI和GPIO的初始化结构体
	SPI_InitTypeDef   SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// APB1、2外设时钟使能
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);		// 开启SPI2时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO , ENABLE);	// 开启GPIOB、AFIO时钟

  // GPIO配置，设置为PB输出
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 					// 选定PB12进行后续设置
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// PB12通信速度设为50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// PB12设为推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);							// PB12完成配置
  GPIO_SetBits(GPIOB, GPIO_Pin_12);										// 配置FLASH的片选信号线PB12，并设为高电平，也就是不选中FLASH
	// 配置SPI2的SCK, MISO and MOSI 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;		// 选定PB13/14/15进行后续设置
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// PB13/14/15通信速度设为50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// PB13/14/15设为复用推挽输出，输入输出的方向完全由内部控制，不需要程序处理
  GPIO_Init(GPIOB, &GPIO_InitStructure);							// PB13/14/15完成配置
  
	  // SPI配置
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI设置为双线双向全双工
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;			// 设置SPI工作模式:设置为主SPI
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	// 设置SPI的数据大小:SPI发送接收8位帧结构
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				// 选择了串行时钟的稳态:时钟低
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;			// 数据捕获于第1个时钟沿
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;					// NSS信号由硬件（NSS管脚）还是软件（使用SSI位）控制
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		// 定义波特率预分频的值为4
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// 指定数据传输从MSB位还是LSB位开始
	  SPI_InitStructure.SPI_CRCPolynomial = 7;					// CRC值计算的多项式

	  SPI_Init(SPI2, &SPI_InitStructure);								// 利用SPI结构体初始化函数初始化SPI结构体 
	  SPI_Cmd(SPI2, ENABLE);														// 使能SPI2外设
}

// Connected to Data Flash
void WIZ_CS(uint8_t val)
{
	if (val == LOW) 
	{
   		GPIO_ResetBits(GPIOB, WIZ_SCS); 	// GPIOB的WIZ_SCS（GPIO_Pin_12）引脚清零拉低
	}
	else if (val == HIGH)
	{
   		GPIO_SetBits(GPIOB, WIZ_SCS); 		// GPIOB的WIZ_SCS（GPIO_Pin_12）引脚置1拉高
	}
}


uint8_t SPI2_SendByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);  	// 等待发送区空
         
	  SPI_I2S_SendData(SPI2, byte);									   									// 发送一个字节
          
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 	// 等待接收完一个字节
          
	  return SPI_I2S_ReceiveData(SPI2);								   								// 返回收到的数据 
}



