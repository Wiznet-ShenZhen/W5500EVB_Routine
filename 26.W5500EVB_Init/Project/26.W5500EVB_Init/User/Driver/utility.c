/**
******************************************************************************
* @file    			utility.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-02-14
* @brief 				����ʵ�ú��� ��ʱ���� ���ݸ�ʽת�� 
******************************************************************************
*/
#include "w5500.h"
#include "w5500_conf.h"
#include "utility.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>


static u8  fac_us=0;																										/*us��ʱ������*/
static u16 fac_ms=0;																										/*ms��ʱ������*/

/**
*@brief		��ʼ���ӳٺ���
*@param		SYSCLK:ϵͳʱ��
*@return	��
*/
void systick_init (u8 sysclk)
{
	SysTick->CTRL&=0xfffffffb;																						/*bit2���,ѡ���ⲿʱ��  HCLK/8*/
	fac_us=sysclk/8;		    
	fac_ms=(u16)fac_us*1000;
}								    

/**
*@brief	  ����ʱ����
*@param		time_s:Ҫ��ʱ��ʱ����
*@return	��
*/
void delay_s( uint32 time_s )
{
  for(;time_s>0;time_s--)
    delay_ms(1000);
}

/**
*@brief	  ������ʱ����
*@param		time_ms:Ҫ��ʱ����ʱ����
*@return	��
*/
void delay_ms( uint32 time_ms )
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)time_ms*fac_ms;																		/*ʱ�����(SysTick->LOADΪ24bit)*/
	SysTick->VAL =0x00;           																				/*��ռ�����*/
	SysTick->CTRL=0x01 ;         																					/*��ʼ����*/ 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));																		/*�ȴ�ʱ�䵽��*/
	SysTick->CTRL=0x00;       																						/*�رռ�����*/
	SysTick->VAL =0X00;      																							/*��ռ�����*/	  	    
}   

/**
*@brief	  ΢����ʱ����
*@param		time_ms:Ҫ��ʱ΢��ʱ����
*@return	��
*/
void delay_us( uint32 time_us )
{		
	u32 temp;	    	 
	SysTick->LOAD=time_us*fac_us; 																				/*ʱ�����*/	  		 
	SysTick->VAL=0x00;        																						/*��ռ�����*/
	SysTick->CTRL=0x01 ;      																						/*��ʼ���� */	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));																		/*�ȴ�ʱ�䵽��*/
	SysTick->CTRL=0x00;       																						/*�رռ�����*/
	SysTick->VAL =0X00;       																						/*��ռ�����*/
}

/**
*@brief	 	�ַ�תת��Ϊ8λ���ͺ���
*@param		str:Ҫת���ַ����� base:
*@return	num:����ת�����������
*/
uint16 atoi16(char* str,uint16 base	)
{
  unsigned int num = 0;
  while (*str !=0)
          num = num * base + c2d(*str++);
  return num;
}

/**
*@brief	 	�ַ�תת��Ϊ32λ���ͺ���
*@param		str:Ҫת���ַ����� base:
*@return	num:����ת�����������
*/
uint32 atoi32(char* str,uint16 base	)
{
  uint32 num = 0;
  while (*str !=0)
          num = num * base + c2d(*str++);
  return num;
}

/**
*@brief	 	������ת��Ϊ�ַ�������
*@param		n:Ҫת�������� str[5]:���ת������ַ���  len������������
*@return	��
*/
void itoa(uint16 n,uint8 str[5], uint8 len)
{
  
  uint8 i=len-1;

  memset(str,0x20,len);
  do{
  str[i--]=n%10+'0';
  
 }while((n/=10)>0);

 return;
}


