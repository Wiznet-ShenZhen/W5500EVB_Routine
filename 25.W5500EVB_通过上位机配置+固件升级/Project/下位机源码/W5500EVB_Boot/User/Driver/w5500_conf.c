/*
**************************************************************************************************
* @file    		w5500_conf.c
* @author  		WisIOE Software Team
* @version 		V1.1
* @date    		2017-08-01
* @brief  		����MCU����ֲW5500������Ҫ�޸ĵ��ļ�������W5500��MAC��IP��ַ
**************************************************************************************************
* @company  		��������Ƽ����޹�˾
* @website  		www.wisioe.com
* @forum        www.w5500.cn
* @qqGroup      383035001
**************************************************************************************************
*/
#include <stdio.h> 
#include <string.h>

#include "w5500_conf.h"
#include "bsp_i2c_ee.h"
#include "bsp_spi_flash.h"
#include "utility.h"
#include "w5500.h"
#include "dhcp.h"
#include "bsp_TiMbase.h"

CONFIG_MSG  ConfigMsg,RecvMsg;																	/*���ýṹ��*/
EEPROM_MSG_STR EEPROM_MSG;															/*EEPROM�洢��Ϣ�ṹ��*/

/*����MAC��ַ,������W5500�����������ͬһ�ֳ���������ʹ�ò�ͬ��MAC��ַ*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*����Ĭ��IP��Ϣ*/
uint8 local_ip[4]={192,168,1,120};												/*����W5500Ĭ��IP��ַ*/
uint8 subnet[4]={255,255,255,0};												/*����W5500Ĭ����������*/
uint8 gateway[4]={192,168,1,1};													/*����W5500Ĭ������*/
uint8 dns_server[4]={114,114,114,114};									/*����W5500Ĭ��DNS*/

uint16 local_port=8080;	                       					/*���屾�ض˿�*/

/*����Զ��IP��Ϣ*/
uint8  remote_ip[4]={192,168,1,111};										/*Զ��IP��ַ*/
uint16 remote_port=8080;																/*Զ�˶˿ں�*/

/*IP���÷���ѡ��������ѡ��*/
uint8	ip_from=IP_FROM_DHCP;				

uint8 dhcp_ok=0;																				/*dhcp�ɹ���ȡIP*/
uint32	ms=0;																						/*�������*/
uint32	dhcp_time= 0;																		/*DHCP���м���*/
vu8	ntptimer = 0;																				/*NPT�����*/

