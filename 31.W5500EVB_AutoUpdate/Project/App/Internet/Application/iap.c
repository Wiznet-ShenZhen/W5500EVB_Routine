#include <stdio.h>
#include "types.h"
#include "stm32f10x.h"

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

	if ((size % 0x400) != 0)
	{
		pagenumber = (size / 0x400) + 1;
	}
	else
	{
		pagenumber = size / 0x400;
	}
	return pagenumber;
}

void Flash_Erase(uint32 dest, uint32 len)
{
	uint32 i, page = 0;

	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

	page = FLASH_PagesMask(len);

	for (i = 0; i < page; i++)
	{
		FLASH_ErasePage(dest + 0x400 * i);

		IWDG_ReloadCounter();
	}

	FLASH_Lock();
}

void Flash_Write_Ready(void)
{
	FLASH_Unlock();

	FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
}

void Flash_Write_Done(void)
{
	FLASH_Lock();
}

void Flash_Write(uint32 dest, uint8* data, uint32 len)
{
	uint16 temp;
	uint32 i, totallen;

	//Check last byte
	if ((len % 2) != 0)
		totallen = len - 1;
	else
		totallen = len;

	IWDG_ReloadCounter();

	for (i = 0, temp = 0x00; i < totallen; i += 2)
	{
		temp = data[i + 1];
		temp = (temp << 8) + data[i];

		FLASH_ProgramHalfWord(dest + i, temp);
	}

	IWDG_ReloadCounter();

	//Check last byte
	if ((len % 2) != 0)
	{
		temp = 0xFF;
		temp = (temp << 8) + data[len - 1];

		FLASH_ProgramHalfWord(dest + totallen, temp);
	}
}
