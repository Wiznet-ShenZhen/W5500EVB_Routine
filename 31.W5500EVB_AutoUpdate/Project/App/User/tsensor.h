#ifndef __TSENSOR_H
#define __TSENSOR_H	
#include "stm32f10x.h"
#include "Types.h" 

#define ADC_CH_temp  	ADC_Channel_16 //�¶ȴ�����ͨ��
   
u16  T_get_temperature(void);  //ȡ���¶�ֵ
void temp_adc_Init(void); //ADCͨ����ʼ��
u16  T_Get_Adc(u8 ch); //���ĳ��ͨ��ֵ  
u16  T_Get_Adc_Average(u8 ch,u8 times);//�õ�ĳ��ͨ��10�β�����ƽ��ֵ 
int8 get_temperature(void);
#endif 
