#ifndef __FLASH_H__
#define __FLASH_H__
#include "types.h"
//FLASH��ʼ��ַ
//#define STM32_FLASH_BASE            0X08020000
extern uint8 flash_Buffer[];//ָ����ַ��ʼ��ȡָ����������
void FLASH_Write(uint32 WriteAddr,uint16 *pBuffer,uint16 NumToWrite);		//��ָ����ַ��ʼд��ָ�����ȵ�����
void FLASH_Read(uint32 ReadAddr,uint16 *pBuffer,uint16 size) ;
uint32 FLASH_PagesMask(uint32 Size);
void copy_app(uint32 fw_len);							   
#endif

