#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stm32f10x.h"
#include "ult.h"
#include "w5500.h"
#include "socket.h"
#include "dns.h"
#include "usart.h"

uint8 buffer[2048]={0};												// 定义一个2KB的数组，用来存放Socket的通信数据
uint8 len,dns_flag=0;										// 定义串口输入的数据长度、初始化DNS标志位

//uint8 domain_name[];
uint8 DNS_GET_IP[4];
uint16 MSG_ID = 0x1122;
uint8 BUFPUB[1024];
uint8 dns_num=0;
extern CONFIG_MSG ConfigMsg;


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
  p = (op << 11) | 0x0100;			/* Recursion desired */
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
    
    if (cp1) len = cp1 - dname;	/* More to come */
    else len = dlen;			/* Last component */
    
    *cp++ = len;				/* Write length of component */
    if (len == 0) break;
    
    /* Copy component up to (but not including) dot */
    strncpy((char *)cp, (char*)dname, len);
    cp += len;
    if (!cp1)
    {
      *cp++ = 0;			/* Last one; write null and finish */
      break;
    }
    dname += len+1;
    dlen -= len+1;
  }
  
  *(uint16*)&cp[0] = htons(0x0001);				/* type */
  *(uint16*)&cp[2] = htons(0x0001);				/* class */
  cp += sizeof(uint16)*2;
  
  return ((int)((uint32)(cp) - (uint32)(buf)));
}


int parse_name(uint8 * msg, uint8 * compressed, /*char * buf,*/ uint16 len)
{
  uint16 slen;		/* Length of current segment */
  uint8  * cp;
  int16  clen = 0;		/* Total length of compressed name */
  int16  indirect = 0;	/* Set if indirection encountered */
  int16  nseg = 0;		/* Total number of segments in name */
  int8   name[MAX_DNS_BUF_SIZE];
  int8   *buf;
  
  buf = name;
  
  cp = compressed;
  
  for (;;)
  {
    slen = *cp++;	/* Length of this segment */
    
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
    
    if (slen == 0)	/* zero length == all done */
      break;
    
    len -= slen + 1;
    
    if (len <= 0) return -1;
    
    if (!indirect) clen += slen;
    
    while (slen-- != 0) *buf++ = (int8)*cp++;
    *buf++ = '.';
    nseg++;
  }
  
  if (nseg == 0)
  {
    /* Root name; represent as single dot */
    *buf++ = '.';
    len--;
  }
  
  *buf++ = '\0';
  len--;
  
  return clen;	/* Length of compressed message */
}


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

uint8 * dns_answer(uint8 * msg, uint8 * cp)
{
  int16 len, type;
  //	int8  xdata name[MAX_DNS_BUF_SIZE];
  
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
      DNS_GET_IP[0] = *cp++;
      DNS_GET_IP[1] = *cp++;
      DNS_GET_IP[2] = *cp++;
      DNS_GET_IP[3] = *cp++;
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
  
  if(pdhdr->rcode == 0) return 1;		// No error
  else return 0;
}

uint8 dns_query(uint8 s, uint8 * name)
{
  static uint32 dns_wait_time = 0;
  struct dhdr dhp;
  uint8 ip[4];
  uint16 len, port;
  switch(getSn_SR(s))
  {
    case SOCK_UDP:
      if ((len = getSn_RX_RSR(s)) > 0)
      {
        if (len > MAX_DNS_BUF_SIZE) len = MAX_DNS_BUF_SIZE;
        len = recvfrom(s, BUFPUB, len, ip, &port);
        if(parseMSG(&dhp, BUFPUB))
        {
          close(s);
          return DNS_RET_SUCCESS;
        }
        else 
          dns_wait_time = DNS_RESPONSE_TIMEOUT;
      }
      else
      {
        Delay_ms(1000);
        dns_wait_time++;
        //if(ConfigMsg.debug) printf("dns wait time=%d\r\n", dns_wait_time);
      }
      if(dns_wait_time >= DNS_RESPONSE_TIMEOUT)   
      {
        close(s);
        return DNS_RET_FAIL;
      }
      break;
    case SOCK_CLOSED:
      dns_wait_time = 0;
      socket(s, Sn_MR_UDP, 3000, 0);
      //if(ConfigMsg.debug) printf("dns socket init over\r\n");
      len = dns_makequery(0, name, BUFPUB, MAX_DNS_BUF_SIZE);
      //if(ConfigMsg.debug) printf("dns make query\r\n");
      sendto(s, BUFPUB, len, EXTERN_DNS_SERVERIP, IPPORT_DOMAIN);
      break;         
  }
  return DNS_RET_PROGRESS;
}

uint16 do_dns()
{
  uint8 dns_retry_cnt=0;
  uint8 dns_ok=0;
  if( (dns_ok==1) ||  (dns_retry_cnt > DNS_RETRY))
  {
    return 0;
  }

  else if(memcmp(ConfigMsg.dns,"\x00\x00\x00\x00",4))//判断DNS服务器的IP地址是否配置
  {
    switch(dns_query(SOCK_DNS,buffer))//发出DNS请求报文和解析DNS响应报文
    {
      case DNS_RET_SUCCESS:								//DNS解析域名成功
        dns_ok=1;														//DNS运行标志位置1
        memcpy(ConfigMsg.rip,DNS_GET_IP,4);//把解析到的IP地址复制给ConfigMsg.rip
        dns_retry_cnt=0;										//DNS请求报文次数置0
			  dns_num =10;
        printf("%d.%d.%d.%d\r\n",ConfigMsg.rip[0],ConfigMsg.rip[1],ConfigMsg.rip[2],ConfigMsg.rip[3]);
        break;
      case DNS_RET_FAIL:											//DNS解析域名失败
        dns_ok=0;															//DNS运行标志位置0
        dns_retry_cnt++;											//DNS请求报文次数加1
        printf("Fail! Please check your network configuration or DNS server.\r\n");
        break;
      default:
        break;
    }
    
  }
  else																	//如果DNS服务器IP为0.0.0.0
     printf("Invalid DNS server [%d.%d.%d.%d]\r\n",ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
			dns_num++;
	
	return 1;
}

uint8 check_usart_buf(void)
{
	if(USART_RX_STA&0x8000)								// 串口数据发送
	{				   
		len=USART_RX_STA&0x3fff;
		memcpy(buffer,USART_RX_BUF,len);
		USART_RX_STA=0;
		memset(USART_RX_BUF,0,len+1);
		printf("[ %s ]'s IP Address is:\r\n",buffer);
		dns_flag=1;													// DNS标志位置1
	}
	if(dns_flag==1)
	{			
		if(dns_num>=10)											// DNS次数≥10
		{
			dns_flag=0;												// DNS标志位清0
			memset(buffer,0,len+1);
			dns_num=0;												// dns_num清0
			printf("Input the domain name(eg>>www.baidu.com):\r\n");
		}
		else
			return 1;			
	}
	return 0;
}
