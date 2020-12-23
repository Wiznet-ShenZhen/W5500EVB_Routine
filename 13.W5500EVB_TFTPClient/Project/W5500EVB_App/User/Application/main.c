#include "include.h"

int main(void)
{
  /***** MCU时钟初始化 *****/				  
	Systick_Init(72);
	
	/***** 配置嵌套中断向量 *****/	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x8000);  //only app, no boot included	

	/***** GPIO初始化 *****/	
  GPIO_Configuration();
	
	/***** 串口初始化 *****/	
	USART1_Init(); 

  printf(" W5500EVB initialization over. \r\n");
	get_config();//read config data from flash 
  printf(" Firmware version is %d.%d .\r\n",ConfigMsg.sw_ver[0],ConfigMsg.sw_ver[1]);	
			
	while(1)
	{
		LED();
	}
}

