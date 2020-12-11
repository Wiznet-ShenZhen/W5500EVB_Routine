#include "stm32f10x.h"
#include "config.h"
#include "socket.h"
#include "w5500.h"
#include "ult.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void WIZ_SPI_Init(void)
{
	SPI_InitTypeDef   SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_SPI2, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO , ENABLE);
  /* Configure SPIy pins: SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  GPIO_SetBits(GPIOB, GPIO_Pin_12);
	  /* SPI Config -------------------------------------------------------------*/
	  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	  SPI_InitStructure.SPI_CRCPolynomial = 7;

	  SPI_Init(SPI2, &SPI_InitStructure);
	  SPI_Cmd(SPI2, ENABLE);
}

void WIZ_CS(uint8_t val)
{
	if (val == LOW) 
	{
   		GPIO_ResetBits(GPIOB, WIZ_SCS); 
	}
	else if (val == HIGH)
	{
   		GPIO_SetBits(GPIOB, WIZ_SCS); 
	}
}

uint8_t SPI2_SendByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
         
	  SPI_I2S_SendData(SPI2, byte);
          
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
          
	  return SPI_I2S_ReceiveData(SPI2);
}



