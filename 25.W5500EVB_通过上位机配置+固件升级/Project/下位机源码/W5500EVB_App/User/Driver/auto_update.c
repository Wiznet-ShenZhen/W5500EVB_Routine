#include "auto_update.h"

/*
*��ѯ�汾��
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
uint8 update_flag =0;   										 //�Զ����±�־λ
uint8 yeelink_server_ip[4]={120,24,96,192};  // ������IP��ַ
uint8 yeelink_server_port=80;	               // �������˿ں�
uint8 Buffer[2048]={0};
uint16 anyport=3000;
uint32 content_len=0;
uint32 recv_count=0;
uint8 ver_num[10]={'"','1','.','0','.','0','"'};//�洢�汾��
char bin_name[100]={0};
char post_msg[1024];              // ƴ�ӱ��ģ���ȡ�������� 
uint8  recv_buffer[2048];
/*
*@���ܣ�  ����APP�汾��
*@������  ��
*@����ֵ����
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
    send(W5500_UPDATE,(const uint8 *)postH,sizeof(postH));                 //������֤			    					
		delay_ms(5000);
		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)		
    {
            len = recv(W5500_UPDATE, (uint8*)recv_buffer, len);            //��������
			      if(strstr((char*)recv_buffer,"\"error\""))                     //�����ڰ���error,�ͽ�������
						{
							  printf("upload error\r\n");
						    return;
						}						
			     //  printf("%s\r\n",recv_buffer);
			      mid((char*)recv_buffer,"\"version\":",",",(char*)version);     //���Ի�ȡ·��
			     // printf("%s\r\n",version);	
						/*********��ȡ�汾��************/
						 ver_num[1]=ConfigMsg.op[0];
						 ver_num[3]=ConfigMsg.op[1];
						 ver_num[5]=ConfigMsg.op[2];
						if(strncmp((const char *)ver_num,(const char *)version,7)<0)				
						{ 							
			       update_flag=1;
						 //�汾��д��Flash
						 ConfigMsg.op[0]=version[1];
						 ConfigMsg.op[1]=version[3];
						 ConfigMsg.op[2]=version[5];
						 write_ver_to_flash();
						 mid((char*)recv_buffer,"\"http://W5500.com/fw_update/upload/","\",",(char*)bin_name);//���Ի�ȡ·��
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
*@���ܣ�  ���¹̼�
*@������  ��
 @����ֵ����
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
    	send(W5500_UPDATE,(const uint8 *)post_msg,sizeof(post_msg));//������֤			
		  delay_ms(3000);
		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)		
    {
            len = recv(W5500_UPDATE, (uint8*)Buffer, len); //��������
			   //   printf("%s\r\n",Buffer);			   
				    mid((char*)Buffer,"Content-Length: ","\r\n",sub);//��ȡ�ַ�������
				    p=strstr((char*)Buffer,"\r\n\r\n");
				    tmplen =len-( p-(char*)Buffer)-4;//��һ�����ڵ����ݳ���
				    content_len=ATOI32(sub,10);

			      printf("\r\ncontent_len:%ld\r\n",content_len);
				    while(rxlen!=content_len)
				   { 						
              if(rxlen ==0)
              {
								printf("first pakge\r\n");
								printf("download \r\n");
							  FLASH_Unlock();
                FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);//������־λ
								n_pages = FLASH_PagesMask(content_len);
                for(n_erased = 0; n_erased < n_pages; n_erased++)
                {
									//printf("����flash\r\n");
                  FLASH_ErasePage(AppBackupAddress + 0x400 * n_erased);//����flash������
                }
								
								if((tmplen%2)!=0)//���������������
								{
								  flg=1;
									tail=Buffer[len-1];//�������һ�ֽڵ�����
								}
								
								for(i=0;i<tmplen-1;i=i+2) //����д��
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
									if(flg==1)//�ж��ϸ����Ƿ�������
									{
									  tmplen=recv(W5500_UPDATE, (uint8*)(Buffer+1),tmplen);
										
										Buffer[0]=tail;//ƴ������
									  //	flg=0;
										if(((tmplen+1)%2)!=0)//���ֽ�����������flg��λ��ȡ�����һ���ֽ�
									  {
									    flg=1;
									    tail=Buffer[tmplen];
										  //printf("tail:%d\r\n",tail);
											data_len = tmplen+1;//���ֽ���
									  }else//���ֽ���Ϊż��
										{
										  flg=0;//�����־λ
											tail=0;
											data_len = tmplen+1;
										}	
									}
									else
								  {	
                    //�ϸ���Ϊż��������										
					          tmplen=recv(W5500_UPDATE, (uint8*)Buffer,tmplen);
										if((tmplen%2)!=0)//�������ݰ�Ϊ������
									  {
									    flg=1;//��־λ��λ
									    tail=Buffer[tmplen-1];//�������һ������
									  }
										data_len =tmplen;
									}
									if((rxlen+tmplen)==content_len)//�ж��Ƿ�Ϊ���һ����
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
				 ConfigMsg.state=NEW_APP_IN_BACK;// �ı�״̬
				 write_config_to_flash();
			   update_flag=2;
				 delay_ms(1000);  //wait for a while
         reboot();        //����boot	       		
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
*@���ܣ�  ���ַ���ת��λ������
*@������  ��
 @����ֵ��������
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



























