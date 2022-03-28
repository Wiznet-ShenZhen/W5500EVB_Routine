#include "usart.h"

u8 USART_RX_BUF[USART_REC_LEN];     // ����һ�����ڴ������ݽ��յĻ��棬��СΪUSART_REC_LEN
u16 USART_RX_STA=0;       					// ���岢��ʼ�����ڽ��ձ�־λ

int flag_0x0d = 0;									// ���ڽ���"0x0d"��־λ
int flag_0x0a = 0;									// ���ڽ���"0x0a"��־λ

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

// ����1�жϷ���
void USART1_IRQHandler(void)                	
{
	u8 Res;																								// ����һ�����ڽ�������ֵ�ı���
	// 0x4000��Ϊx0d�ı�־λ��0x8000��Ϊx0a�ı�־λ
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж��Ƿ��⵽�����ݽ���
	{
		Res = USART_ReceiveData(USART1);										// �����ڽ��յ������ݸ�ֵ��Res		
		
		if((USART_RX_STA & 0x8000)==0)											// �ж��Ƿ���յ�0x0a
		{
			if(USART_RX_STA & 0x4000)													// 3.��⵽0x0d��־λ��Ч
			{
					if(Res!=0x0a)																	// δ���յ�0x0a
					{
						USART_RX_BUF[USART_RX_STA&0X3FFF]=0X0D; 		//���ϸոս��յ���0X0D��USART_RX_BUF[]��
						USART_RX_STA++;
						USART_RX_BUF[USART_RX_STA&0X3FFF]=Res;   		//����ǰ���յ������ݷ���USART_RX_BUF[]��
						USART_RX_STA++;
						USART_RX_STA &= 0xBFFF;   				//����ո�0x0d�Ľ��ձ�־ (0xBFFF = 1011 1111 1111 1111)
					}
					else 
						USART_RX_STA|=0x8000;												// 4.���0x0a��0x0a��־λ0x8000��Ч								
			}
			else
			{	
				if(Res==0x0d)	
					USART_RX_STA|=0x4000;													// 2.���0x0d��0x0d��־λ0x4000��Ч
				else																						// 1.���ڽ��յ��˵�n������
				{
					USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;		// �����ݷŵ����ڽ��ջ�������USART_RX_BUF[n-1]
					USART_RX_STA++;																// ����û���յ�0x0a�������ϡ�USART_RX_STA & 0x8000 = 1����������ݻ�����ڽ��ջ����жѻ�

					if(USART_RX_STA>(USART_REC_LEN-1))
						USART_RX_STA=0;															// �������ݴ���,���¿�ʼ����
				}		 
			}
		}			   		 
  }  											 
}

//void USART1_IRQHandler(void)                	
//{
//	u8 Res;																								// ����һ�����ڽ�������ֵ�ı���
//	// 0x4000��Ϊx0d�ı�־λ��0x8000��Ϊx0a�ı�־λ
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) // �����ж��Ƿ��⵽�����ݽ���
//	{
//		Res = USART_ReceiveData(USART1);										// �����ڽ��յ������ݸ�ֵ��Res		
//		
//		
//		if(flag_0x0a == 0)											// �ж��Ƿ���յ�0x0a
//		{
//			if(flag_0x0d == 1)													// 3.��⵽0x0d��־λ��Ч
//			{
//					if(Res != 0x0a)																	// δ���յ�0x0a
//					{
//						USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;
//						USART_RX_STA++;
//						flag_0x0a = 0;
//						flag_0x0d = 0;
//					}
//					else 
//						flag_0x0a = 1;												// 4.���0x0a��0x0a��־λ0x8000��Ч								
//			}
//			else
//			{	
//				if(Res == 0x0d)	
//					flag_0x0d = 1;																// 2.���0x0d��0x0d��־λ0x4000��Ч
//				else																						// 1.���ڽ��յ��˵�n������
//				{
//					USART_RX_BUF[USART_RX_STA & 0x3FFF] = Res;		// �����ݷŵ����ڽ��ջ�������USART_RX_BUF[n-1]
//					USART_RX_STA++;																// ����û���յ�0x0a�������ϡ�USART_RX_STA & 0x8000 = 1����������ݻ�����ڽ��ջ����жѻ�

//					if(USART_RX_STA > (USART_REC_LEN-1))
//						USART_RX_STA=0;															// �����ջ������ˣ����USART_RX_BUF[0]��ʼ����
//						printf("USART_RX_BUF is full now, data will be covered! \r\n");
//				}		 
//			}
//		}			   		 
//  }  											 
//}
