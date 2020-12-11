#include "stm32f10x.h"
#include "usart.h"
#include "device.h"
#include "ult.h"
#include "config.h"
#include "string.h"
#include <stdio.h>

int main(void)
{
	uint8 i,t;
	Systick_Init(72);/* ��ʼ��Systick����ʱ��*/	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x8000);  //only app, no boot included	
	USART1_Init(); /*��ʼ������ͨ��:115200@8-n-1*/	
  GPIO_Configuration();/* ����GPIO*/
  printf(" STM32 initialization over. \r\n");
	get_config();//read config data from flash 
  printf(" Firmware version is %d.%d .\r\n",ConfigMsg.sw_ver[0],ConfigMsg.sw_ver[1]);	
			
	while(1)
	{
				for(i=0;i<=4;i++)
				{
					GPIO_Write(GPIOA,t);
					t=(t<<1)+1;
					Delay_ms(1000);
				}
				t=0xfe;
				for(i=0;i<=4;i++)
				{
					GPIO_Write(GPIOA,t);
					t=(t<<1);
					Delay_ms(100);
				}
				t=0xfe;		
				GPIO_SetBits(GPIOA, GPIO_Pin_3); // led off
				Delay_ms(100);		
				GPIO_SetBits(GPIOA, GPIO_Pin_2); // led off 
				Delay_ms(100);		
				GPIO_SetBits(GPIOA, GPIO_Pin_1);
				Delay_ms(100);	
				GPIO_SetBits(GPIOA, GPIO_Pin_0);	
				Delay_ms(100);
	}
}

