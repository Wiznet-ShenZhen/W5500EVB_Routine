#ifndef _MBTCP_H_
#define _MBTCP_H_

#include "config.h"
#include "types.h"

extern uint8 mbTCPtid1;
extern uint8 mbTCPtid2;
	
extern volatile uint8 *pucRTUBufferCur;
extern volatile uint16 usRTUBufferPos;
extern volatile uint8 *pucTCPBufferCur;
extern volatile uint16 usTCPBufferPos;


bool MBudp2rtuFrame(void);
bool MBtcp2rtuFrame(void);

void mbTCPtoEVB(void);
void mbEVBtoTCP(void);
#endif


