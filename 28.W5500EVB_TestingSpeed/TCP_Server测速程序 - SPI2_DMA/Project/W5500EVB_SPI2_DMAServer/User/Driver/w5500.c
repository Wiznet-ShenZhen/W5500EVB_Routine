#include <stdio.h>
#include <string.h>
#include "config.h"
#include "SPI2.h"
#include "w5500.h"
#include "socket.h"
#include "device.h"
#include "stm32f10x.h"
#ifdef __DEF_IINCHIP_PPP__
   #include "md5.h"
#endif
DMA_InitTypeDef DMA_InitStructure;

#define DATA_BUF_SIZE    16384

static uint8 I_STATUS[MAX_SOCK_NUM];
static uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */

uint8 SPI2_DMA_TxBuff[DATA_BUF_SIZE+3];

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

void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
	uint8_t spi_data[4];
	
	#if !defined (SPI_DMA)
   //IINCHIP_ISR_DISABLE();                        // Interrupt Service Routine Disable
   IINCHIP_CSoff();                              // CS=0, SPI start
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);// Address byte 1
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);// Address byte 2
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);    // Data write command and Write data length 1
   IINCHIP_SpiSendData(data);                    // Data write (write 1byte data)
   IINCHIP_CSon();                               // CS=1,  SPI end
   //IINCHIP_ISR_ENABLE();                         // Interrupt Service Routine Enable
	
	#else
	 spi_data[0] = (addrbsb & 0x00FF0000) >> 16;
	 spi_data[1] = (addrbsb & 0x0000FF00) >> 8;
	 spi_data[2] = ((addrbsb & 0x000000F8) + 4);
	 SPI_DMA_WRITE(spi_data, &data, 1);//写数据
#endif
}

uint8 IINCHIP_READ(uint32 addrbsb)
{
	 uint8 data = 0;
	 uint8_t spi_data[3];
	
	#if !defined (SPI_DMA)   
   //IINCHIP_ISR_DISABLE();                        // Interrupt Service Routine Disable
   IINCHIP_CSoff();                              // CS=0, SPI start
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);// Address byte 1
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);// Address byte 2
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;// Data read command and Read data length 1
   data = IINCHIP_SpiSendData(0x00);             // Data read (read 1byte data)
   IINCHIP_CSon();                               // CS=1,  SPI end
   //IINCHIP_ISR_ENABLE();                         // Interrupt Service Routine Enable     
  #else
	 spi_data[0] = (addrbsb & 0x00FF0000) >> 16;
	 spi_data[1] = (addrbsb & 0x0000FF00) >> 8;
	 spi_data[2] = ((addrbsb & 0x000000F8));
	 SPI_DMA_READ(spi_data, &data, 1);//读数据
	#endif
	 return data; 
}

uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
//   uint16 idx = 0;
	 uint8_t spi_data[3];
#if !defined (SPI_DMA)  
	SPI_Cmd(SPI1, ENABLE);
   if(len == 0) printf("Unexpected2 length 0\r\n");

   //IINCHIP_ISR_DISABLE();
   IINCHIP_CSoff();                              // CS=0, SPI start
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);// Address byte 1
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);// Address byte 2
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);    // Data write command and Write data length 1
   for(idx = 0; idx < len; idx++)                // Write data in loop
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   IINCHIP_CSon();                               // CS=1, SPI end
   //IINCHIP_ISR_ENABLE();                         // Interrupt Service Routine Enable    
    
#else 
	 spi_data[0] = ((addrbsb & 0x00FF0000) >> 16);
	 spi_data[1] = ((addrbsb & 0x0000FF00) >> 8);
	 spi_data[2] = ((addrbsb & 0x000000F8) + 4);
	 SPI_DMA_WRITE(spi_data, buf, len);//写数据
#endif	 
	 return len; 
}

uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
//  uint16 idx = 0;
	uint8_t spi_data[3];
