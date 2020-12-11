#include "bsp_spi_flash.h"
#include "w5500_conf.h"
#include "bsp_usart1.h"

#define Dummy_Byte                    0xFF
/*******************************************************************************
* Function Name  : SPI_FLASH_Init
* Description    : Initializes the peripherals used by the SPI FLASH driver.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_FLASH_Init(void)
{
  SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	#ifdef  STM32_W5500EVB
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);
				
		/*!< Configure SPI_FLASH_SPI pins: SCK */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI2, &SPI_InitStructure);
		SPI_Cmd(SPI2, ENABLE);
	#endif
	#ifdef  WILDFIRE_STM32
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOD, ENABLE);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
		
		/*!< Configure SPI_FLASH_SPI pins: SCK */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
		/*!< Configure SPI_FLASH_SPI_CS_PIN pin: SPI_FLASH Card CS pin */
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI1, &SPI_InitStructure);
		SPI_Cmd(SPI1, ENABLE);
	#endif
}

u8 SPI_FLASH_ReadByte(void)
{
  return (SPI_FLASH_SendByte(Dummy_Byte));
}

/*******************************************************************************
* Function Name  : SPI_FLASH_SendByte
* Description    : Sends a byte through the SPI interface and return the byte
*                  received from the SPI bus.
* Input          : byte : byte to send.
* Output         : None
* Return         : The value of the received byte.
*******************************************************************************/
u8 SPI_FLASH_SendByte(u8 byte)
{
	#ifdef  STM32_W5500EVB
	  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI2, byte);
		while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
		return SPI_I2S_ReceiveData(SPI2);
	#endif
	#ifdef  WILDFIRE_STM32
	  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
		SPI_I2S_SendData(SPI1, byte);
		while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
		return SPI_I2S_ReceiveData(SPI1);
	#endif
}

