/**
******************************************************************************
* @file    			HTTPS.c
* @author  			WIZnet Software Team 
* @version 			V1.0
* @date    			2015-12-12
* @brief   			heeputil.c/.h webpge.h ���ʹ�� ���Web Server����
* @attention		ѡ��IE��������в���
*
******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "w5500.h"
#include "socket.h"
#include "utility.h"
#include "http_server.h"

extern char  homepage_default[];
char tx_buf[MAX_URI_SIZE];
char rx_buf[MAX_URI_SIZE];
 uint8 pub_buf[1024];

 /**
*@brief		  ת��ת���ַ�Ϊascii charater 
*@param		  url����Ҫת����ҳ��ַ
*@return	  ��
*/
void unescape_http_url(char * url)
{
  int x, y;
  
  for (x = 0, y = 0; url[y]; ++x, ++y) 
  {
    if ((url[x] = url[y]) == '%') 
	{
      url[x] = c2d(url[y+1])*0x10+c2d(url[y+2]);
      y+=2;
    }
  }
  url[x] = '\0';
}

 /**
*@brief		  ִ��һ���𸴣��� html, gif, jpeg,etc.
*@param		  buf- ������
*@param			type- ����������
*@param			len-  �����ݳ���
*@return	  ��
*/
void make_http_response_head(unsigned char * buf,char type,uint32 len)
{
  char * head;
  char tmp[10];
  memset(buf,0x00,MAX_URI_SIZE); 
  /* �ļ�����*/
  if 	(type == PTYPE_HTML) head = RES_HTMLHEAD_OK;
  else if (type == PTYPE_GIF)	head = RES_GIFHEAD_OK;
  else if (type == PTYPE_TEXT)	head = RES_TEXTHEAD_OK;
  else if (type == PTYPE_JPEG)	head = RES_JPEGHEAD_OK;
  else if (type == PTYPE_FLASH)	head = RES_FLASHHEAD_OK;
  else if (type == PTYPE_MPEG)	head = RES_MPEGHEAD_OK;
  else if (type == PTYPE_PDF)	head = RES_PDFHEAD_OK;

  sprintf(tmp,"%ld", len);	
  strcpy((char*)buf, head);
  strcat((char*)buf, tmp);
  strcat((char*)buf, "\r\n\r\n");
  //printf("%s\r\n", buf);
}

 /**
*@brief		  Ѱ��һ��MIME�����ļ�
*@param		  buf- MIME���ļ�
*@param		  type-��������
*@return	  ��
*/
void find_http_uri_type(u_char * type, char * buf) 
{
  /* Decide type according to extention*/
  if 	(strstr(buf, ".pl"))				*type = PTYPE_PL;
  else if (strstr(buf, ".html") || strstr(buf,".htm"))	*type = PTYPE_HTML;
  else if (strstr(buf, ".gif"))				*type = PTYPE_GIF;
  else if (strstr(buf, ".text") || strstr(buf,".txt"))	*type = PTYPE_TEXT;
  else if (strstr(buf, ".jpeg") || strstr(buf,".jpg"))	*type = PTYPE_JPEG;
  else if (strstr(buf, ".swf")) 				*type = PTYPE_FLASH;
  else if (strstr(buf, ".mpeg") || strstr(buf,".mpg"))	*type = PTYPE_MPEG;
  else if (strstr(buf, ".pdf")) 				*type = PTYPE_PDF;
  else if (strstr(buf, ".cgi") || strstr(buf,".CGI"))	*type = PTYPE_CGI;
  else if (strstr(buf, ".js") || strstr(buf,".JS"))	*type = PTYPE_TEXT;	
  else if (strstr(buf, ".xml") || strstr(buf,".XML"))	*type = PTYPE_HTML;
  else 							*type = PTYPE_ERR;
}

 /**
*@brief		  ����ÿһ��http��Ӧ
*@param		  request�� ����һ��ָ��
*@return	  ��
*/
void parse_http_request(st_http_request * request,u_char * buf)
{
  char * nexttok;
  nexttok = strtok((char*)buf," ");
  if(!nexttok)
  {
    request->METHOD = METHOD_ERR;
    return;
  }
  if(!strcmp(nexttok, "GET") || !strcmp(nexttok,"get"))
  {
    request->METHOD = METHOD_GET;
    nexttok = strtok(NULL," ");
   			
  }
  else if (!strcmp(nexttok, "HEAD") || !strcmp(nexttok,"head"))	
  {
    request->METHOD = METHOD_HEAD;
    nexttok = strtok(NULL," ");
  		
  }
  else if (!strcmp(nexttok, "POST") || !strcmp(nexttok,"post"))
  {
    nexttok = strtok(NULL,"\0");//20120316
    //nexttok = strtok(NULL," ");
    request->METHOD = METHOD_POST;
 		
  }
  else
  {
    request->METHOD = METHOD_ERR;
  }	 
  if(!nexttok)
  {
    request->METHOD = METHOD_ERR; 			
    return;
  }
  strcpy(request->URI,nexttok); 					
}

 /**
*@brief		  �õ���Ӧ�����е���һ������
*@param		  url����Ҫת����ҳ��ַ
*@param			param_name�� 
*@return	  ����һ������
*/
uint8* get_http_param_value(char* uri, char* param_name)
{
  uint16 len;
  uint8* pos2;
  uint8* name=0; 
  uint8 *ret=pub_buf;
  uint16 content_len=0;
  int8 tmp_buf[10]={0x00,};
  if(!uri || !param_name) return 0;
  /***************/
  mid(uri,"Content-Length: ","\r\n",tmp_buf);
  content_len=atoi16(tmp_buf,10);
  //printf("content len=%d\r\n",content_len);
  uri = (int8*)strstr(uri,"\r\n\r\n");
  uri+=4;
  //printf("uri=%s\r\n",uri);
  uri[content_len]=0;
  /***************/	 
  name= (uint8*)strstr(uri,param_name);
  if(name)
  {
    name += strlen(param_name) + 1; 
    pos2=(uint8*)strstr((char*)name,"&");
    if(!pos2) 
    {
      pos2=name+strlen((char*)name);
    }
    len=0;
    len = pos2-name;
 
    if(len)
    {
      ret[len]=0;
      strncpy((char*)ret,(char*)name,len);
      unescape_http_url((char *)ret);
      replacetochar((char *)ret,'+',' ');
    }
    else
      ret[0]=0;
  }
  else
    return 0;
  return ret;		
}

