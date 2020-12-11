#ifndef __SPI2_H
#define __SPI2_H

#include "stm32f10x.h"

void SPI2_Init(void);
uint8_t SPI2_SendByte(uint8_t byte);
#endif

