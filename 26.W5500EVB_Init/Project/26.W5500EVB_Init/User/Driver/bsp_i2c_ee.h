#ifndef __I2C_EE_H
#define	__I2C_EE_H

#include "stm32f10x.h"

/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */

/* AT24C01/02ÿҳ��8���ֽ� 
 * AT24C04/08A/16Aÿҳ��16���ֽ� 
 */
	

#define EE_DEV_ADDR			0xA0		/* 24xx02���豸��ַ */
#define EE_PAGE_SIZE		8			  /* 24xx02��ҳ���С */
#define EE_SIZE				256			  /* 24xx02������ */


uint8_t ee_CheckOk(void);
uint8_t ee_ReadBytes(uint8_t *_pReadBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t ee_WriteBytes(uint8_t *_pWriteBuf, uint16_t _usAddress, uint16_t _usSize);
uint8_t I2C_WriteBytes(uint16_t _usAddress,uint8_t _pWriteBuf );
uint8_t I2C_ReadBytes(uint16_t _usAddress);
void ee_Erase(void);
void ee_Test(void);

#endif /* __I2C_EE_H */
