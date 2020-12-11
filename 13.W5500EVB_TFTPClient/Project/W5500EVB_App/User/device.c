#include "device.h"
#include "ult.h"
#include "config.h"
#include <stdio.h> 
#include <string.h>

CONFIG_MSG  ConfigMsg, RecvMsg;

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA |RCC_APB2Periph_GPIOB |RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO , ENABLE);
  // Port A output
  GPIO_InitStructure.GPIO_Pin =GPIO_Pin_0|GPIO_Pin_1| GPIO_Pin_2 |GPIO_Pin_3; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
   /* Enable the TIM2 global Interrupt */ 
   //Usart1 NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0X6000);  //only app, no boot included
}

void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

void write_config_to_eeprom(void)
{
  uint8 i, j;
  uint32 dAddr, data;  
  FLASH_Unlock();  // Unlock the Flash Program Erase controller
  // Clear All pending flags
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  FLASH_ErasePage(ConfigAddr);  // Erase the FLASH pages   
  dAddr = ConfigAddr;
  for (i = 0, j = 0; i < (uint8)(sizeof(ConfigMsg)/4);i++) 
  {
    data = *(u32 *)(ConfigMsg.mac+j);
    FLASH_ProgramWord(dAddr, data);
    dAddr += 4;
    j +=4;
  }
}

void get_config(void)
{
  uint16 i;
  
  uint32 addr = ConfigAddr;
  for (i =0; i < CONFIG_MSG_SIZE; i++) 
  {
    *(ConfigMsg.mac+i) = *(vu8*)addr;
    addr += 1;
  }
//  // F/W Version is fixed in the code (netconfig.h)
//  ConfigMsg.sw_ver[1] = FW_VER_LOW;
//  ConfigMsg.sw_ver[0] = FW_VER_HIGH;
//  //save to flash
//  write_config_to_eeprom();
}
