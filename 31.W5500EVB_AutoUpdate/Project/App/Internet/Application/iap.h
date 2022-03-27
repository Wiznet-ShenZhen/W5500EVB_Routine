#ifndef __IAP_H_
#define __IAP_H_

uint32 FLASH_PagesMask(vu32 Size);

void Flash_Erase(uint32 dest, uint32 len);

void Flash_Write_Ready(void);

void Flash_Write_Done(void);

void Flash_Write(uint32 dest, uint8* data, uint32 len);

#endif

