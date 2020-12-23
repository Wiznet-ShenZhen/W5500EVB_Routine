#include "include.h"

int main(void)
{
  /***** MCUʱ�ӳ�ʼ�� *****/				  
	Systick_Init(72);
	
	/***** ����Ƕ���ж����� *****/	
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x8000);  //only app, no boot included	

	/***** GPIO��ʼ�� *****/	
  GPIO_Configuration();
	
	/***** ���ڳ�ʼ�� *****/	
	USART1_Init(); 

  printf(" W5500EVB initialization over. \r\n");
	get_config();//read config data from flash 
  printf(" Firmware version is %d.%d .\r\n",ConfigMsg.sw_ver[0],ConfigMsg.sw_ver[1]);	
			
	while(1)
	{
		LED();
	}
}

