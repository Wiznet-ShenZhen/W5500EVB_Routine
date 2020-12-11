#ifndef _MBASCII_H
#define _MBASCII_H

#include "config.h"

#define MB_ASCII_START          ':'
#define MB_ASCII_DEFAULT_CR     '\r'    /*!< Default CR character for Modbus ASCII. */
#define MB_ASCII_DEFAULT_LF     '\n'    /*!< Default LF character for Modbus ASCII. */


extern volatile uint8 mb_state_finish;

bool MBtcp2evbFrame(void);
bool MBevb2tcpFrame(void);
uint8 prvucMBLRC( uint8 * pucFrame, uint16 usLen );
uint8 prvucMBCHAR2BIN( uint8 ucCharacter );
uint8 prvucMBBIN2CHAR( uint8 ucByte );
void ASCII_Uart_RX(void);
void do_Modbus(void);
#endif

