#ifndef __FLASH_H__
#define __FLASH_H__
#include "types.h"
//FLASH起始地址
//#define STM32_FLASH_BASE            0X08020000
extern uint8 flash_Buffer[];//指定地址开始读取指定长度数据
void FLASH_Write(uint32 WriteAddr,uint16 *pBuffer,uint16 NumToWrite);		//从指定地址开始写入指定长度的数据
void FLASH_Read(uint32 ReadAddr,uint16 *pBuffer,uint16 size) ;
uint32 FLASH_PagesMask(uint32 Size);
void copy_app(uint32 fw_len);							   
#endif