/**
*@brief		����W5500��IP��ַ
*@param		��
*@return	��
*/
void set_w5500_ip(void)
{	
		
 /*���ƶ����������Ϣ�����ýṹ��*/
	memcpy(ConfigMsg.mac, mac, 6);
	memcpy(ConfigMsg.lip,local_ip,4);
	memcpy(ConfigMsg.sub,subnet,4);
	memcpy(ConfigMsg.gw,gateway,4);
	memcpy(ConfigMsg.dns,dns_server,4);
	if(ip_from==IP_FROM_DEFINE)	
		printf(" ʹ�ö����IP��Ϣ����W5500\r\n");
	
	/*ʹ��EEPROM�洢��IP����*/	
	if(ip_from==IP_FROM_EEPROM)
	{
		/*��EEPROM�ж�ȡIP������Ϣ*/
		read_config_from_eeprom();		
		
		/*�����ȡEEPROM��MAC��Ϣ,��������ã����ʹ��*/		
		if( *(EEPROM_MSG.mac)==0x00&& *(EEPROM_MSG.mac+1)==0x08&&*(EEPROM_MSG.mac+2)==0xdc)		
		{
			printf(" IP from EEPROM\r\n");
			/*����EEPROM������Ϣ�����õĽṹ�����*/
			memcpy(ConfigMsg.lip,EEPROM_MSG.lip, 4);				
			memcpy(ConfigMsg.sub,EEPROM_MSG.sub, 4);
			memcpy(ConfigMsg.gw, EEPROM_MSG.gw, 4);
		}
		else
		{
			printf(" EEPROMδ����,ʹ�ö����IP��Ϣ����W5500,��д��EEPROM\r\n");
			write_config_to_eeprom();	/*ʹ��Ĭ�ϵ�IP��Ϣ������ʼ��EEPROM������*/
		}			
	}

	/*ʹ��DHCP��ȡIP�����������DHCP�Ӻ���*/		
	if(ip_from==IP_FROM_DHCP)								
	{
		/*����DHCP��ȡ��������Ϣ�����ýṹ��*/
		if(dhcp_ok==1)
		{
			printf(" IP from DHCP\r\n");		 
			memcpy(ConfigMsg.lip,DHCP_GET.lip, 4);
			memcpy(ConfigMsg.sub,DHCP_GET.sub, 4);
			memcpy(ConfigMsg.gw,DHCP_GET.gw, 4);
			memcpy(ConfigMsg.dns,DHCP_GET.dns,4);
		}
		else
		{
			printf(" DHCP�ӳ���δ����,���߲��ɹ�\r\n");
			printf(" ʹ�ö����IP��Ϣ����W5500\r\n");
		}
	}
		
	/*����������Ϣ��������Ҫѡ��*/	
	ConfigMsg.sw_ver[0]=FW_VER_HIGH;
	ConfigMsg.sw_ver[1]=FW_VER_LOW;	

	/*��IP������Ϣд��W5500��Ӧ�Ĵ���*/	
	setSUBR(ConfigMsg.sub);
	setGAR(ConfigMsg.gw);
	setSIPR(ConfigMsg.lip);
	
	getSIPR (local_ip);			
	printf(" W5500 IP��ַ   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	printf(" W5500 �������� : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	printf(" W5500 ����     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		����W5500��MAC��ַ
*@param		��
*@return	��
*/
void set_w5500_mac(void)
{
	memcpy(ConfigMsg.mac, mac, 6);
	setSHAR(ConfigMsg.mac);	/**/
	memcpy(DHCP_GET.mac, mac, 6);
}

/**
*@brief		����W5500��GPIO�ӿ�
*@param		��
*@return	��
*/
void gpio_for_w5500_config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_FLASH_Init();																		     /*��ʼ��STM32 SPI�ӿ�*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOG, ENABLE);
		
    	/*ʹ��SPI1*/
    #ifdef STM32_SPI1
			/*����RESET����*/	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz*/		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*��������ģʽΪͨ���������*/		
			GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
			GPIO_SetBits(GPIOC, GPIO_Pin_0);		
			/*����INT����*/	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz */		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*��������ģʽΪͨ������ģ����������*/	
			GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
		#endif
  
	    /*ʹ��SPI2*/
	  #ifdef  STM32_SPI2
			/*����RESET����*/	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz*/		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;		 /*��������ģʽΪͨ���������*/		
			GPIO_Init(GPIOB, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
			GPIO_SetBits(GPIOB, GPIO_Pin_9);		
			/*����INT����*/	
			GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;						 /*ѡ��Ҫ���Ƶ�GPIO����*/		 
			GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 /*������������Ϊ50MHz */		
			GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;				 /*��������ģʽΪͨ������ģ����������*/	
			GPIO_Init(GPIOC, &GPIO_InitStructure);							 /*���ÿ⺯������ʼ��GPIO*/
		#endif

}

/**
*@brief		W5500Ƭѡ�ź����ú���
*@param		val: Ϊ��0����ʾƬѡ�˿�Ϊ�ͣ�Ϊ��1����ʾƬѡ�˿�Ϊ��
*@return	��
*/
void wiz_cs(uint8_t val)
{
	#ifdef  STM32_SPI1
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_4); 
		}
	#endif
	#ifdef STM32_SPI2
		if (val == LOW) 
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_12); 
		}
		else if (val == HIGH)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_12); 
		}
	#endif
	
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_csoff(void)
{
	wiz_cs(LOW);
}

/**
*@brief		����W5500��Ƭѡ�˿�SCSnΪ��
*@param		��
*@return	��
*/
void iinchip_cson(void)
{	
   wiz_cs(HIGH);
}

/**
*@brief		W5500��λ���ú���
*@param		��
*@return	��
*/
void reset_w5500(void)
{
	#ifdef STM32_SPI1
			GPIO_ResetBits(GPIOC, GPIO_Pin_0);
			delay_us(2);  
			GPIO_SetBits(GPIOC, GPIO_Pin_0);
			delay_ms(1600);	
	#endif
	#ifdef  STM32_SPI2
			GPIO_ResetBits(GPIOB, GPIO_Pin_9);
			delay_us(2);  
			GPIO_SetBits(GPIOB, GPIO_Pin_9);
			delay_ms(1600);
	#endif
}

/**
*@brief		STM32 SPI1��д8λ����
*@param		dat��д���8λ����
*@return	��
*/
uint8  IINCHIP_SpiSendData(uint8 dat)
{
   return(SPI_FLASH_SendByte(dat));
}