#if !defined (SPI_DMA)  
	SPI_Cmd(SPI1, ENABLE);
  if(len == 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
  //IINCHIP_ISR_DISABLE();
  //SPI MODE I/F
  IINCHIP_CSoff();                                  // CS=0, SPI start
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);// Address byte 1
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);// Address byte 2
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    // Data write command and Write data length 1
  for(idx = 0; idx < len; idx++)                    // Write data in loop
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  IINCHIP_CSon();                                   // CS=1, SPI end
  //IINCHIP_ISR_ENABLE();                             // Interrupt Service Routine Enable
 
#else
	spi_data[0] = (addrbsb & 0x00FF0000) >> 16;
	spi_data[1] = (addrbsb & 0x0000FF00) >> 8;
	spi_data[2] = (addrbsb & 0x000000F8);
	SPI_DMA_READ(spi_data, buf, len);//读数据
#endif
	return len;
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
          IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i]);		// MCU记录每个Socket发送缓存的大小到数组tx_size[]对应的元素值
          IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i]);		// MCU记录每个Socket接收缓存的大小到数组rx_size[]对应的元素值
          
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
@brief  W5500心跳检测程序，设置Socket在线时间寄存器Sn_KPALVTR，单位为5s
*/
void setkeepalive(SOCKET s)
{ 
  IINCHIP_WRITE(Sn_KPALVTR(s),0x02);
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
   return IINCHIP_READ(Sn_SR(s)); // MCU读Sn_SR对应地址里面的数值并返回
}


