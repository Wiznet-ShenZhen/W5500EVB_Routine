#include "usart.h"

u8 USART_RX_BUF[USART_REC_LEN];     // ����һ�����ڴ������ݽ��յĻ��棬��СΪUSART_REC_LEN
u16 USART_RX_STA=0;       					// ���岢��ʼ�����ڽ��ձ�־λ

void USART1_Init(void)
{
	// ����USART��GPIO�ĳ�ʼ���ṹ��
  USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// ����GPIOA��USART1��AFIOʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_USART1|RCC_APB2Periph_AFIO , ENABLE);

	// ����USART1_TxΪ�����������������Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;		// ��Ϊ���������������������ķ�����ȫ���ڲ����ƣ�����Ҫ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;					// ѡ��PA9���к�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// PA9ͨ���ٶ���Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);						// PA9������� 
  // ����USART1_RxΪ�������룺���ա������߼��������ڲ��ṹ������ʱ���൱�ڽ��˸ߵ�ƽ��һ�㲻�������գ����ܸ��š�
  GPIO_InitStructure.GPIO_Mode =GPIO_Mode_IN_FLOATING ;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;				// ѡ��PA10���к�������
  GPIO_Init(GPIOA, &GPIO_InitStructure);						// PA10�������
	
	// USART����
  USART_InitStructure.USART_BaudRate = 115200;			// ���ò�����
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;		// ���ô���λ��
  USART_InitStructure.USART_StopBits = USART_StopBits_1;				// ����ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No ;					// ������żУ��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;		// ����Ӳ�������ƹ���
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;				// ���ù�����ʽΪ���ܷ�ʽ 
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); 		// ��USART1�����ж�   
  USART_Init(USART1, &USART_InitStructure);					// ��ʼ��USART1
  USART_Cmd(USART1, ENABLE);												// ��USART1
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

