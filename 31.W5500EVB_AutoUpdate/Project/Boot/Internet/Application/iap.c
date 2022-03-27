#include "iap.h"
#include "config.h"
/*******************************************************************************
* Function Name  : FLASH_PagesMask
* Description    : Calculate the number of pages
* Input          : - Size: The image size
* Output         : None
* Return         : The number of pages
*******************************************************************************/
uint32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if((size % 0x400) != 0)
  {
    pagenumber = (size / 0x400) + 1;
  }
  else
  {
    pagenumber = size / 0x400;
  }
  return pagenumber;
}


bool copy_app(uint32 fw_len, uint32 fw_checksum)
{
	uint32 i,nErasedPage;
	
  if(fw_len>0)
  {
    uint32 nPage=FLASH_PagesMask(fw_len);
    uint32 checksum=0;
    //unlock flash
    FLASH_Unlock();
    // Clear All pending flags
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    //erase required pages
    for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
    {
      FLASH_ErasePage(ApplicationAddress + 0x400*nErasedPage);
      IWDG_ReloadCounter();//feed iwdg
    }
    //copy app from backup address to real address
    for(i=0; i<fw_len; i+=2)
    {
      FLASH_ProgramHalfWord(ApplicationAddress+i, *(uint16*)(AppBackupAddress+i));
      checksum+=*(uint16*)(ApplicationAddress+i);
      IWDG_ReloadCounter();//feed iwdg
    }
		//²Á³ýbackup flash
		for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
    {
      FLASH_ErasePage(AppBackupAddress + 0x400*nErasedPage);
      IWDG_ReloadCounter();//feed iwdg
    }
		
    //lock flash again
    FLASH_Lock();
		return TRUE;
    //is checksum correct?
	#if 0
    if(checksum==fw_checksum)
    {    
//      printf(" ConfigMsg.fw_checksum = %d, copy calc checksum = %d\r\n", fw_checksum, checksum);
//			printf("fw_len = %d\r\n", fw_len);
			return TRUE;
    }
    else
    {
      printf(" ConfigMsg.fw_checksum = %d, copy calc checksum = %d\r\n", fw_checksum, checksum);
      printf("fw_len = %d\r\n", fw_len);
      return FALSE;
    }
	#endif
  }
  else
    return FALSE;
}