/**
@brief  get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
  uint16 val=0,val1=0;
	uint8 buff[2];
  do
  {
//    val1 = IINCHIP_READ(Sn_TX_FSR0(s));
//    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
		wiz_read_buf(Sn_TX_FSR0(s),buff,2);
		val1=(buff[0]<<8)+buff[1];
      if (val1 != 0)
    {
//        val = IINCHIP_READ(Sn_TX_FSR0(s));
//        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
			wiz_read_buf(Sn_TX_FSR0(s),buff,2);
		  val=(buff[0]<<8)+buff[1];
    }
  } while (val != val1);
   return val;
}


/**
@brief   get socket RX recv buf size

This gives size of received data in receive buffer.
*/
uint16 getSn_RX_RSR(SOCKET s)		// 获取Sn_RX_RSR空闲接收缓存寄存器的值并返回,Sn_RX_RSR中保存的是接收缓存中已接收和保存的数据大小
{
  uint16 val=0,val1=0;
	uint8 buff[2];
  do
  {
//    val1 = IINCHIP_READ(Sn_RX_RSR0(s));		
//    val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR1(s));		// MCU读出Sn_RX_RSR的值赋给val1
		wiz_read_buf(Sn_RX_RSR0(s),buff,2);
		val1=(buff[0]<<8)+buff[1];
    if(val1 != 0)																				// 如果Sn_RX_RSR的值不为0，说明接收缓存中收到了数据
    {
//        val = IINCHIP_READ(Sn_RX_RSR0(s));				
//        val = (val << 8) + IINCHIP_READ(Sn_RX_RSR1(s));	// MCU读出Sn_RX_RSR的值赋给val
			wiz_read_buf(Sn_RX_RSR0(s),buff,2);
		  val=(buff[0]<<8)+buff[1];
    }
  } 
	while (val != val1);																	// 此时应该val=val1,表达式为假，跳出do while循环
  return val;																						// 返回val
}
/**
@brief   This function is being called by send() and sendto() function also.

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)		// MCU把数据发送给W5500的过程，W5500将数据写入buf，并更新数据的写指针的地址
{
  uint16 ptr =0;
  uint32 addrbsb =0;
	uint8 buff[2];
	
  if(len == 0)
  {
//    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }

// Sn_RX_WR是发送写指针寄存器，用来保存发送缓存中将要传输数据的首地址 	
//	ptr = IINCHIP_READ( Sn_TX_WR0(s) );
//  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));			// MCU读取Sn_RX_WR寄存器的值
//  printf("Sn_TX_WR:0x%4x\r\n",ptr);
	wiz_read_buf(Sn_TX_WR0(s),buff,2);
	ptr=((buff[0] & 0x00ff) << 8 )+buff[1];
	
  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;										// 将发送缓存中将要传输数据的首地址转成32位

  wiz_write_buf(addrbsb, data, len);														// W5500从该首地址开始写入数据，数据长度为len
  
	
  ptr += len;																										// 首地址的值+len变为数据新的首地址
	
	buff[0]=(uint8)((ptr & 0xff00) >> 8);
	buff[1]=(uint8)(ptr & 0x00ff);
	wiz_write_buf(Sn_TX_WR0(s),buff,2);
//  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
//  IINCHIP_WRITE( Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));						// 将新的首地址保存在Sn_RX_WR中

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
  uint8 buff[2];
  if(len == 0)
  {
//    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }
// Sn_RX_RD是接收读指针寄存器
//  ptr = IINCHIP_READ( Sn_RX_RD0(s) );			
//  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s) );			// MCU读取Sn_RX_RD寄存器的值

	wiz_read_buf(Sn_RX_RD0(s),buff,2);
	ptr=((buff[0] & 0x00ff) << 8 )+buff[1];
	
	
  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;					// 将发送缓存中将要传输数据的首地址转成32位
  wiz_read_buf(addrbsb, data, len);										// W5500从该首地址开始读取数据，数据长度为len
  ptr += len;																					// 首地址的值+len变为数据新的首地址
  
	buff[0]=(uint8)((ptr & 0xff00) >> 8);
	buff[1]=(uint8)(ptr & 0x00ff);
	wiz_write_buf(Sn_RX_RD0(s),buff,2);
//  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));		
//  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));		// 将新的首地址保存在Sn_RX_RD中
}

void setSn_IR(uint8 s, uint8 val)
{
    IINCHIP_WRITE(Sn_IR(s), val);
}

void SPI_DMA_WRITE(uint8_t* Addref, uint8_t* pTxBuf, uint16_t tx_len)
{
	
	uint16_t i;
	memset(SPI2_DMA_TxBuff, 0, tx_len + 3);
	
	SPI2_DMA_TxBuff[0] = Addref[0];
	SPI2_DMA_TxBuff[1] = Addref[1];
	SPI2_DMA_TxBuff[2] = Addref[2];

	for(i=0; i<tx_len; i++)
	{
	  SPI2_DMA_TxBuff[3 + i] = pTxBuf[i];		
	}	
	
	DMA_InitStructure.DMA_BufferSize  = (uint16_t)(tx_len + 3);//数据缓存大小
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t ) (&(SPI2->DR));//外设地址
	DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable;//不执行外设地址增量模式
	DMA_InitStructure.DMA_MemoryInc  = DMA_MemoryInc_Enable;//存储器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度8位
	
	DMA_InitStructure.DMA_MemoryDataSize  = DMA_MemoryDataSize_Byte;//存储器数据宽度8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//不执行循环操作
  DMA_InitStructure.DMA_Priority =DMA_Priority_VeryHigh;//优先级--高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//存储器到存储器失能


  /* Configure TX DMA */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;//数据传输方向存储器到外设（从存储器读）
  DMA_InitStructure.DMA_MemoryBaseAddr  = (uint32_t )SPI2_DMA_TxBuff ;//数据存储地址
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  /* Configure RX DMA */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;//数据传输方向外设到存储器（从外设读）
  DMA_InitStructure.DMA_MemoryBaseAddr  = (uint32_t )SPI2_DMA_TxBuff ; //数据存储地址
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
  IINCHIP_CSoff(); 
	
	/* Enable the SPI1 Tx DMA request */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);

  /* Enable the DMA SPI1 TX Stream */
	DMA_Cmd(DMA1_Channel5, ENABLE);
	/* Enable the DMA SPI1 RX Stream */
	DMA_Cmd(DMA1_Channel4, ENABLE);

 	SPI_Cmd(SPI2,ENABLE);
	 
  /* Waiting the end of Data transfer */
  
  while (DMA_GetFlagStatus(DMA1_FLAG_TC4)==RESET);//等待数据传输完成
  while (DMA_GetFlagStatus(DMA1_FLAG_TC5)==RESET);
  
	IINCHIP_CSon(); 
	
	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_ClearFlag(DMA1_FLAG_TC5);
  
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_Cmd(DMA1_Channel5, DISABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);
	
	SPI_Cmd(SPI2, DISABLE);
}

