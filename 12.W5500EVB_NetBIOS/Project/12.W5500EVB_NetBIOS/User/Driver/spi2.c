#include "stm32f10x.h"

void SPI2_Init(void)
{
	SPI_InitTypeDef   SPI_InitStructure;

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
	  
	  /* Enable SPI */
	  SPI_Cmd(SPI2, ENABLE);

}


uint8_t SPI2_SendByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
         
	  SPI_I2S_SendData(SPI2, byte);
          
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
          
	  return SPI_I2S_ReceiveData(SPI2);
}
/*
void SPI1_TXByte(uint8_t byte)
{
	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);       

	  SPI_I2S_SendData(SPI1, byte);	
}
*/


