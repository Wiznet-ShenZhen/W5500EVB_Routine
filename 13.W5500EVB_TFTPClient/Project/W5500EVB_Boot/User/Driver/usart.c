#include "usart.h"
#include "ult.h"

u8 USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
u16 USART_RX_STA=0;       //接收状态标记
uint8 datatemp[1024];

void USART1_Init(void)
{
  	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1, ENABLE);
	USART_DeInit(USART1);  //复位串口1
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  	// Configure the USART1_Rx as input floating
  	GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING ;
  	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate =115200;
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
int get_char(void)
{
	unsigned char ch;
	while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET)
	{
	}
	ch = USART_ReceiveData(USART1);
  return ch;
}

/**
*@brief		读取串口数据寄存器
*@param		*key：	
*@return	1：得到串口输入数据， 0：未得到串口输入数据
*/
u8 serial_key_pressed(char *key)
{
	FlagStatus flag  ;
	/* First clear Rx buffer */
	flag = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
	if ( flag == SET)
	{
		*key = (char)USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
*@brief		获取从串口输入的字符
*@param		无
*@return	key：从串口得到的字符
*/
char get_key(void)
{
	char key = 0;
	/* Waiting for user input */
	while (1)
	{
		if (serial_key_pressed((char*)&key)) break;
	}
	return key;
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
	u8 Res;
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
	{
		Res =USART_ReceiveData(USART1);//(USART1->DR);	//读取接收到的数据		
		if((USART_RX_STA&0x8000)==0)//接收未完成
		{
			if(USART_RX_STA&0x4000)//接收到了0x0d
			{
					if(Res!=0x0a)
					USART_RX_STA=0;//接收错误,重新开始
					else USART_RX_STA|=0x8000;	//接收完成了 								
			}
			else //还没收到0X0D
			{	
				if(Res==0x0d)USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//接收数据错误,重新开始接收	  
				}		 
			}
		}			   		 
  }  											 
}