/**
*@brief		д��һ��8λ���ݵ�W5500
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data��д���8λ����
*@return	��
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data)
{
   iinchip_csoff();                              		
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);	
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4);  
   IINCHIP_SpiSendData(data);                   
   iinchip_cson();                            
}

/**
*@brief		��W5500����һ��8λ����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   data����д��ĵ�ַ����ȡ����8λ����
*@return	��
*/
uint8 IINCHIP_READ(uint32 addrbsb)
{
   uint8 data = 0;
   iinchip_csoff();                            
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
   data = IINCHIP_SpiSendData(0x00);            
   iinchip_cson();                               
   return data;    
}

/**
*@brief		��W5500д��len�ֽ�����
*@param		addrbsb: д�����ݵĵ�ַ
*@param   buf��д���ַ���
*@param   len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf,uint16 len)
{
   uint16 idx = 0;
   if(len == 0) printf("Unexpected2 length 0\r\n");
   iinchip_csoff();                               
   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
   IINCHIP_SpiSendData( (addrbsb & 0x000000F8) + 4); 
   for(idx = 0; idx < len; idx++)
   {
     IINCHIP_SpiSendData(buf[idx]);
   }
   iinchip_cson();                           
   return len;  
}

/**
*@brief		��W5500����len�ֽ�����
*@param		addrbsb: ��ȡ���ݵĵ�ַ
*@param 	buf����Ŷ�ȡ����
*@param		len���ַ�������
*@return	len�������ַ�������
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf,uint16 len)
{
  uint16 idx = 0;
  if(len == 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
  iinchip_csoff();                                
  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));    
  for(idx = 0; idx < len; idx++)                   
  {
    buf[idx] = IINCHIP_SpiSendData(0x00);
  }
  iinchip_cson();                                  
  return len;
}

/**
*@brief		д������Ϣ��EEPROM
*@param		��
*@return	��
*/
void write_config_to_eeprom(void)
{
	uint16 dAddr=0;
	ee_WriteBytes(ConfigMsg.mac,dAddr,(uint8)EEPROM_MSG_LEN);				
	delay_ms(10);																							
}

/**
*@brief		��EEPROM��������Ϣ
*@param		��
*@return	��
*/
void read_config_from_eeprom(void)
{
	ee_ReadBytes(EEPROM_MSG.mac,0,EEPROM_MSG_LEN);
	delay_us(10);
}

/**
*@brief		STM32��ʱ��2��ʼ��
*@param		��
*@return	��
*/
void timer2_init(void)
{
	TIM2_Configuration();																		/* TIM2 ��ʱ���� */
	TIM2_NVIC_Configuration();															/* ��ʱ�����ж����ȼ� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 , ENABLE);		/* TIM2 ���¿�ʱ�ӣ���ʼ��ʱ */
}

/**
*@brief		dhcp�õ��Ķ�ʱ����ʼ��
*@param		��
*@return	��
*/
void dhcp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		ntp�õ��Ķ�ʱ����ʼ��
*@param		��
*@return	��
*/
void ntp_timer_init(void)
{
			timer2_init();																	
}

