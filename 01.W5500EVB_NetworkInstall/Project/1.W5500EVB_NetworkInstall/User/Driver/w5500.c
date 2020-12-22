#include <stdio.h>
#include <string.h>
#include "config.h"
#include "SPI2.h"
#include "w5500.h"
#include "socket.h"

#ifdef __DEF_IINCHIP_PPP__
   #include "md5.h"
#endif

static uint8 I_STATUS[MAX_SOCK_NUM];
static uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */

uint8 getISR(uint8 s)
{
  return I_STATUS[s];
}
void putISR(uint8 s, uint8 val)
{
   I_STATUS[s] = val;
}

uint16 getIINCHIP_RxMAX(uint8 s)
{
   return RSIZE[s];
}
uint16 getIINCHIP_TxMAX(uint8 s)
{
   return SSIZE[s];
}
void IINCHIP_CSoff(void)
{
  WIZ_CS(LOW);
}
void IINCHIP_CSon(void)
{
   WIZ_CS(HIGH);
}
u8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI2_SendByte(dat));
}

void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)			// MCU将通过SPI获取的数据写入自身的接收缓存
{
   IINCHIP_CSoff();                              			// CS=0, SPI数据帧开始
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	// 地址段
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);	// 控制段
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  // 控制段
   IINCHIP_SpiSendData(data);                    			// 数据段，写入数据值
   IINCHIP_CSon();                               			// CS=1, SPI数据帧结束
}

uint8 IINCHIP_READ(uint32 addrbsb)										// MCU将通过SPI获取的数据读取并返回数据值
{
   uint8 data = 0;																		// 定义读取的数据值
   IINCHIP_ISR_DISABLE();                        			// 产生中断
   IINCHIP_CSoff();                              			// CS=0, SPI数据帧开始
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	// 16位地址段
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);	// 8位控制段
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;	// 控制段
   data = IINCHIP_SpiSendData(0x00);             			// MCU将通过SPI收到的数据存放在data中 
   IINCHIP_CSon();                               			// CS=1, SPI数据帧结束
   return data;    																		// 返回读取的数据值
}

uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)	// W5500将通过SPI获取的数据写入相关寄存器，并返回写入的数据长度
{
   uint16 idx = 0;																			// idx定义为正在写入的第几个数
   if(len == 0) printf("Unexpected2 length 0\r\n");			// 写入数据为空；len表示写入数据的长度
   IINCHIP_CSoff();                              				// CS=0, SPI数据帧开始
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);		// 地址段，提供16位偏移地址（0000 0000 0000 0000）
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);		// 控制段，共8位（0000 0000 高5位BSB位为00000表示通用寄存器）
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);    // 控制段+4(0000 0100 RWB位置1表示写入，OM位为00表示SPI工作模式为VDM)
   for(idx = 0; idx < len; idx++)                				// 数据段，写入数据值
   {
     IINCHIP_SpiSendData(buf[idx]);											// MCU通过SPI发送数据
   }
   IINCHIP_CSon();                               				// CS=1, SPI数据帧结束
   return len;  																				// 返回写入的数据长度值
}

uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)	// W5500将通过SPI获取的数据读出，并返回读取的数据长度
{
  uint16 idx = 0;																			// idx定义为正在读取的第几个数
  if(len == 0)																				// len定义为读取数据的长度
  {	
    printf("Unexpected2 length 0\r\n");								// 读取数据长度为0
  }
  IINCHIP_CSoff();                                  	// CS=0, SPI数据帧开始
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);		// 地址段
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);		// 控制段
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    		// 控制段
  for(idx = 0; idx < len; idx++)                    	// 数据段，读取数据值
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);							// 将MCU通过SPI发送过来的数据存放在buf数组中
  }
  IINCHIP_CSon();                                   	// CS=1, SPI数据帧结束 
  return len;																					// 返回读取的数据长度值
}


/**
@brief  This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/
void iinchip_init(void)
{
  setMR( MR_RST );
#ifdef __DEF_IINCHIP_DBG__
  printf("MR value is %02x \r\n",IINCHIP_READ_COMMON(MR));
#endif
}

/**
@brief  This function set the transmit & receive buffer size as per the channels is used
Note for TMSR and RMSR bits are as follows\n
bit 1-0 : memory size of channel #0 \n
bit 3-2 : memory size of channel #1 \n
bit 5-4 : memory size of channel #2 \n
bit 7-6 : memory size of channel #3 \n
bit 9-8 : memory size of channel #4 \n
bit 11-10 : memory size of channel #5 \n
bit 12-12 : memory size of channel #6 \n
bit 15-14 : memory size of channel #7 \n
W5500的Tx, Rx的最大寄存器宽度是16K Bytes,\n
In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
and to data transmission and receiption from non-allocated channel may cause some problems.\n
If the 16KBytes memory is already  assigned to centain channel, \n
other 3 channels couldn't be used, for there's no available memory.\n
If two 4KBytes memory are assigned to two each channels, \n
other 2 channels couldn't be used, for there's no available memory.\n
*/
void sysinit( uint8 * tx_size, uint8 * rx_size  )
{
  int16 i;
  int16 ssum,rsum;
#ifdef __DEF_IINCHIP_DBG__
  printf("sysinit()\r\n");
#endif
  ssum = 0;
  rsum = 0;

  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
  {
          IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i]);
          IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i]);
          
