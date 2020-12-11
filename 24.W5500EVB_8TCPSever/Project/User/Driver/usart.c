#include "usart.h"

u8 USART_RX_BUF[USART_REC_LEN];     // ����һ�����ڴ������ݽ��յĻ��棬��СΪUSART_REC_LEN
u16 USART_RX_STA=0;       					// ���岢��ʼ�����ڽ��ձ�־λ

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
/* ��Printf���ݷ������� */
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

// ����1�жϷ������
void USART1_IRQHandler(void)                	
{
	u8 Res;																					// ����һ�����ڽ�������ֵ�ı���
	// �жϴ�����ؼĴ����Ƿ���յ����ݣ������յ����ݣ�������ж�(���յ������ݱ�����0x0d��0x0a��˳���β)
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		Res = USART_ReceiveData(USART1);							// USART1->DR����ȡ���յ�������		
		
		if((USART_RX_STA & 0x8000)==0)								// �ж��Ƿ������ɣ��ж��Ƿ�ͬʱ�յ�0x0d��0x0a
		{
			if(USART_RX_STA & 0x4000)										// ���յ���0x0d
			{
					if(Res!=0x0a)														// δ���յ�0x0a
						USART_RX_STA = 0;											// ���մ���,���ڽ��ձ�־λ��0�����½���
					else 
						USART_RX_STA|=0x8000;									// ���յ���0x0a���������								
			}
			else 																				// δ���յ�0x0d
			{	
				if(Res==0x0d)
					USART_RX_STA|=0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA & 0X3FFF]=Res;
					USART_RX_STA++;
					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;												// �������ݴ���,���¿�ʼ����	  
				}		 
			}
		}			   		 
  }  											 
} 
