#include "usart.h"

u8 USART_RX_BUF[USART_REC_LEN];     // 定义一个用于串口数据接收的缓存，大小为USART_REC_LEN
u16 USART_RX_STA=0;       					// 定义并初始化串口接收标志位

int flag_0x0d = 0;									// 串口接收"0x0d"标志位
int flag_0x0a = 0;									// 串口接收"0x0a"标志位

void USART1_Init(void)
{
  USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO , ENABLE);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  // Configure the USART1_Rx as input floating
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING ;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No ;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //Enable rx enable, 
  
  /* Configure the USARTx */ 
  USART_Init(USART1, &USART_InitStructure);
  /* Enable the USARTx */
  USART_Cmd(USART1, ENABLE);
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

// 串口1中断服务
void USART1_IRQHandler(void)                	
{
	u8 Res;																								// 定义一个串口接收数据值的变量
	// 0x4000作为x0d的标志位，0x8000作为x0a的标志位
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 串口中断是否检测到有数据进来
	{
		Res = USART_ReceiveData(USART1);										// 将串口接收到的数据赋值给Res		
		
		if((USART_RX_STA & 0x8000)==0)											// 判断是否接收到0x0a
		{
			if(USART_RX_STA & 0x4000)													// 3.检测到0x0d标志位生效
			{
					if(Res!=0x0a)																	// 未接收到0x0a
					{
						USART_RX_BUF[USART_RX_STA&0X3FFF]=0X0D; 		//补上刚刚接收到的0X0D到USART_RX_BUF[]中
						USART_RX_STA++;
						USART_RX_BUF[USART_RX_STA&0X3FFF]=Res;   		//将当前接收到的数据放入USART_RX_BUF[]中
						USART_RX_STA++;
						USART_RX_STA &= 0xBFFF;   				//清除刚刚0x0d的接收标志 (0xBFFF = 1011 1111 1111 1111)
					}
					else 
						USART_RX_STA|=0x8000;												// 4.检测0x0a，0x0a标志位0x8000生效								
			}
			else
			{	
				if(Res==0x0d)	
					USART_RX_STA|=0x4000;													// 2.检测0x0d，0x0d标志位0x4000生效
				else																						// 1.串口接收到了第n个数据
				{
					USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;		// 将数据放到串口接收缓存数组USART_RX_BUF[n-1]
					USART_RX_STA++;																// 由于没接收到0x0a，不符合“USART_RX_STA & 0x8000 = 1”，因此数据会继续在接收缓存中堆积

					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;															// 接收数据错误,重新开始接收
				}		 
			}
		}			   		 
  }  											 
}

//void USART1_IRQHandler(void)                	
//{
//	u8 Res;																								// 定义一个串口接收数据值的变量
//	// 0x4000作为x0d的标志位，0x8000作为x0a的标志位
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // 串口中断是否检测到有数据进来
//	{
//		Res = USART_ReceiveData(USART1);										// 将串口接收到的数据赋值给Res		
//		
//		
//		if(flag_0x0a == 0)											// 判断是否接收到0x0a
//		{
//			if(flag_0x0d == 1)													// 3.检测到0x0d标志位生效
//			{
//					if(Res != 0x0a)																	// 未接收到0x0a
//					{
//						USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;
//						USART_RX_STA++;
//						flag_0x0a = 0;
//						flag_0x0d = 0;
//					}
//					else 
//						flag_0x0a = 1;												// 4.检测0x0a，0x0a标志位0x8000生效								
//			}
//			else
//			{	
//				if(Res == 0x0d)	
//					flag_0x0d = 1;																// 2.检测0x0d，0x0d标志位0x4000生效
//				else																						// 1.串口接收到了第n个数据
//				{
//					USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;		// 将数据放到串口接收缓存数组USART_RX_BUF[n-1]
//					USART_RX_STA++;																// 由于没接收到0x0a，不符合“USART_RX_STA & 0x8000 = 1”，因此数据会继续在接收缓存中堆积

//					if(USART_RX_STA > (USART_REC_LEN-1))
//						USART_RX_STA=0;															// 若接收缓存满了，则从USART_RX_BUF[0]开始覆盖
//						printf("USART_RX_BUF is full now, data will be covered! \r\n");
//				}		 
//			}
//		}			   		 
//  }  											 
//}
