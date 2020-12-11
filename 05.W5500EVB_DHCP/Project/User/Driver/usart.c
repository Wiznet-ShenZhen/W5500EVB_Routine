#include "usart.h"

void USART1_Init(void)
{
	// 建立USART和GPIO的初始化结构体
  USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 开启GPIOA、USART1、AFIO时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO , ENABLE);

	// 配置USART1_Tx为复用推挽输出，可作为串口输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		// 设为复用推挽输出，输入输出的方向完全由内部控制，不需要程序处理
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					// 选定PA9进行后续设置
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// PA9通信速度设为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);						// PA9完成配置 
  // 配置USART1_Rx为浮空输入：悬空。由于逻辑器件的内部结构，悬空时，相当于接了高电平。一般不建议悬空，易受干扰。
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING ;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				// 选定PA10进行后续设置
  GPIO_Init(GPIOA, &GPIO_InitStructure);						// PA10完成配置
	
	// USART配置
  USART_InitStructure.USART_BaudRate = 115200;			// 设置波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// 设置传输位宽
  USART_InitStructure.USART_StopBits = USART_StopBits_1;				// 设置停止位
  USART_InitStructure.USART_Parity = USART_Parity_No ;					// 设置奇偶校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// 设置硬件流控制功能
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				// 设置工作方式为接受方式 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 		// 打开USART1接收中断   
  USART_Init(USART1, &USART_InitStructure);					// 初始化USART1
  USART_Cmd(USART1, ENABLE);												// 打开USART1
}

int fputc(int ch, FILE *f)
{
/* 将Printf内容发往串口 */
  USART_SendData(USART1, (unsigned char) ch);
  while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET); 
  return (ch);
}

void USART1_Putc(unsigned char c)
{
    USART_SendData(USART1, c);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET );
}

void USART1_Puts(char * str)
{
    while(*str)
    {
        USART_SendData(USART1, *str++);
        while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }
}