/**
*@brief	 	���ַ���ת��Ϊʮ���ƻ�ʮ������������
*@param		str:Ҫת���ַ����� len������������
*@return	�ɹ� - 1, ʧ�� - 0
*/
int validatoi(char* str, int base,int* ret)
{
  int c;
  char* tstr = str;
  if(str == 0 || *str == '\0') return 0;
  while(*tstr != '\0')
  {
    c = c2d(*tstr);
    if( c >= 0 && c < base) tstr++;
    else    return 0;
  }
  
  *ret = atoi16(str,base);
  return 1;
}

/**
*@brief	 	���µ��ַ�ȥ�滻�ַ�����������ַ�
*@param		str:�滻���ַ�����oldchar:������ַ���newchar���µ��ַ�	
*@return	��
*/
void replacetochar(char * str,	char oldchar,char newchar	)
{
  int x;
  for (x = 0; str[x]; x++) 
    if (str[x] == oldchar) str[x] = newchar;	
}

/**
*@brief	 	��ʮ������ת��Ϊ�ַ���
*@param		c:Ҫת��ʮ��������
*@return	����һ���ַ�������
*/
char c2d(uint8 c	)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';

	return (char)c;
}

/**
*@brief	 	16λ�ַ���8λ��8λת��
*@param		i:Ҫת��������
*@return	ת���������
*/
uint16 swaps(uint16 i)
{
  uint16 ret=0;
  ret = (i & 0xFF) << 8;
  ret |= ((i >> 8)& 0xFF);
  return ret;	
}
/**
*@brief	 	32λ�ַ��ߵ�λ�任
*@param		i:Ҫת��������
*@return	ת���������
*/
uint32 swapl(uint32 l)
{
  uint32 ret=0;
  ret = (l & 0xFF) << 24;
  ret |= ((l >> 8) & 0xFF) << 16;
  ret |= ((l >> 16) & 0xFF) << 8;
  ret |= ((l >> 24) & 0xFF);
  return ret;
}

/**
*@brief	 	�ַ�������
*@param		srcĿ���ַ��� s1 s2�����ַ���
*@return	��
*/
void mid(int8* src, int8* s1, int8* s2, int8* sub)
{
	int8* sub1;
	int8* sub2;
	uint16 n;

  sub1=strstr(src,s1);
  sub1+=strlen(s1);
  sub2=strstr(sub1,s2);
  n=sub2-sub1;
  strncpy(sub,sub1,n);
  sub[n]=0;
}

/**
*@brief	 	ip�����ַת��
*@param		adr����ַ ip��ip
*@return	��
*/
void inet_addr_(unsigned char* addr,unsigned char *ip)
{
	int i;
	char taddr[30];
	char * nexttok;
	char num;
	strcpy(taddr,(char *)addr);
	
	nexttok = taddr;
	for(i = 0; i < 4 ; i++)
	{
		nexttok = strtok(nexttok,".");
		if(nexttok[0] == '0' && nexttok[1] == 'x') num = atoi16(nexttok+2,0x10);
		else num = atoi16(nexttok,10);
		
		ip[i] = num;
		nexttok = NULL;
	}
}	
 
