#include "usart.h"

u8 USART_RX_BUF[USART_REC_LEN];     // 定义一个用于串口数据接收的缓存，大小为USART_REC_LEN
u16 USART_RX_STA=0;       					// 定义并初始化串口接收标志位

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

// 串口1中断服务程序
void USART1_IRQHandler(void)                	
{
	u8 Res;																					// 定义一个串口接收数据值的变量
	// 判断串口相关寄存器是否接收到数据，若接收到数据，则进入中断(接收到的数据必须以0x0d、0x0a按顺序结尾)
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART1);							// USART1->DR，读取接收到的数据		
		
		if((USART_RX_STA & 0x8000)==0)								// 判断是否接收完成：判断是否同时收到0x0d和0x0a
		{
			if(USART_RX_STA & 0x4000)										// 接收到了0x0d
			{
					if(Res!=0x0a)														// 未接收到0x0a
						USART_RX_STA = 0;											// 接收错误,串口接收标志位置0，重新接收
					else 
						USART_RX_STA|=0x8000;									// 接收到了0x0a，接收完成								
			}
			else 																				// 未接收到0x0d
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF]=Res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;												// 接收数据错误,重新开始接收	  
				}		 
			}
		}			   		 
  }  											 
} 

