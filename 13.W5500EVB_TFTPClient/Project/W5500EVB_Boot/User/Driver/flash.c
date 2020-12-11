#include "stm32f10x.h"
#include "config.h"
#include "flash.h"
uint8 len=0;
uint8 flash_Buffer[512];
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 FLASH_ReadHalfWord(uint32 faddr)
{
	return *(vu16*)faddr; 
}

void FLASH_Write(uint32 WriteAddr,uint16 *pBuffer,uint16 size)   
{ 			 		 
        u16 i;
        FLASH_Unlock();	
        FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
				FLASH_ErasePage(WriteAddr);//������ҳ	
        for(i=0;i<size;i++)
        {
            FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
            WriteAddr+=2;//��ַ����2. 
        }
        FLASH_Lock();//����
}

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void FLASH_Read(uint32 ReadAddr,uint16 *pBuffer,uint16 size)   	
{
    u16 i;
    for(i=0;i<size;i++)
    {
        pBuffer[i]=FLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
        ReadAddr+=2;//ƫ��2���ֽ�.	
    }
}
uint32 FLASH_PagesMask(uint32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if((size % 0x800) != 0)
  {
    pagenumber = (size / 0x800) + 1;
  }
  else
  {
    pagenumber = size / 0x800;
  }
  return pagenumber;
}
void copy_app(uint32 fw_len)
{
  u32 i=0;
	uint32 FLASH_WRITE_ADDR = 0x08000000;
  vu32 nErasedPage=0;
  if(fw_len>0)
  {
    uint32 nPage=FLASH_PagesMask(fw_len);

    //unlock flash
    FLASH_Unlock();
    // Clear All pending flags
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    //erase required pages
    for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
    {
      FLASH_ErasePage(FLASH_WRITE_ADDR + 0x800*nErasedPage);
    }
    //copy app from backup address to real address
    for(i=0; i<fw_len; i+=2)
    {
      FLASH_ProgramHalfWord(FLASH_WRITE_ADDR+i, *(uint16*)(FLASH_WRITE_ADDR+i));
    }
    //lock flash again
    FLASH_Lock();
  }
}

