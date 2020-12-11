
/*STM32��SPI������Χ���߽ӿڣ��������ǽ�STM32��SPI����Ϊȫ˫��ģʽ����NSSʹ�õ����ģʽ��
��ʹ��SPIǰ�����������������Ǳ�����⣬��STM32��Ϊ��������һ���ֽ�����ʱ����Ȼ�ܽ��յ�һ�����ݣ�
���������Ƿ����ɳ��������
�� ȫ˫��ģʽ(BIDIMODE=0����RXONLY=0) 
��  ��д�����ݵ�SPI_DR�Ĵ���(���ͻ�����)�󣬴��俪ʼ�� 
��  �ڴ��͵�һλ���ݵ�ͬʱ�����ݱ����еشӷ��ͻ��������͵�8λ����λ�Ĵ����У�
Ȼ��˳�򱻴��е���λ�͵�MOSI�����ϣ� 
��  ���ͬʱ����MISO�����Ͻ��յ������ݣ���˳�򱻴��е���λ����8λ����λ�Ĵ���
�У�Ȼ�󱻲��еش��͵�SPI_DR�Ĵ���(���ջ�����)�С� 
ע�⣺Ҳ����˵��������ģʽ�£����ͺͽ�����ͬʱ���еģ��������Ƿ�����һ�����ݣ�Ҳ���ܽ��յ�һ�����ݡ�
��STM32�ڲ�Ӳ����������̵�֧��*/


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
	// ��ʼ��GPIO�ܽź�SPI�Ĳ������ã�����SPI��GPIO�ĳ�ʼ���ṹ��
	SPI_InitTypeDef   SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	// APB1��2����ʱ��ʹ��
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);		// ����SPI2ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO , ENABLE);	// ����GPIOB��AFIOʱ��

  // GPIO���ã�����ΪPB���
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 					// ѡ��PB12���к�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// PB12ͨ���ٶ���Ϊ50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		// PB12��Ϊ�������
  GPIO_Init(GPIOB, &GPIO_InitStructure);							// PB12�������
  GPIO_SetBits(GPIOB, GPIO_Pin_12);										// ����FLASH��Ƭѡ�ź���PB12������Ϊ�ߵ�ƽ��Ҳ���ǲ�ѡ��FLASH
	// ����SPI2��SCK, MISO and MOSI 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;		// ѡ��PB13/14/15���к�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		// PB13/14/15ͨ���ٶ���Ϊ50MHz
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// PB13/14/15��Ϊ���������������������ķ�����ȫ���ڲ����ƣ�����Ҫ������
  GPIO_Init(GPIOB, &GPIO_InitStructure);							// PB13/14/15�������
  
	  // SPI����
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;		// SPI����Ϊ˫��˫��ȫ˫��
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;			// ����SPI����ģʽ:����Ϊ��SPI
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;	// ����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;				// ѡ���˴���ʱ�ӵ���̬:ʱ�ӵ�
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;			// ���ݲ����ڵ�1��ʱ����
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;					// NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;		// ���岨����Ԥ��Ƶ��ֵΪ4
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;// ָ�����ݴ����MSBλ����LSBλ��ʼ
	  SPI_InitStructure.SPI_CRCPolynomial = 7;					// CRCֵ����Ķ���ʽ

	  SPI_Init(SPI2, &SPI_InitStructure);								// ����SPI�ṹ���ʼ��������ʼ��SPI�ṹ�� 
	  SPI_Cmd(SPI2, ENABLE);														// ʹ��SPI2����
}

// Connected to Data Flash
void WIZ_CS(uint8_t val)
{
	if (val == LOW) 
	{
   		GPIO_ResetBits(GPIOB, WIZ_SCS); 	// GPIOB��WIZ_SCS��GPIO_Pin_12��������������
	}
	else if (val == HIGH)
	{
   		GPIO_SetBits(GPIOB, WIZ_SCS); 		// GPIOB��WIZ_SCS��GPIO_Pin_12��������1����
	}
}


uint8_t SPI2_SendByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);  	// �ȴ���������
         
	  SPI_I2S_SendData(SPI2, byte);									   									// ����һ���ֽ�
          
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 	// �ȴ�������һ���ֽ�
          
	  return SPI_I2S_ReceiveData(SPI2);								   								// �����յ������� 
}



