#include "auto_update.h"

/*
*查询版本号
******/
char  postH[]={
"POST /fw_update/2.php HTTP1.1\r\n"\
"Host:w5500.com\r\n"\
"Accept:image/gif,image/x-xbitmap,image/jpeg,image/pjpeg,*/*\r\n"\
"User-Agent: Mozilla/4.0 (compatible;MSTE 5.5;Windows 98)\r\n"\
"Content-Length:21\r\n"\
"Content-Type:application/x-www-form-urlencoded\r\n"\
"Cache-Control:no-Cache\r\n"\
"Connection:close\r\n"\
"\r\n"\
"mac=00:08:DC:11:12:13"\
};

uint8 flg=0;
uint8 tail;
uint32 data_len;
uint16 rx_len;
int8 sub[10];
uint8 update_flag =0;   										 //自动更新标志位
uint8 yeelink_server_ip[4]={120,24,96,192};  // 服务器IP地址
uint8 yeelink_server_port=80;	               // 服务器端口号
uint8 Buffer[2048]={0};
uint16 anyport=3000;
uint32 content_len=0;
uint32 recv_count=0;
uint8 ver_num[10]={'"','1','.','0','.','0','"'};//存储版本号
char bin_name[100]={0};
char post_msg[1024];              // 拼接报文，获取下载数据 
uint8  recv_buffer[2048];
/*
*@功能：  测试APP版本号
*@参数：  无
*@返回值：无
***/


void w5500_version(void)
{
	uint16 len;
	uint8  version[10];   	
	switch(getSn_SR(W5500_UPDATE))
  {
		case SOCK_ESTABLISHED:
    if(getSn_IR(W5500_UPDATE) & Sn_IR_CON)
    {
      setSn_IR(W5500_UPDATE, Sn_IR_CON);
    }
    send(W5500_UPDATE,(const uint8 *)postH,sizeof(postH));                 //发送验证			    					
		delay_ms(5000);
		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)		
    {
            len = recv(W5500_UPDATE, (uint8*)recv_buffer, len);            //接收数据
			      if(strstr((char*)recv_buffer,"\"error\""))                     //报文内包含error,就结束函数
						{
							  printf("upload error\r\n");
						    return;
						}						
			     //  printf("%s\r\n",recv_buffer);
			      mid((char*)recv_buffer,"\"version\":",",",(char*)version);     //可以获取路径
			     // printf("%s\r\n",version);	
						/*********读取版本号************/
						 ver_num[1]=ConfigMsg.op[0];
						 ver_num[3]=ConfigMsg.op[1];
						 ver_num[5]=ConfigMsg.op[2];
						if(strncmp((const char *)ver_num,(const char *)version,7)<0)				
						{ 							
			       update_flag=1;
						 //版本号写入Flash
						 ConfigMsg.op[0]=version[1];
						 ConfigMsg.op[1]=version[3];
						 ConfigMsg.op[2]=version[5];
						 write_ver_to_flash();
						 mid((char*)recv_buffer,"\"http://W5500.com/fw_update/upload/","\",",(char*)bin_name);//可以获取路径
						 snprintf(post_msg,sizeof(post_msg),"POST /fw_update/upload/%s HTTP/1.1\r\n"\
             "Host:w5500.com\r\n"\
             "Accept:image/gif,image/x-xbitmap,image/jpeg,image/pjpeg,*/*\r\n"\
             "Pragma:no-cache\r\n"\
             "Accept-Encoding: gzip,deflate\r\n"\
             "Connection:keep-alive\r\n"\
             "\r\n",bin_name);
					   printf("The version is %s\r\n",ver_num);
						}else
            {
							update_flag=2;
							printf("The version is %s\r\n",version);	
							printf("The version is no need to update\r\n");
						  return;
						}
			       		
      }					
		close(W5500_UPDATE);
    break;
  case SOCK_CLOSE_WAIT:
    //disconnect(SOCK_FW);
    break;
 
	case SOCK_CLOSED:
    socket(W5500_UPDATE,Sn_MR_TCP,30000,Sn_MR_ND);
    break;
  case SOCK_INIT:                       
     connect(W5500_UPDATE, yeelink_server_ip ,yeelink_server_port);
    break;
  }
}

/*
*@功能：  更新固件
*@参数：  无
 @返回值：无
****/

