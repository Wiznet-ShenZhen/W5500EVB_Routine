#include "usart.h"

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

