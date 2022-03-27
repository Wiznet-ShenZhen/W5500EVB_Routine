#ifndef _ADC_H_
#define _ADC_H_
#include "stm32f10x.h"
#include "Types.h"

extern uint16 ADC_RCVTab[40];
void ADC_Configuration(void);
void DMA_Configuration(void);

uint16 average(uint16 ADCDataTab[], uint16 nChannel);
#endif 