/**
*@brief		��ʱ��2�жϺ���
*@param		��
*@return	��
*/
void timer2_isr(void)
{
	ms++;	
  if(ms>=1000)
  {  
    ms=0;
    dhcp_time++;																					/*DHCP��ʱ��1S*/
		#ifndef	__NTP_H__
		ntptimer++;																						/*NTP����ʱ���1S*/
		#endif
  }

}
/**
*@brief		STM32ϵͳ��λ����
*@param		��
*@return	��
*/
void reboot(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  printf(" ϵͳ�����С���\r\n");
  JumpAddress = *(vu32*) (0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  Jump_To_Application();
}

/**
*@brief		��ȡ������Ϣ
*@param		��
*@return	��
*/
  void get_config(void)
{
  uint16 i;
  uint32 addr = ConfigAddr;
  for (i =0; i < sizeof(ConfigMsg)-4; i++) 
  {
    *(ConfigMsg.mac+i) = *(vu8*)addr;
      addr += 1;
  }
	addr = VerAddr;
  for (i =0; i < 4; i++) 
  {
    *(ConfigMsg.op+i) = *(vu8*)addr;
      addr += 1;
  }
}


/**
*@brief		���ƶ����������Ϣ�����ýṹ�岢д��Flah
*@param		��
*@return	��
*/
void set_default(void)
{
  //set default mac
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.lip, local_ip, 4);
  memcpy(ConfigMsg.sub,  subnet, 4);
  memcpy(ConfigMsg.gw,  gateway, 4);
  memcpy(ConfigMsg.dns,dns_server,4);
  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.debug = 1;
  ConfigMsg.op[0] = '1';
	ConfigMsg.op[1] = '0';
	ConfigMsg.op[2] = '0';
  write_config_to_flash();
	write_ver_to_flash();
}
/**
*@brief		�������������Ϣ�����ýṹ��д��Flah
*@param		��
*@return	��
*/
void write_config_to_flash(void)//flash
{
  uint8 i, j;
  uint32 dAddr, data;
   
  // Unlock the Flash Program Erase controller
  FLASH_Unlock();
  // Clear All pending flags
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  // Erase the FLASH pages 
  FLASH_ErasePage(ConfigAddr);
   
  dAddr = ConfigAddr;

  for (i = 0, j = 0; i < (uint8)(sizeof(ConfigMsg)/4);i++) 
  {
    data = *(u32 *)(ConfigMsg.mac+j);
    FLASH_ProgramWord(dAddr, data);
    dAddr += 4;
    j +=4;
  }
  FLASH_Lock();
}
/**
*@brief		�汾����Ϣд��
*@param		��
*@return	��
*/
void write_ver_to_flash(void)// ˫��д�� Flash
{
  uint32 dAddr, data;
   
  // Unlock the Flash Program Erase controller
  FLASH_Unlock();
  // Clear All pending flags
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
  // Erase the FLASH pages 
  FLASH_ErasePage(VerAddr);  
  dAddr = VerAddr;
	data = *(u32 *)(ConfigMsg.op);
	FLASH_ProgramWord(dAddr, data);
  FLASH_Lock();
}



/**
*@brief		��IP������Ϣд��W5500��Ӧ�Ĵ����ͳ�ʼ��8��Socket�ķ��ͽ��ջ����С
*@param		��
*@return	��
*/
void set_network(void)
{
  //uint8 ip[4]; 
  setSHAR(ConfigMsg.mac);
  setSUBR(ConfigMsg.sub);
  setGAR(ConfigMsg.gw);
  setSIPR(ConfigMsg.lip);

  //Init. TX & RX Memory size
  socket_buf_init(txsize, rxsize); 
  setRTR(6000);
  setRCR(3);
	
	
	getSIPR (local_ip);			
	//printf(" W5500 IP��ַ   : %d.%d.%d.%d\r\n", local_ip[0],local_ip[1],local_ip[2],local_ip[3]);
	getSUBR(subnet);
	//printf(" W5500 �������� : %d.%d.%d.%d\r\n", subnet[0],subnet[1],subnet[2],subnet[3]);
	getGAR(gateway);
	//printf(" W5500 ����     : %d.%d.%d.%d\r\n", gateway[0],gateway[1],gateway[2],gateway[3]);
}

/**
*@brief		��ת��APPִ��
*@param		��
*@return	��
*/
void reboot_app(void)
{
  pFunction Jump_To_Application;
  uint32 JumpAddress;
  JumpAddress = *(vu32*) (ApplicationAddress + 0x00000004);
  Jump_To_Application = (pFunction) JumpAddress;
  //initialize user application's stack pointer
  __set_MSP(*(__IO uint32_t *)ApplicationAddress);
  Jump_To_Application();
}

/**
*@brief		�����µ�APPд�뵽Appִ������
*@param		��
*@return	��
*/
void copy_app(uint32 fw_len)
{
	vu32 nErasedPage;
	uint32 nPage;
	u32 i;
  if(fw_len>0)
  {
     nPage=FLASH_PagesMask(fw_len);
    //unlock flash
    FLASH_Unlock();
    // Clear All pending flags
    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    //erase required pages
    for(nErasedPage=0; nErasedPage<nPage; nErasedPage++)
    {
      FLASH_ErasePage(ApplicationAddress + 0x400*nErasedPage);
    }
    //copy app from backup address to real address
    for(i=0; i<fw_len; i+=4)
    {
      FLASH_ProgramWord(ApplicationAddress+i, *(uint32*)(AppBackupAddress+i));

    }
    //lock flash again
    FLASH_Lock();
  }
}

uint32 FLASH_PagesMask(vu32 Size)
{
  u32 pagenumber = 0x0;
  u32 size = Size;

  if((size % 0x400) != 0)
  {
    pagenumber = (size / 0x400) + 1;
  }
  else
  {
    pagenumber = size / 0x400;
  }
  return pagenumber;
}