/**
*@brief	 	��32��ַת��Ϊʮ���Ƹ�ʽ
*@param		addr:  Ҫת����ַ
*@return	����ʮ���Ƹ�ʽ��ַ
*/
char* inet_ntoa(
	unsigned long addr	
	)
{
	static char addr_str[32];
	memset(addr_str,0,32);
	sprintf(addr_str,"%d.%d.%d.%d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	return addr_str;
}

/**
*@brief	 	��16��ַת��Ϊʮ���Ƹ�ʽ
*@param		addr:  Ҫת����ַ
*@return	����ʮ���Ƹ�ʽ��ַ
*/
char* inet_ntoa_pad(unsigned long addr)
{
	static char addr_str[16];
	memset(addr_str,0,16);
	printf(addr_str,"%03d.%03d.%03d.%03d",(int)(addr>>24 & 0xFF),(int)(addr>>16 & 0xFF),(int)(addr>>8 & 0xFF),(int)(addr & 0xFF));
	return addr_str;
}
 
/**
*@brief	 	��֤IP��ַ
*@param		ip addr
*@return	�ɹ� - 1, ʧ�� - 0
*/
char verify_ip_address(char* src, uint8 * ip)
{
	int i;
	int tnum;
	char tsrc[50];
	char* tok = tsrc;
	
	strcpy(tsrc,src);
	
	for(i = 0; i < 4; i++)
	{
		tok = strtok(tok,".");
		if ( !tok ) return 0;
		if(tok[0] == '0' && tok[1] == 'x')
		{
			if(!validatoi(tok+2,0x10,&tnum)) return 0;
		}
		else if(!validatoi(tok,10,&tnum)) return 0;

		ip[i] = tnum;
		
		if(tnum < 0 || tnum > 255) return 0;
		tok = NULL;
	}
	return 1;	
}

/**
*@brief		��һ�� ����ģʽ��unsigned short������ת�������ģʽ��TCP/IP �����ֽڸ�ʽ������.
*@param		Ҫת��������
*@return 	���ģʽ������
*/ 
uint16 htons( 
	uint16 hostshort	/**< A 16-bit number in host byte order.  */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return swaps(hostshort);
#else
	return hostshort;
#endif		
}

/**
*@brief		��һ�� ����ģʽ��unsigned long������ת�������ģʽ��TCP/IP �����ֽڸ�ʽ������.
*@param		Ҫת��������
*@return 	���ģʽ������
*/ 
unsigned long htonl(
	unsigned long hostlong		/**< hostshort  - A 32-bit number in host byte order.  */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return swapl(hostlong);
#else
	return hostlong;
#endif	
}



/**
*@brief		��һ�����ģʽ��TCP/IP �����ֽڸ�ʽ������ת��������ģʽ��unsigned short������
*@param		Ҫת��������
*@return 	unsigned shortģʽ������
*/ 
unsigned long ntohs(
	unsigned short netshort	/**< netshort - network odering 16bit value */
	)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )	
	return htons(netshort);
#else
	return netshort;
#endif		
}


/**
*@brief		��һ�����ģʽ��TCP/IP �����ֽڸ�ʽ������ת��������ģʽ��unsigned long������
*@param		Ҫת��������
*@return 	unsigned longģʽ������
*/ 
unsigned long ntohl(unsigned long netlong)
{
#if ( SYSTEM_ENDIAN == _ENDIAN_LITTLE_ )
	return htonl(netlong);
#else
	return netlong;
#endif		
}



/**
*@brief		�����ַ���У��ֵ
*@param		Ҫת��������
*@return 	У��ֵ
*/ 
unsigned short checksum(
	unsigned char * src, 	/**< pointer to stream  */
	unsigned int len		/**< size of stream */
	)
{
	u_int sum, tsum, i, j;
	u_long lsum;

	j = len >> 1;

	lsum = 0;

	for (i = 0; i < j; i++) 
	{
		tsum = src[i * 2];
		tsum = tsum << 8;
		tsum += src[i * 2 + 1];
		lsum += tsum;
	}

	if (len % 2) 
	{
		tsum = src[i * 2];
		lsum += (tsum << 8);
	}


	sum = lsum;
	sum = ~(sum + (lsum >> 16));
	return (u_short) sum;	
}

/**
*@brief		����ַ�Ƿ�����
*@param		Ҫת��������
*@return 	�ɹ�Ϊ1 ʧ��Ϊ0
*/ 
u_char check_dest_in_local(u_long destip)
{
	int i = 0;
	u_char * pdestip = (u_char*)&destip;
	for(i =0; i < 4; i++)
	{
		if((pdestip[i] & IINCHIP_READ(SUBR0+i)) != (IINCHIP_READ(SIPR0+i) & IINCHIP_READ(SUBR0+i)))
			return 1;	// Remote
	}
	return 0;
}
