#include "types.h"
#include "mbascii.h"
#include "mbtcp.h"
//#include "timer.h"

volatile uint8 mb_state_finish;
extern uint8 plus_count;


#define MB_SER_PDU_SIZE_MIN     3       /*!< Minimum size of a Modbus ASCII frame. */
#define MB_SER_PDU_SIZE_MAX     256     /*!< Maximum size of a Modbus ASCII frame. */
#define MB_SER_PDU_SIZE_LRC     1       /*!< Size of LRC field in PDU. */
#define MB_SER_PDU_ADDR_OFF     0       /*!< Offset of slave address in Ser-PDU. */
#define MB_SER_PDU_PDU_OFF      1       /*!< Offset of Modbus-PDU in Ser-PDU. */
typedef enum
{
    STATE_RX_IDLE,              /*!< Receiver is in idle state. */
    STATE_RX_RCV,               /*!< Frame is beeing received. */
	  STATE_RX_END, 
    STATE_RX_ERROR              /*!< If the frame is invalid. */
} eMBRcvState;



extern volatile uint8* pucASCIIBufferCur;
extern volatile uint16 usASCIIBufferPos;

extern volatile uint8 *pucTCPBufferCur;
extern volatile uint16 usTCPBufferPos;

volatile uint8 ucRTUBuf[MB_SER_PDU_SIZE_MAX];
volatile uint8 *ucASCIIBuf = ucRTUBuf;

//static eMBRcvState eRcvState;

uint8 prvucMBCHAR2BIN( uint8 ucCharacter )
{
    if( ( ucCharacter >= '0' ) && ( ucCharacter <= '9' ) )
    {
        return ( uint8 )( ucCharacter - '0' );
    }
    else if( ( ucCharacter >= 'A' ) && ( ucCharacter <= 'F' ) )
    {
        return ( uint8 )( ucCharacter - 'A' + 0x0A );
    }
    else
    {
        return 0xFF;
    }
}


uint8 prvucMBBIN2CHAR( uint8 ucByte )
{
    if( ucByte <= 0x09 )
    {
        return ( uint8 )( '0' + ucByte );
    }
    else if( ( ucByte >= 0x0A ) && ( ucByte <= 0x0F ) )
    {
        return ( uint8 )( ucByte - 0x0A + 'A' );
    }
    else
    {
        /* Programming error. */
//        assert( 0 );
    }
    return '0';
}


uint8 prvucMBLRC( uint8 * pucFrame, uint16 usLen )
{
    uint8           ucLRC = 0;  /* LRC char initialized */

    while( usLen-- )
    {
        ucLRC += *pucFrame++;   /* Add buffer byte without carry */
    }

    /* Return twos complement */
    ucLRC = ( uint8 ) ( -( ( int8 ) ucLRC ) );
		
    return ucLRC;
}

static bool mbEVBPackage( uint8 * pucRcvAddress, uint8 ** pucFrame, uint16 * pusLength )
{
	uint16 count;
	for(count=0; count < usASCIIBufferPos; count++)
	{
		if(count%2)
		{
			pucASCIIBufferCur[(uint16)(count/2)] |= prvucMBCHAR2BIN(ucASCIIBuf[count]);
		}
		else
	  {
			pucASCIIBufferCur[count/2] = (uint8)(prvucMBCHAR2BIN(ucASCIIBuf[count])<<4);
		}
	}
	if(prvucMBLRC( ( uint8 * ) pucASCIIBufferCur, usASCIIBufferPos/2 ) == 0)
  {
	  *pucRcvAddress = pucASCIIBufferCur[MB_SER_PDU_ADDR_OFF];
	  *pusLength = ( uint16 )( usASCIIBufferPos/2 - MB_SER_PDU_PDU_OFF - MB_SER_PDU_SIZE_LRC );
	  *pucFrame = ( uint8 * ) & pucASCIIBufferCur[MB_SER_PDU_PDU_OFF];
		return TRUE;
	}
	
	return FALSE;
}


bool MBevb2tcpFrame(void)
{
	uint8 pucRcvAddress;
	uint16 pusLength;
	uint8* ppucFrame;
	
	if(mbEVBPackage( &pucRcvAddress, &ppucFrame, &pusLength ) != FALSE)
  {
		/*组合成MODBUS TCP包*/
		pucTCPBufferCur = ppucFrame-7;		
		
		pucTCPBufferCur[0] = mbTCPtid1;
		pucTCPBufferCur[1] = mbTCPtid2;
		
		pucTCPBufferCur[2] = 0;
		pucTCPBufferCur[3] = 0;
		
		pucTCPBufferCur[4] = ( pusLength + 1 ) >> 8U;
    pucTCPBufferCur[5] = ( pusLength + 1 ) & 0xFF;
		
		pucTCPBufferCur[6] = pucRcvAddress;
		
		usTCPBufferPos = pusLength + 7;
		return TRUE;
	}
	return FALSE;
}

