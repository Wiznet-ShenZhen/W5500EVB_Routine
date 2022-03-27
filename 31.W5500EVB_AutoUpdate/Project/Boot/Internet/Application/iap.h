#ifndef __IAP_H_
#define __IAP_H_

#include "stm32f10x.h"
#include "types.h"
uint32 FLASH_PagesMask(vu32 Size);
bool copy_app(uint32 fw_len, uint32 fw_checksum);
#endif

