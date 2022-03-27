#include "config.h"
#include "timer.h"
#include "usart.h"
#include "seg.h"
#include "mcu_init.h"
/* Private variables ---------------------------------------------------------*/
uint32 ms=0;
uint8 sec=0,min=0;
uint8 hour=0;
uint32 mode_ms;
//for seg
uint32 inact=0;//uint: s
uint16 recon=0;//uint: ms
uint16 nagle_time_count=0;
uint16 plus_wait_count=0;//uint: ms
//http timer
uint16 cgi_post_wait_time=0;
uint8 http_time_flag = 0;
uint16 http_time_cnt = 0;//after 5 minutes, the pwd_rxd flag will be reset.
uint8 firmware_update_flag = 0;
uint8 firmware_update_count= 0;
uint8 set_default_count = 0;
uint32 dns_wait_time;
extern uint32  dhcp_time;

void Timer_Configuration(void)
{
	  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);/* 设置TIM2CLK 为 72MHZ */
    TIM_TimeBaseStructure.TIM_Period=1000;
    TIM_TimeBaseStructure.TIM_Prescaler= 71;
	
		/* 对外部时钟进行采样的时钟分频,这里没有用到 */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	  TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}
/**************************/
/*****Timer2 interrupt*****/
/**************************/
void Timer2_ISR(void)
{
  ms++;//mili second
  nagle_time_count++;
  recon++;
	inact++;//不活动时间
	
 
	if(plus_count==3)//出现了3个+	
	{
		plus_wait_count++;	
		if( plus_wait_count>999)//收到+++ 延时1S内无新数据  
		{
			change_to_at=1;//有待改变
		}			
	} 

	if(plus_count==1||plus_count==2)
	{
		if(nagle_time_count>19) plus_count=0;
	}

	if(ms==dhcp_retry_time) dhcp_retry_flag=1;//dhcp重试时间
		

	if(ms%200==0)
	{
		if((connected_flag==0)||(connected_flag&&(ms%1000==0)))
		{
			LED0=!LED0;
		}
		
		if( KEY0 ==0)set_default_count++;
		 else 
		 {
			 if(set_default_count>1 && set_default_count<5) //1s以内有效
			 {
				 reboot_flag=1;
			 }
			 set_default_count=0;
		 }
	}
		
  if(ms==1000)//second
  {     
    sec++;
		dhcp_time++;
    WorkState.on_time++;
		ms=0;
	  LED1=!LED1;
    dns_wait_time++;
		if(firmware_update_flag) firmware_update_count++; //
  }
  if(sec==60)//minute
  {   
		if(http_time_flag) http_time_cnt++; //http timeout counter

		min++;
		sec=0;
  }
  if(min==60)//hour
  {
    min=0;
		hour++;
  }
  if(hour>=24)//day
  {
    hour=0;
  }
}