void SPI_DMA_READ(uint8_t* Addref, uint8_t* pRxBuf, uint16_t rx_len)
{
//	uint16_t i;
	
	memset(SPI2_DMA_TxBuff, 0, rx_len + 3);//清发送缓存

	SPI2_DMA_TxBuff[0] = Addref[0];  //获取读地址
	SPI2_DMA_TxBuff[1] = Addref[1];
	SPI2_DMA_TxBuff[2] = Addref[2];

#if 0
	printf("rx len: %d\r\n", rx_len);
	printf("Rx Data: ");
	for(i=0; i<rx_len; i++)
		printf("%02X ", SPI2_DMA_TxBuff[i]);
	printf("\r\n");

	printf("pTmpBuf1: ");
	for(i=0; i<rx_len + 3; i++)
		printf("%02X ", SPI2_DMA_TxBuff[i]);
	printf("\r\n");
#endif
//配置DMA	
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )(&(SPI2->DR));//外设地址
	DMA_InitStructure.DMA_BufferSize  = (uint16_t)(rx_len + 3);//数据缓存大小
	DMA_InitStructure.DMA_PeripheralInc  = DMA_PeripheralInc_Disable;//不执行外设地址增量模式
	DMA_InitStructure.DMA_MemoryInc  = DMA_MemoryInc_Enable;//存储器地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;//外设数据宽度8位
	
	DMA_InitStructure.DMA_MemoryDataSize  = DMA_MemoryDataSize_Byte;//存储器数据宽度8位
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;//不执行循环操作
  DMA_InitStructure.DMA_Priority =DMA_Priority_VeryHigh;//优先级--高
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//存储器到存储器失能


  /* Configure TX DMA */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST ;//数据传输方向存储器到外设（从存储器读）
  DMA_InitStructure.DMA_MemoryBaseAddr  = (uint32_t )SPI2_DMA_TxBuff ;//数据存储地址
  DMA_Init(DMA1_Channel5, &DMA_InitStructure);
  /* Configure RX DMA */
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC ;//数据传输方向外设到存储器（从外设读）
  DMA_InitStructure.DMA_MemoryBaseAddr  = (uint32_t )SPI2_DMA_TxBuff ; //数据存储地址
  DMA_Init(DMA1_Channel4, &DMA_InitStructure);

  IINCHIP_CSoff(); 
	
		/* Enable the SPI Rx DMA request */
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, ENABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, ENABLE); 
	
	/* Enable the DMA SPI TX Stream */
	DMA_Cmd(DMA1_Channel5, ENABLE);
	/* Enable the DMA SPI RX Stream */
	DMA_Cmd(DMA1_Channel4, ENABLE);
  
	SPI_Cmd(SPI2, ENABLE);
	
  /* Waiting the end of Data transfer */
  while (DMA_GetFlagStatus(DMA1_FLAG_TC4)==RESET);
  while (DMA_GetFlagStatus(DMA1_FLAG_TC5)==RESET);

  IINCHIP_CSon(); 
	
	DMA_ClearFlag(DMA1_FLAG_TC4);
	DMA_ClearFlag(DMA1_FLAG_TC5);

	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_Cmd(DMA1_Channel5, DISABLE);

	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Rx, DISABLE);
	SPI_I2S_DMACmd(SPI2, SPI_I2S_DMAReq_Tx, DISABLE);

	SPI_Cmd(SPI2, DISABLE);

	memcpy(pRxBuf, SPI2_DMA_TxBuff + 3, rx_len);
#if 0
	printf("Rx Data: ");
	for(i=0; i<rx_len; i++)
		printf("%02X ", SPI2_DMA_RxBuff[i]);
	printf("\r\n");
#endif
}