void w5500_update(void)
{
	char *p;
	uint32 i=0;
	uint32 len=0;
	uint32 rxlen =0;
  uint32 tmplen = 0;
	uint32 n_erased=0;
	uint8 n_pages=0;
	uint32 flashdest = AppBackupAddress;
	uint16 data;
  switch(getSn_SR(W5500_UPDATE))
  {
	case SOCK_ESTABLISHED:
			printf("connect server\r\n");
    if(getSn_IR(W5500_UPDATE) & Sn_IR_CON)
    {
      setSn_IR(W5500_UPDATE, Sn_IR_CON);
    }
    	send(W5500_UPDATE,(const uint8 *)post_msg,sizeof(post_msg));//发送验证			
		  delay_ms(3000);
		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)		
    {
            len = recv(W5500_UPDATE, (uint8*)Buffer, len); //接收数据
			   //   printf("%s\r\n",Buffer);			   
				    mid((char*)Buffer,"Content-Length: ","\r\n",sub);//获取字符串长度
				    p=strstr((char*)Buffer,"\r\n\r\n");
				    tmplen =len-( p-(char*)Buffer)-4;//第一个包内的数据长度
				    content_len=ATOI32(sub,10);

			      printf("\r\ncontent_len:%ld\r\n",content_len);
				    while(rxlen!=content_len)
				   { 						
              if(rxlen ==0)
              {
								printf("first pakge\r\n");
								printf("download \r\n");
							  FLASH_Unlock();
                FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//擦除标志位
								n_pages = FLASH_PagesMask(content_len);
                for(n_erased = 0; n_erased < n_pages; n_erased++)
                {
									//printf("擦除flash\r\n");
                  FLASH_ErasePage(AppBackupAddress + 0x400 * n_erased);//擦除flash的内容
                }
								
								if((tmplen%2)!=0)//如果是奇数个数据
								{
								  flg=1;
									tail=Buffer[len-1];//保留最后一字节的数据
								}
								
								for(i=0;i<tmplen-1;i=i+2) //半字写入
								{
									data = *(p+4+i+1);
									data=(data<<8) + (*(p+4+i));
									FLASH_ProgramHalfWord(flashdest, data);					
									recv_count++;
									flashdest += 2;
								}									
								rxlen = tmplen;

								tmplen=0;
							}else if(rxlen>0)
						  {
								
								memset(Buffer,0xff,2048);
								tmplen = getSn_RX_RSR(W5500_UPDATE);
								if(tmplen>0)
							  {	
										printf("%c\r\n",'.');
									if(flg==1)//判断上个包是否有数据
									{
									  tmplen=recv(W5500_UPDATE, (uint8*)(Buffer+1),tmplen);
										
										Buffer[0]=tail;//拼接数据
									  //	flg=0;
										if(((tmplen+1)%2)!=0)//总字节数是奇数，flg置位，取出最后一个字节
									  {
									    flg=1;
									    tail=Buffer[tmplen];
										  //printf("tail:%d\r\n",tail);
											data_len = tmplen+1;//总字节数
									  }else//总字节数为偶数
										{
										  flg=0;//清除标志位
											tail=0;
											data_len = tmplen+1;
										}	
									}
									else
								  {	
                    //上个包为偶数个数据										
					          tmplen=recv(W5500_UPDATE, (uint8*)Buffer,tmplen);
										if((tmplen%2)!=0)//本次数据包为奇数个
									  {
									    flg=1;//标志位置位
									    tail=Buffer[tmplen-1];//保存最后一个数据
									  }
										data_len =tmplen;
									}
									if((rxlen+tmplen)==content_len)//判断是否为最后一个包
									{   
									  	data_len =	data_len +2;
									}
							     for(i = 0; i<data_len-1; i+=2)
								  {
										data = Buffer[i+1];
										data =(data<<8) + Buffer[i];
									  FLASH_ProgramHalfWord(flashdest, data);
									  flashdest += 2;
									  recv_count++;
								   }						
                   rxlen += tmplen;	            					 
					         tmplen = 0;
					
								 }
						  }

				 }
				 FLASH_Lock();
				 printf("download %ld bytes ok\r\n",rxlen);
				 ConfigMsg.fw_len = content_len;
				 ConfigMsg.state=NEW_APP_IN_BACK;// 改变状态
				 write_config_to_flash();
			   update_flag=2;
				 delay_ms(1000);  //wait for a while
         reboot();        //进入boot	       		
      }					
		close(W5500_UPDATE);
    break;
  case SOCK_CLOSE_WAIT:
    //disconnect(SOCK_FW);
    break;
  case SOCK_CLOSED:
    socket(W5500_UPDATE,Sn_MR_TCP,AUTO_UPDATE_PORT,Sn_MR_ND);
    break;
  case SOCK_INIT:
		//  printf("init socket");
     connect(W5500_UPDATE, yeelink_server_ip ,yeelink_server_port);
    break;
  }
	
}


/*
*@功能：  将字符串转换位整形数
*@参数：  无
 @返回值：整形数
****/
uint32 ATOI32(char* str,uint16 base	)
{
  uint32 num = 0;
  while (*str !=0)
          num = num * base + C2D(*str++);
  return num;
}
char C2D(uint8 c	)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return 10 + c -'a';
	if (c >= 'A' && c <= 'F')
		return 10 + c -'A';
	return (char)c;
}



























