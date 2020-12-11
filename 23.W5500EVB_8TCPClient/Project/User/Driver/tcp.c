#include "tcp.h"

uint8 buffer[2048];												// ¶¨ÒåÒ»¸ö2KBµÄÊı×é£¬ÓÃÀ´´æ·ÅSocketµÄÍ¨ĞÅÊı¾İ
uint8 server_ip1[4]={192,168,1,101};			// ÅäÖÃÔ¶³Ì·şÎñÆ÷IPµØÖ·
uint8 server_ip2[4]={192,168,1,102};			// ÅäÖÃÔ¶³Ì·şÎñÆ÷IPµØÖ·
uint16 server_port1=5000;								// ÅäÖÃÔ¶³Ì·şÎñÆ÷¶Ë¿Ú
uint16 server_port2=6000;								// ÅäÖÃÔ¶³Ì·şÎñÆ÷¶Ë¿Ú
uint16 server_port3=7000;								// ÅäÖÃÔ¶³Ì·şÎñÆ÷¶Ë¿Ú
uint16 server_port4=8000;								// ÅäÖÃÔ¶³Ì·şÎñÆ÷¶Ë¿Ú

uint16 local_port = 5000;												// ¶¨ÒåÒ»¸ö±¾µØ¶Ë¿Ú²¢³õÊ¼»¯
uint16 len = 0;

void tcpc(uint8 i)
{
	switch (getSn_SR(i))														// »ñÈ¡socket0µÄ×´Ì¬
	{
	case SOCK_INIT:															// Socket´¦ÓÚ³õÊ¼»¯Íê³É(´ò¿ª)×´Ì¬
		//printf("Socket%d-1 ", i);
		Delay_ms(500);
		if (i == 0)
		{	connect(i, server_ip1, server_port1);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 1)
		{	connect(i, server_ip1, server_port2);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 2)
		{	connect(i, server_ip1, server_port3);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 3)
		{	connect(i, server_ip1, server_port4);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 4)
		{	connect(i, server_ip2, server_port1);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 5)
		{	connect(i, server_ip2, server_port2);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 6)
		{	connect(i, server_ip2, server_port3);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½ÓÇëÇó
		else if(i == 7)
		{	connect(i, server_ip2, server_port4);	}		// ÅäÖÃSn_CRÎªCONNECT£¬²¢ÏòTCP·şÎñÆ÷·¢³öÁ¬½
		
		break;
		
	case SOCK_ESTABLISHED:											// Socket´¦ÓÚÁ¬½Ó½¨Á¢×´Ì¬
		//printf("Socket%d-2 ", i);
		Delay_ms(500);
		if (getSn_IR(i) & Sn_IR_CON)
		{
			setSn_IR(i, Sn_IR_CON);								// Sn_IRµÄCONÎ»ÖÃ1£¬Í¨ÖªW5500Á¬½ÓÒÑ½¨Á¢
			printf("%d:connected.\r\n", i);
		}
		// Êı¾İ»Ø»·²âÊÔ³ÌĞò£ºÊı¾İ´ÓÉÏÎ»»ú·şÎñÆ÷·¢¸øW5500£¬W5500½ÓÊÕµ½Êı¾İºóÔÙ»Ø¸ø·şÎñÆ÷
		len = getSn_RX_RSR(i);										// len=Socket0½ÓÊÕ»º´æÖĞÒÑ½ÓÊÕºÍ±£´æµÄÊı¾İ´óĞ¡
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500½ÓÊÕÀ´×Ô·şÎñÆ÷µÄÊı¾İ£¬²¢Í¨¹ıSPI·¢ËÍ¸øMCU
			printf("%d:%s\r\n", i, buffer);							// ´®¿Ú´òÓ¡½ÓÊÕµ½µÄÊı¾İ
      send(i,buffer,len);										// ½ÓÊÕµ½Êı¾İºóÔÙ»Ø¸ø·şÎñÆ÷£¬Íê³ÉÊı¾İ»Ø»·
			memset(buffer,0,len+1);
		}
		// W5500´Ó´®¿Ú·¢Êı¾İ¸ø¿Í»§¶Ë³ÌĞò£¬Êı¾İĞèÒÔ»Ø³µ½áÊø
		if (USART_RX_STA & 0x8000)								// ÅĞ¶Ï´®¿ÚÊı¾İÊÇ·ñ½ÓÊÕÍê³É
		{
			len = USART_RX_STA & 0x3fff;						// »ñÈ¡´®¿Ú½ÓÊÕµ½Êı¾İµÄ³¤¶È
			send(i, USART_RX_BUF, len);							// W5500Ïò¿Í»§¶Ë·¢ËÍÊı¾İ
			USART_RX_STA = 0;												// ´®¿Ú½ÓÊÕ×´Ì¬±êÖ¾Î»Çå0
			memset(USART_RX_BUF, 0, len + 1);					// ´®¿Ú½ÓÊÕ»º´æÇå0
		}
		break;
		
	case SOCK_CLOSE_WAIT:												// Socket´¦ÓÚµÈ´ı¹Ø±Õ×´Ì¬
		//printf("Socket%d-3 ", i);
		Delay_ms(500);
		len = getSn_RX_RSR(i);										// len=Socket0½ÓÊÕ»º´æÖĞÒÑ½ÓÊÕºÍ±£´æµÄÊı¾İ´óĞ¡
		if (len > 0)
		{
			recv(i, buffer, len);										// W5500½ÓÊÕÀ´×Ô·şÎñÆ÷µÄÊı¾İ£¬²¢Í¨¹ıSPI·¢ËÍ¸øMCU
			printf("%d:%s\r\n", i, buffer);							// ´®¿Ú´òÓ¡½ÓÊÕµ½µÄÊı¾İ
			send(i, buffer, len);										// ½ÓÊÕµ½Êı¾İºóÔÙ»Ø¸ø·şÎñÆ÷£¬Íê³ÉÊı¾İ»Ø»·
		}
		disconnect(i);																// ¶Ï¿ªSocket0µÄÁ¬½Ó
		break;
		
	case SOCK_CLOSED:														// Socket´¦ÓÚ¹Ø±Õ×´Ì¬
		//printf("Socket%d-4 ", i);
		Delay_ms(500);
		socket(i, Sn_MR_TCP, local_port++, Sn_MR_ND);		// ´ò¿ªSocket0£¬²¢ÅäÖÃÎªTCPÎŞÑÓÊ±Ä£Ê½£¬´ò¿ªÒ»¸ö±¾µØ¶Ë¿Ú
		break;
	}
}

void do_8tcpc(void)
{
	int i = 0;
	for (i = 0; i <= 7; i++)
	{
		tcpc(i);
	}
}
