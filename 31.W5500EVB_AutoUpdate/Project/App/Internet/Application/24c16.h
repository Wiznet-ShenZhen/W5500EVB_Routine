#ifndef __24c16_H
#define __24c16_H

#include "stm32f10x.h"
#include "types.h"

#define I2C_SCK	        GPIO_Pin_6  //out
#define I2C_SDA	        GPIO_Pin_7

void at24c16_init(void);
void at24c16_write(uint16 addr, unsigned char val);
unsigned char at24c16_read(uint16 addr);

void eep_block_write(uint16 eepAddr, uint8* dat, uint16 index, uint16 len);

void erase_eeprom(uint16 startAddr, uint16 len);

#endif /* __MAIN_H */




