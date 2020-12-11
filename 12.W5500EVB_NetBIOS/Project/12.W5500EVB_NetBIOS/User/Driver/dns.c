/**
******************************************************************************
* @file    			DNS.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-xx-xx
* @brief   			���������ͻ��˺��� ͨ����������Domain_name�ɻ����IP��ַ  
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "W5500_conf.h"
#include "dns.h"

uint8 domain_name[]="www.baidu.com";
uint8 dns_get_ip[4];
uint16 MSG_ID = 0x1122;
uint8 BUFPUB[1024];

/**
*@brief		 ����DNS��ѯ����
*@param		 op   - ����������
*@param		 name - ָ��������ָ��
*@param		 buf  - ����DNS��Ϣ��
*@param		 len  - buf�ĳ���
*@return	 ����DNS����ָ��
*/
int dns_makequery(uint16 op, uint8 * name, uint8 * buf, uint16 len)
{
  uint8  *cp;
  uint8   *cp1;
  //	int8   sname[MAX_DNS_BUF_SIZE];
  uint8  *dname;
  uint16 p;
  uint16 dlen;
  
  cp = buf;
  
  MSG_ID++;
  *(uint16*)&cp[0] = htons(MSG_ID);
  p = (op << 11) | 0x0100;
  *(uint16*)&cp[2] = htons(p);
  *(uint16*)&cp[4] = htons(1);
  *(uint16*)&cp[6] = htons(0);
  *(uint16*)&cp[8] = htons(0);
  *(uint16*)&cp[10]= htons(0);
  
  cp += sizeof(uint16)*6;
  //	strcpy(sname, name);
  dname = name;
  dlen = strlen((char*)dname);
  for (;;)
  {
    /* Look for next dot */
    cp1 = (unsigned char*)strchr((char*)dname, '.');
    
    if (cp1) len = cp1 - dname;																						/* More to come */
    else len = dlen;																											/* Last component */
    
    *cp++ = len;																													/* Write length of component */
    if (len == 0) break;
    
    strncpy((char *)cp, (char*)dname, len);																/* Copy component up to (but not including) dot */
    cp += len;
    if (!cp1)
    {
      *cp++ = 0;																													/* Last one; write null and finish */
      break;
    }
    dname += len+1;
    dlen -= len+1;
  }
  
  *(uint16*)&cp[0] = htons(0x0001);																				/* type */
  *(uint16*)&cp[2] = htons(0x0001);																				/* class */
  cp += sizeof(uint16)*2;
  
  return ((int)((uint32)(cp) - (uint32)(buf)));
}

/**
*@brief		 ��ѯDNS������Ϣ����������DNS�������Ļظ�
*@param		 s��DNS������socket��name:Ҫ��������Ϣ
*@return	 �ɹ�: ����1, ʧ�� :���� -1
*/
uint8 dns_query(uint8 s, uint8 * name)
{
  static uint32 dns_wait_time = 0;
  struct dhdr dhp;																												/*����һ���ṹ��������������ͷ��Ϣ*/
  uint8 ip[4];
  uint16 len, port;
  switch(getSn_SR(s))																											/*��ȡsocket״̬*/
  {
		case SOCK_CLOSED:
			dns_wait_time = 0;
			socket(s, Sn_MR_UDP, 3000, 0);																			/*��W5500��socket��3000�˿ڣ�������ΪUDPģʽ*/
			break;  
		
    case SOCK_UDP:																												/*socket�Ѵ�*/
			len = dns_makequery(0, name, BUFPUB, MAX_DNS_BUF_SIZE);							/*����DNS�����Ĳ�����BUFPUB*/
			sendto(s, BUFPUB, len, EXTERN_DNS_SERVERIP, IPPORT_DOMAIN);					/*����DNS�����ĸ�DNS������*/
      if ((len = getSn_RX_RSR(s)) > 0)
			{
        if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
				len = recvfrom(s, BUFPUB, len, ip, &port);												/*����UDP��������ݲ�����BUFPUB*/
        if(parseMSG(&dhp, BUFPUB))																				/*����DNS��Ӧ��Ϣ*/
				{
          close(s);																												/*�ر�socket*/
          return DNS_RET_SUCCESS;																					/*����DNS�����ɹ�������Ϣ*/
				}
        else 
					dns_wait_time = DNS_RESPONSE_TIMEOUT;														/*�ȴ���Ӧʱ������Ϊ��ʱ*/
      }
      else
      {
        delay_ms(1000);																										/*û���յ�DNS��������UDP�ظ�������̫Ƶ������ʱ1s*/
        dns_wait_time++;																									/*DNS��Ӧʱ���1*/
      }
      if(dns_wait_time >= DNS_RESPONSE_TIMEOUT)														/*���DNS�ȴ�ʱ�䳬��3s*/
      {
        close(s);																													/*�ر�socket*/
        return DNS_RET_FAIL;
      }
      break;
			
  }
  return DNS_RET_PROGRESS;
}