#ifdef __DEF_IINCHIP_DBG__
         printf("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i)));
         printf("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
#endif
    SSIZE[i] = (int16)(0);
    RSIZE[i] = (int16)(0);


    if (ssum <= 16384)
    {
         switch( tx_size[i] )
      {
      case 1:
        SSIZE[i] = (int16)(1024);
        break;
      case 2:
        SSIZE[i] = (int16)(2048);
        break;
      case 4:
        SSIZE[i] = (int16)(4096);
        break;
      case 8:
        SSIZE[i] = (int16)(8192);
        break;
      case 16:
        SSIZE[i] = (int16)(16384);
      break;
      default :
        RSIZE[i] = (int16)(2048);
        break;
      }
    }

   if (rsum <= 16384)
    {
         switch( rx_size[i] )
      {
      case 1:
        RSIZE[i] = (int16)(1024);
        break;
      case 2:
        RSIZE[i] = (int16)(2048);
        break;
      case 4:
        RSIZE[i] = (int16)(4096);
        break;
      case 8:
        RSIZE[i] = (int16)(8192);
        break;
      case 16:
        RSIZE[i] = (int16)(16384);
        break;
      default :
        RSIZE[i] = (int16)(2048);
        break;
      }
    }
    ssum += SSIZE[i];
    rsum += RSIZE[i];

  }
}

// added

/*.
*/
void setGAR(
  uint8 * addr  /**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
  )
{
    wiz_write_buf(GAR0, addr, 4);
}
void getGWIP(uint8 * addr)
{
    wiz_read_buf(GAR0, addr, 4);
}

/**
@brief  It sets up SubnetMask address
*/
void setSUBR(uint8 * addr)
{   
    wiz_write_buf(SUBR0, addr, 4);
}
/**
@brief  This function sets up MAC address.
*/
void setSHAR(
  uint8 * addr  /**< a pointer to a 6 -byte array responsible to set the MAC address. */
  )
{
  wiz_write_buf(SHAR0, addr, 6);  
}

/**
@brief  This function sets up Source IP address.
*/
void setSIPR(
  uint8 * addr  /**< a pointer to a 4 -byte array responsible to set the Source IP address. */
  )
{
    wiz_write_buf(SIPR0, addr, 4);  
}

/**
@brief  This function sets up Source IP address.
*/
void getGAR(uint8 * addr)
{
    wiz_read_buf(GAR0, addr, 4);
}
void getSUBR(uint8 * addr)
{
    wiz_read_buf(SUBR0, addr, 4);
}
void getSHAR(uint8 * addr)
{
    wiz_read_buf(SHAR0, addr, 6);
}
void getSIPR(uint8 * addr)
{
    wiz_read_buf(SIPR0, addr, 4);
}

void setMR(uint8 val)
{
  IINCHIP_WRITE(MR,val);
}

/**
@brief  This function gets Interrupt register in common register.
 */
uint8 getIR( void )
{
   return IINCHIP_READ(IR);
}

/**
@brief  This function sets up Retransmission time.

If there is no response from the peer or delay in response then retransmission
will be there as per RTR (Retry Time-value Register)setting
*/
void setRTR(uint16 timeout)
{
  IINCHIP_WRITE(RTR0,(uint8)((timeout & 0xff00) >> 8));
  IINCHIP_WRITE(RTR1,(uint8)(timeout & 0x00ff));
}

/**
@brief  This function set the number of Retransmission.

If there is no response from the peer or delay in response then recorded time
as per RTR & RCR register seeting then time out will occur.
*/
void setRCR(uint8 retry)
{
  IINCHIP_WRITE(WIZ_RCR,retry);
}

/**
@brief  This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void clearIR(uint8 mask)
{
  IINCHIP_WRITE(IR, ~mask | getIR() ); // must be setted 0x10.
}

/**
@brief  This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16 Sn_MSSR)
{
  IINCHIP_WRITE( Sn_MSSR0(s), (uint8)((Sn_MSSR & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_MSSR1(s), (uint8)(Sn_MSSR & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8 ttl)
{    
   IINCHIP_WRITE( Sn_TTL(s) , ttl);
}



/**
@brief  get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8 getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}


/**
@brief   get socket status
*/
uint8 getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief  get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_TX_FSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
      if (val1 != 0)
    {
        val = IINCHIP_READ(Sn_TX_FSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
    }
  } while (val != val1);
   return val;
}


/**
@brief   get socket RX recv buf size

This gives size of received data in receive buffer.
*/
uint16 getSn_RX_RSR(SOCKET s)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_RX_RSR0(s));
    val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
    if(val1 != 0)
    {
        val = IINCHIP_READ(Sn_RX_RSR0(s));
        val = (val << 8) + IINCHIP_READ(Sn_RX_RSR1(s));
    }
  } while (val != val1);
   return val;
}


/**
@brief   This function is being called by send() and sendto() function also.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr =0;
  uint32 addrbsb =0;
  if(len == 0)
  {
    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }

 
  ptr = IINCHIP_READ( Sn_TX_WR0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
  wiz_write_buf(addrbsb, data, len);
  
  ptr += len;
  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));
}


/**
@brief  This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
  uint16 ptr = 0;
  uint32 addrbsb = 0;
  
  if(len == 0)
  {
    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ( Sn_RX_RD0(s) );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s) );

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
  wiz_read_buf(addrbsb, data, len);
  ptr += len;

  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
}

void setSn_IR(uint8 s, uint8 val)
{
    IINCHIP_WRITE(Sn_IR(s), val);
}