/**
*@brief		��ѯDNS������Ϣ����������DNS�������Ļظ�
*@param		s��DNS������socket��name:Ҫ��������Ϣ
*@return	�ɹ�: ����1, ʧ�� :���� -1
*/
void do_dns(void)
{
  uint8 dns_retry_cnt=0;
  uint8 dns_ok=0;
  if( (dns_ok==1) || (dns_retry_cnt > DNS_RETRY))
  {
    return;
  }

  else if(memcmp(ConfigMsg.dns,"\x00\x00\x00\x00",4))											/*�ж�DNS��������IP��ַ�Ƿ�����*/
  {
    switch(dns_query(SOCK_DNS,domain_name))																/*����DNS�����ĺͽ���DNS��Ӧ����*/
    {
      case DNS_RET_SUCCESS:																								/*DNS���������ɹ�*/
        dns_ok=1;																													/*DNS���б�־λ��1*/
        memcpy(ConfigMsg.rip,dns_get_ip,4);																/*�ѽ�������IP��ַ���Ƹ�ConfigMsg.rip*/
        dns_retry_cnt=0;																									/*DNS�����Ĵ�����0*/
        printf("Get [%s]'s IP address [%d.%d.%d.%d] from %d.%d.%d.%d\r\n",domain_name,ConfigMsg.rip[0],ConfigMsg.rip[1],ConfigMsg.rip[2],ConfigMsg.rip[3],ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
        break;
			
      case DNS_RET_FAIL:																									/*DNS��������ʧ��*/
        dns_ok=0;																													/*DNS���б�־λ��0*/
        dns_retry_cnt++;																									/*DNS�����Ĵ�����1*/
        printf("Fail! Please check your network configuration or DNS server.\r\n");
        break;
			
      default:
        break;
    }
		
  }
  else																																		/*���DNS������IPΪ0.0.0.0*/
     printf("Invalid DNS server [%d.%d.%d.%d]\r\n",ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
}


/**
*@brief		 ��ѹ��������Ϣת��Ϊ�ɶ�����ʽ
*@param		 msg        - ָ��ظ���Ϣ��ָ��
           compressed - ָ��ظ���Ϣ��������ָ��
           buf        - ��ſɶ���������Ϣ
           len        - buf�ĳ���
*@return	 ����ѹ��������Ϣ����
*/
int parse_name(uint8 * msg, uint8 * compressed, /*char * buf,*/ uint16 len)
{
  uint16 slen;																													/* Length of current segment */
  uint8  * cp;
  int16  clen = 0;																											/* Total length of compressed name */
  int16  indirect = 0;																									/* Set if indirection encountered */
  int16  nseg = 0;																											/* Total number of segments in name */
  int8   name[MAX_DNS_BUF_SIZE];
  int8   *buf;
  
  buf = name;
  
  cp = compressed;
  
  for (;;)
  {
    slen = *cp++;																												/* Length of this segment */
    
    if (!indirect) clen++;
    
    if ((slen & 0xc0) == 0xc0)
    {
      if (!indirect)
        clen++;
      indirect = 1;
      /* Follow indirection */
      cp = &msg[((slen & 0x3f)<<8) + *cp];
      slen = *cp++;
    }
    
    if (slen == 0)																											/* zero length == all done */
      break;
    
    len -= slen + 1;
    
    if (len <= 0) return -1;
    
    if (!indirect) clen += slen;
    
    while (slen-- != 0) *buf++ = (int8)*cp++;
    *buf++ = '.';
    nseg++;
  }

  if (nseg == 0)                                                    	 /* Root name; represent as single dot */
  {
    *buf++ = '.';
    len--;
  }
  
  *buf++ = '\0';
  len--;
  
  return clen;																												/* Length of compressed message */
}

/**
*@brief		 �����ظ���Ϣ����ѯ��¼
*@param		 msg - ָ��ظ���Ϣ��ָ��
           cp  - ָ����ѯ��¼��ָ��
*@return	 ������һ����¼ָ��
*/
uint8 * dns_question(uint8 * msg, uint8 * cp)
{
  int16 len;
  //	int8  xdata name[MAX_DNS_BUF_SIZE];
  
  len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
  
  if (len == -1) return 0;
  
  cp += len;
  cp += 2;		/* type */
  cp += 2;		/* class */
  
  return cp;
}

/**
*@brief		 �����ظ���Ϣ��Ӧ���¼
*@param		 msg - ָ��ظ���Ϣ��ָ��
           cp  - ����Ӧ���¼��ָ��
*@return	 ������һ��Ӧ���¼ָ��
*/
uint8 * dns_answer(uint8 * msg, uint8 * cp)
{
  int16 len, type;
  
  len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
  
  if (len == -1) return 0;
  
  cp += len;
  type = ntohs(*((uint16*)&cp[0]));
  cp += 2;		/* type */
  cp += 2;		/* class */
  cp += 4;		/* ttl */
  cp += 2;		/* len */
  
  switch (type)
  {
    case TYPE_A:
      dns_get_ip[0] = *cp++;
      dns_get_ip[1] = *cp++;
      dns_get_ip[2] = *cp++;
      dns_get_ip[3] = *cp++;
      break;
    case TYPE_CNAME:
    case TYPE_MB:
    case TYPE_MG:
    case TYPE_MR:
    case TYPE_NS:
    case TYPE_PTR:
      /* These types all consist of a single domain name */
      /* convert it to ascii format */
      len = parse_name(msg, cp, /*name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      
      cp += len;
      break;
		
    case TYPE_HINFO:
      len = *cp++;
      cp += len;
      len = *cp++;
      cp += len;
      break;
		
    case TYPE_MX:
      cp += 2;
      /* Get domain name of exchanger */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      cp += len;
      break;
		
    case TYPE_SOA:
      /* Get domain name of name server */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      cp += len;
      /* Get domain name of responsible person */
      len = parse_name(msg, cp,/* name,*/ MAX_DNS_BUF_SIZE);
      if (len == -1) return 0;
      
      cp += len;
      
      cp += 4;
      cp += 4;
      cp += 4;
      cp += 4;
      cp += 4;
      break;
		
    case TYPE_TXT:
      /* Just stash */
      break;
		
    default:
    /* Ignore */
      break;
  }
  
  return cp;
}

/**
*@brief		 ��������DNS�������Ļظ�����
*@param		 dhdr - ָ��DNS��Ϣͷ��ָ��
           buf  - ���ջظ���Ϣ
           len  - �ظ���Ϣ�ĳ���
*@return	 ��
*/
uint8 parseMSG(struct dhdr * pdhdr, uint8 * pbuf)
{
  uint16 tmp;
  uint16 i;
  uint8 * msg;
  uint8 * cp;
  
  msg = pbuf;
  memset(pdhdr, 0, sizeof(pdhdr));
  
  pdhdr->id = ntohs(*((uint16*)&msg[0]));
  tmp = ntohs(*((uint16*)&msg[2]));
  if (tmp & 0x8000) pdhdr->qr = 1;
  
  pdhdr->opcode = (tmp >> 11) & 0xf;
  
  if (tmp & 0x0400) pdhdr->aa = 1;
  if (tmp & 0x0200) pdhdr->tc = 1;
  if (tmp & 0x0100) pdhdr->rd = 1;
  if (tmp & 0x0080) pdhdr->ra = 1;
  
  pdhdr->rcode = tmp & 0xf;
  pdhdr->qdcount = ntohs(*((uint16*)&msg[4]));
  pdhdr->ancount = ntohs(*((uint16*)&msg[6]));
  pdhdr->nscount = ntohs(*((uint16*)&msg[8]));
  pdhdr->arcount = ntohs(*((uint16*)&msg[10]));
  
  /* Now parse the variable length sections */
  cp = &msg[12]; 
  /* Question section */
  for (i = 0; i < pdhdr->qdcount; i++)
  {
    cp = dns_question(msg, cp);
  } 
  /* Answer section */
  for (i = 0; i < pdhdr->ancount; i++)
  {
    cp = dns_answer(msg, cp);
  }  
  /* Name server (authority) section */
  for (i = 0; i < pdhdr->nscount; i++)
  {
    ;
  }  
  /* Additional section */
  for (i = 0; i < pdhdr->arcount; i++)
  {
    ;
  }
  
  if(pdhdr->rcode == 0) return 1;	
  else return 0;
}


