#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"
#include "at_command.h"
#include "util.h"
#include "usart.h"
#include "seg.h"
#include "device.h"
#include "24c16.h"
#include "socket.h"
#include "w5500.h"
#include "dhcp.h"
#include "timer.h"
#include "tsensor.h"
char str[]="?";
DATE_TIME Data_Time;

uint8_t select_sid;
uint8_t config_sid;
uint8_t get_result_sid;
char 	input_temp[128];
uint8_t change_config_flag=0;// 0 没改 1 改
char get_com_seting(char* src, uint8 * value);
void list_printf(void)
{
  uint8 i; 
	static const char *AT_LIST[128]={
	"AT",
	"AT+IP","AT+SN",
	"AT+PRE","AT+DNS","AT+MAC","AT+VER",
	"AT+EXIT","AT+LIST","AT+ECHO","AT+TYPE","AT+NAME","AT+PASS","AT+MARK","AT+COM1","AT+TEMP",
	"AT+DNSEN","AT+RESET","AT+C1_IT","AT+C1_OP",
	"AT+NETRCV","AT+DOMAIN",
	"AT+DEFAULT","AT+NETSEND","AT+NETBIOS",	"AT+RUNTIME","AT+GATEWAY","AT+IP_MODE","AT+C1_PORT","AT+C1_BIND","AT+C1_BAUD",
	"AT+WEB_PORT","AT+C1_DATAB","AT+C1_STOPB","AT+C1_SER_C","AT+C1_SER_T","AT+C1_TCPAT",
	"AT+RECONTIME","AT+C1_PARITY","AT+C1_LINK_P","AT+C1_LINK_M","AT+C1_LINK_T",
	"AT+C1_RCV_NUM","AT+C1_BUF_CLS","AT+C1_SER_LEN","AT+C1_CL1_PP1","AT+C1_CL1_IP1","AT+DEBUGMSGEN","AT+START_MODE",
	"AT+C1_SEND_NUM",NULL
	};
	for(i=0;i<50;i++)
	{	
		if(AT_LIST[i]==NULL) break;
		if(i%3==0)
		{
				printf("%-18s\r\n",AT_LIST[i]);
		}
		else 
				printf("%-18s",AT_LIST[i]);
	}
	printf("\r\n");	
	printf("OK\r\n");	
}
/*****************************************************
//AT+<WIZNET>&MAC 配置mac地址 不开放
//AT+<WIZNET>&DSN=160401-010203
at_type
//  AT命令 输入方式限定 为 
1 字符一个个输入  然后输入回车(回车前没字符）
2 直接1条带回车的命令 
3 多条带回车的命令 最后一条也带回车
****************************************************/

uint8_t at_type=1;
uint8_t print_get_flag=1;
char temp_buff[128]={0};
uint8_t at_input_phase(void)
{
	uint16 i=0,len;
	uint8_t get_one_at=0;
  uint8_t revalue=1;
	char *tempstr;
	memset(temp_buff,0,128);
	memset(input_temp,0,128);

	if(nagle_time_count>10)// 输入间隔时间 >10ms 认为暂时没有命令输入再进行处理
	{
		//IWDG_ReloadCounter();//feed iwdg 
		if(get_enter_count==0 && at_getindex>0)
		{
			if(at_printfindex!=at_getindex)
			{
				at_printfindex++;//收到的没打印
        
				if(ConfigMsg.echo_flag==1) printf("%c",at_rx_buf[at_printfindex-1] );// 1 按字符输入
				if(at_rx_buf[at_printfindex-1]==0x08)//输入 删除
				{
					if(at_printfindex>1) //接收字节大于0
					{
						at_printfindex=at_printfindex-2;
						at_rx_buf[at_printfindex]=0;
						at_getindex=at_getindex-2;
					}
				}
				if(at_printfindex > AT_BUF_LEN-1) at_printfindex=0;
			}
			at_type=1;
		}
		else if(get_enter_count==1 && at_type!=3) //收到1个回车(且不是 多条输入的 最后一条）
		{
			
			if(at_getindex>2)
			{
				if(at_printfindex<at_getindex-2) at_type=2;// 同时有字符输入和回车
			  get_one_at=1;
				while(at_readindex != at_getindex-2)//还未到
				{
					temp_buff[i++]=at_rx_buf[at_readindex++];
					if(at_readindex==AT_BUF_LEN) at_readindex=0;
					if(i>120) i=120; //AT命令和数据 不能超过120 超过部分会被截断
					IWDG_ReloadCounter();//feed iwdg 
				}	
				memcpy(input_temp,temp_buff,strlen(temp_buff)); //拷贝已获取的
				
				if(ConfigMsg.echo_flag==1)//需要打印
				{
					while(at_printfindex < at_getindex)
					{
						at_printfindex++;
						printf("%c",at_rx_buf[at_printfindex-1]);	
						if(at_printfindex > AT_BUF_LEN-1) at_printfindex=0;
						IWDG_ReloadCounter();//feed iwdg 
					}
				}
			}
			else if(at_getindex==2)
			{
				if(ConfigMsg.echo_flag==1) printf("\r\n");//只有回车
			}
			//清除数据
      get_enter_count=0;
   		memset(at_rx_buf,0,at_getindex);
      at_getindex=0;
		  at_readindex=0;
			at_printfindex=0;
		}
		else if(get_enter_count>1) at_type=3;
		
		if(at_type==3 && get_enter_count>0)// 3 收到多个回车
		{
			while(1)
			{
				//IWDG_ReloadCounter();//feed iwdg 
				if(at_rx_buf[at_readindex]==0x0d && at_rx_buf[at_readindex+1]==0x0a) //要读的字符是回车
				{
					get_enter_count--;//回车--
					at_rx_buf[at_readindex]=0;
					at_rx_buf[at_readindex+1]=0;
					at_readindex=at_readindex+2;
					if(at_readindex>AT_BUF_LEN-1) at_readindex=0;
          if(at_readindex==at_getindex) //整条命令读取完毕
					{
						memset(at_rx_buf,0,at_getindex);
						at_getindex=0;
						at_readindex=0;
						at_type=0;
					}
					
					if(i>0) //不是回车
					{
						get_one_at=1;
						if(strlen(temp_buff)<127)
						{
							memcpy(input_temp,temp_buff,strlen(temp_buff)); //拷贝已获取的
							if(ConfigMsg.echo_flag==1)printf("%s\r\n",temp_buff);//打开回显
						}
					}
					else if(ConfigMsg.echo_flag==1)	printf("\r\n");//只有回车
					
					break;
				}
				else
				{
				 temp_buff[i++]=at_rx_buf[at_readindex++];
				 if(i>120) i=120; //120个认为异常
			  }
				
				if(at_readindex>=at_getindex) //异常情况处理
				{
					memset(at_rx_buf,0,at_getindex);
					at_getindex=0;
					at_readindex=0;
					at_type=0;	
					break;
				}	
		
		  }
		}
	}
  
	if(get_one_at==1)
	{
	  get_one_at=0;
//	memset(temp_buff,0,sizeof(temp_buff));
		at_to_AT(temp_buff);							//将输入命令转化为大写
		if(memcmp(temp_buff,"AT",2)==0)
		{											
			//at_len=sprintf(temp_buff,"%s",temp_buff);	//判断AT命令的长度
			//判断是信息配置命令还是信息获取命令	
			if(strstr(temp_buff,"=")!=NULL)					
			{
				select_sid=1;
				change_config_flag=1; 
				tempstr=str_delim(0,"=",temp_buff);					
				len=sprintf(tempstr,"%s",tempstr);
				if(len!=0)		
				{			
					switch(len)
					{
						case 5:
							if(strcmp(tempstr,"AT+IP")==0)							config_sid=0;
							else config_sid=100;
						break;
						case 6:
							if(strcmp(tempstr,"AT+DNS")==0)							config_sid=1;			
							else config_sid=100;
						break;
						case 7:	
							if(strcmp(tempstr,"AT+ECHO")==0)			    	config_sid=5;	
							else if(strcmp(tempstr,"AT+COM1")==0)				config_sid=7;							
							else if(strcmp(tempstr,"AT+NAME")==0)				config_sid=8;	
							else if(strcmp(tempstr,"AT+PASS")==0)				config_sid=9;
							else if(strcmp(tempstr,"AT+MARK")==0)				config_sid=10;
							else config_sid=100;
						break;
						case 8:
							if(strcmp(tempstr,"AT+DNSEN")==0)						config_sid=13;	//配置登录信息
							else if(strcmp(tempstr,"AT+RESET")==0)			config_sid=14;  //复位
							else if(strcmp(tempstr,"AT+C1_OP")==0)			config_sid=15;		
							else if(strcmp(tempstr,"AT+C1_IT")==0)			config_sid=18;						
							else config_sid=100;
						break;
						case 9:
							if(strcmp(tempstr,"AT+DOMAIN")==0)		     	config_sid=20;
							else config_sid=100;			
						break;
						case 10:
							if(strcmp(tempstr,"AT+DEFAULT")==0)					config_sid=21;	
							else if(strcmp(tempstr,"AT+GATEWAY")==0)		config_sid=24;
						  else if(strcmp(tempstr,"AT+IP_MODE")==0)		config_sid=25;
							else if(strcmp(tempstr,"AT+C1_PORT")==0)		config_sid=26;
							else if(strcmp(tempstr,"AT+C1_BAUD")==0)		config_sid=27;
						  else if(strcmp(tempstr,"AT+NETBIOS")==0)		config_sid=50;	
              else if(strcmp(tempstr,"AT+C1_BIND")==0)		config_sid=51;						
							else config_sid=100;			
						break;
						case 11:
							if(strcmp(tempstr,"AT+WEB_PORT")==0)	    config_sid=29;		
							else if(strcmp(tempstr,"AT+C1_DATAB")==0)	config_sid=30;	
							else if(strcmp(tempstr,"AT+C1_STOPB")==0)	config_sid=31;
							else if(strcmp(tempstr,"AT+C1_SER_C")==0)	config_sid=32;
							else if(strcmp(tempstr,"AT+C1_SER_T")==0)	config_sid=33;	
							else if(strcmp(tempstr,"AT+C1_TCPAT")==0)	config_sid=34;
							else config_sid=100;			
						break;
						case 12:
							if(strcmp(tempstr,"AT+C1_PARITY")==0)	      config_sid=36;
							else if(strcmp(tempstr,"AT+RECONTIME")==0)	config_sid=37;	
							else if(strcmp(tempstr,"AT+C1_LINK_P")==0)	config_sid=38;
							else if(strcmp(tempstr,"AT+C1_LINK_M")==0)	config_sid=39;
							else if(strcmp(tempstr,"AT+C1_LINK_T")==0)	config_sid=40;				
							else config_sid=100;				
						break;
						case 13:
							if(strcmp(tempstr,"AT+C1_BUF_CLS")==0)		    config_sid=43;
							else if(strcmp(tempstr,"AT+C1_SER_LEN")==0)		config_sid=44;
							else if(strcmp(tempstr,"AT+C1_CLI_PP1")==0)		config_sid=46;	
							else if(strcmp(tempstr,"AT+C1_CLI_IP1")==0)		config_sid=47;
						  else if(strcmp(tempstr,"AT+DEBUGMSGEN")==0)		config_sid=49;	
							else if(strcmp(tempstr,"AT+START_MODE")==0)		config_sid=6;	
							else config_sid=100;
						break;
						case 15:
				      if(strcmp(tempstr,"AT+<WIZNET>&MAC")==0)				config_sid=3;
							else if(strcmp(tempstr,"AT+<WIZNET>&DSN")==0)		config_sid=4;
							else config_sid=100;
						break;
						default:
							config_sid=100;
						break;
					}
					memset(tempstr,0,len);
					if(config_sid==100)
					{						
						printf("Command Invalid\r\nERROR\r\n");
						return 0;
					}
				}		
			}
			//信息获取命令
			else 
			{
				select_sid=2;
				tempstr=temp_buff;
				len=sprintf(tempstr,"%s",tempstr);	
				if(len!=0)		
				{			
					switch(len)
					{
						case 2:
							if(strcmp(tempstr,"AT")==0)									get_result_sid=0;
							else get_result_sid=100;
						break;
						case 6:
							if(strcmp(tempstr,"AT+IP?")==0)							get_result_sid=1;
						  else if(strcmp(tempstr,"AT+SN?")==0)				get_result_sid=55;
							else get_result_sid=100;
						break;
						case 7:
							if(strcmp(tempstr,"AT+MAC?")==0)						get_result_sid=2;
							else if(strcmp(tempstr,"AT+DNS?")==0)				get_result_sid=3;
							else if(strcmp(tempstr,"AT+VER?")==0)				get_result_sid=4;	
              else if(strcmp(tempstr,"AT+PRE?")==0)				get_result_sid=56;							
							else if(strcmp(tempstr,"AT+EXIT")==0)				get_result_sid=5;		
							else get_result_sid=100;
						break;
						case 8:							
							if(strcmp(tempstr,"AT+ECHO?")==0)			      get_result_sid=7;	
							else if(strcmp(tempstr,"AT+LIST?")==0)			get_result_sid=8;
							else if(strcmp(tempstr,"AT+TYPE?")==0)			get_result_sid=9;
							else if(strcmp(tempstr,"AT+NAME?")==0)			get_result_sid=10;	
							else if(strcmp(tempstr,"AT+PASS?")==0)			get_result_sid=11;
							else if(strcmp(tempstr,"AT+MARK?")==0)			get_result_sid=12;
							else if(strcmp(tempstr,"AT+COM1?")==0)	  	get_result_sid=14;
						  else if(strcmp(tempstr,"AT+TEMP?")==0)	    get_result_sid=52;														
							else get_result_sid=100;
						break;
						case 9:
							if(strcmp(tempstr,"AT+DNSEN?")==0)					get_result_sid=15;	
							else if(strcmp(tempstr,"AT+C1_OP?")==0)			get_result_sid=17;	
							else if(strcmp(tempstr,"AT+C1_IT?")==0)			get_result_sid=20;							
							else get_result_sid=100;
						break;
						case 10:
							if(strcmp(tempstr,"AT+NETRCV?")==0)					get_result_sid=21;
							else if(strcmp(tempstr,"AT+DOMAIN?")==0)		get_result_sid=22;
			
							else get_result_sid=100;			
						break;
						case 11:
							if(strcmp(tempstr,"AT+NETSEND?")==0)	    	get_result_sid=24;
							else if(strcmp(tempstr,"AT+RUNTIME?")==0)		get_result_sid=25;
							else if(strcmp(tempstr,"AT+GATEWAY?")==0)		get_result_sid=26;	
							else if(strcmp(tempstr,"AT+IP_MODE?")==0)		get_result_sid=27;
							else if(strcmp(tempstr,"AT+C1_PORT?")==0)		get_result_sid=28;
							else if(strcmp(tempstr,"AT+C1_BAUD?")==0)		get_result_sid=29;
							else if(strcmp(tempstr,"AT+C1_BIND?")==0)		get_result_sid=53;
              else if(strcmp(tempstr,"AT+NETBIOS?")==0)		get_result_sid=54;				
							else get_result_sid=100;			
						break;
						case 12:
							if(strcmp(tempstr,"AT+WEB_PORT?")==0)				get_result_sid=31;		
							else if(strcmp(tempstr,"AT+C1_DATAB?")==0)	get_result_sid=32;	
							else if(strcmp(tempstr,"AT+C1_STOPB?")==0)	get_result_sid=33;
							else if(strcmp(tempstr,"AT+C1_SER_C?")==0)	get_result_sid=34;
							else if(strcmp(tempstr,"AT+C1_SER_T?")==0)	get_result_sid=35;	
							else if(strcmp(tempstr,"AT+C1_TCPAT?")==0)	get_result_sid=36;
							else get_result_sid=100;			
						break;
						case 13:	
							if(strcmp(tempstr,"AT+C1_PARITY?")==0)	get_result_sid=38;	
							else if(strcmp(tempstr,"AT+RECONTIME?")==0)	get_result_sid=39;
							else if(strcmp(tempstr,"AT+C1_LINK_P?")==0)	get_result_sid=40;
							else if(strcmp(tempstr,"AT+C1_LINK_M?")==0)	get_result_sid=41;
							else if(strcmp(tempstr,"AT+C1_LINK_T?")==0)	get_result_sid=42;			
							else get_result_sid=100;				
						break;	
						case 14:
							if(strcmp(tempstr,"AT+C1_RCV_NUM?")==0)					get_result_sid=44;
							else if(strcmp(tempstr,"AT+C1_BUF_CLS?")==0)		get_result_sid=45;
							else if(strcmp(tempstr,"AT+C1_SER_LEN?")==0)		get_result_sid=46;	
							else if(strcmp(tempstr,"AT+C1_CLI_PP1?")==0)		get_result_sid=48;
							else if(strcmp(tempstr,"AT+C1_CLI_IP1?")==0)		get_result_sid=49;
						  else if(strcmp(tempstr,"AT+DEBUGMSGEN?")==0)		get_result_sid=51;
					  	else if(strcmp(tempstr,"AT+START_MODE?")==0)		get_result_sid=13;						
							else get_result_sid=100;				
						break;
						case 15:
							if(strcmp(tempstr,"AT+C1_SEND_NUM?")==0)				get_result_sid=50;
							else get_result_sid=100;				
						break;			
						default:
							get_result_sid=100;
						break;
					}
					if(get_result_sid==100)
					{
						printf("Command Invalid\r\nERROR\r\n");
						return 0;
					}
					memset(tempstr,0,len+1);
				}
			}
		}
		else 
		{
			printf("Command Invalid\r\nERROR\r\n");
			revalue= 0 ;
		}

	}
	else 
	{	
		return 0;
	}
	
	return revalue;
}

void process_at(void)
{
	int num_command;
  uint8_t i,len;
	uint8_t tempvalue[5]; 
 char *config_content={0};
	//printf("select_sid=%d,config_sid=%d\r\nOK\r\n",select_sid,config_sid);	
	switch(select_sid)
	{
		case 1:
				switch(config_sid)
				{
					case 0:			//AT+IP=XX.XX.XX.XX 命令，配置IP地址
						config_content=str_delim(1,"=",input_temp);
						if(!VerifyIPAddress(config_content,tempvalue))
							printf("<Error Info>\r\n");
						else
						{
							ConfigMsg.net.ip[0]=tempvalue[0];
							ConfigMsg.net.ip[1]=tempvalue[1];
							ConfigMsg.net.ip[2]=tempvalue[2];
							ConfigMsg.net.ip[3]=tempvalue[3];
							printf("[IP] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.ip[0],ConfigMsg.net.ip[1],ConfigMsg.net.ip[2],ConfigMsg.net.ip[3]);															
						}
							break;
					case 1:				//AT+DNS=XX.XX.XX.XX命令，配置DNS	
						config_content=str_delim(1,"=",input_temp);
						if(!VerifyIPAddress(config_content,tempvalue))
							printf("<Error Info>\r\n");	
						else
						{							
							ConfigMsg.net.dns[0]=tempvalue[0];
							ConfigMsg.net.dns[1]=tempvalue[1];
							ConfigMsg.net.dns[2]=tempvalue[2];
							ConfigMsg.net.dns[3]=tempvalue[3];
							printf("[DNS] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.dns[0],ConfigMsg.net.dns[1],ConfigMsg.net.dns[2],ConfigMsg.net.dns[3]);								
						}
							break;
					
					case 3:		//AT+<WIZNET>&MAC=0008DC010203命令，配置mac地址	 不开放	  先输入
							config_content=str_delim(1,"=",input_temp);
							if(strlen(config_content)!=12)
								printf("<Error Info>\r\n");
							else
							{		
								
								phase_mac(config_content,(char *)ConfigMsg.mac); //更新MAC地址
								printf("[MAC] Value is:%02X:%02X:%02X:%02X:%02X:%02X\r\nOK\r\n",ConfigMsg.mac[0],ConfigMsg.mac[1],ConfigMsg.mac[2],ConfigMsg.mac[3],ConfigMsg.mac[4],ConfigMsg.mac[5]);								
//								set_default(); 
//								write_config_to_eeprom();
							}					 	
					break;
					
					case 4:		//AT+<WIZNET>&DSN= 命令，配置时间+MAC后三字节 不开放 字符串输入 AT+<WIZNET>&DSN=160401-010203字符数目必须为13个       
						config_content=str_delim(1,"=",input_temp);
							if(strlen(config_content)!=13)
							printf("<Error Info>\r\n");
							else
							{
					      memset(ConfigMsg.device_id,0,16);
							  memcpy(ConfigMsg.device_id,config_content,13);							
								printf("[SN] Value is:%s\r\nOK\r\n",ConfigMsg.device_id);
								set_default();
								write_config_to_eeprom();
							}		 	
					break;
					case 	5:			//AT+ECHO=命令，回显
						num_command=get_input_value(input_temp);
						if((num_command==0)||(num_command==1))
						{
						 ConfigMsg.echo_flag=num_command;						
							printf("[ECHO] Value is:%d\r\nOK\r\n",ConfigMsg.echo_flag);
						}
						else
							printf("<Error Info>\r\n");
					break;
					case 	6:			//AT+START_MODE=命令，配置开机模式
						num_command=get_input_value(input_temp);
						if((num_command==0)||(num_command==1))
						{
						 ConfigMsg.start_mode=num_command;						
						 printf("[START_MODE] Value is:%d\r\nOK\r\n",ConfigMsg.start_mode);
						}
						else
							printf("<Error Info>\r\n");
					break;
					case 	7:			//AT+COM1=命令，配置串口 默认 9,1,0,1,0 波特率，数据位，校验位，停止位，流控
				
					 config_content=str_delim(1,"=",input_temp);
				  	if(!get_com_seting(config_content,tempvalue))
								printf("<Error Info>\r\n");
						else
						{							
							if( tempvalue[0]>15 || tempvalue[1]>1 || tempvalue[2]>2 || tempvalue[3]>3 || tempvalue[4]>1)			   
							printf("<Error Info>\r\n");	
							else
							{
								  ConfigMsg.sec1.baudrate=tempvalue[0];							
									ConfigMsg.sec1.databit=tempvalue[1];								
									ConfigMsg.sec1.parity=tempvalue[2];								
									ConfigMsg.sec1.stopbit=tempvalue[3];								
									ConfigMsg.sec1.flowcontrol=tempvalue[4];									
						  }
							printf("[COM1] Value is:%d,%d,%d,%d,%d\r\n",ConfigMsg.sec1.baudrate,
							ConfigMsg.sec1.databit, ConfigMsg.sec1.parity, ConfigMsg.sec1.stopbit,ConfigMsg.sec1.flowcontrol);
							
							printf("[baudrate]   :%d\r\n",select_band(ConfigMsg.sec1.baudrate));
		
							printf("[databit]    :");
							if(ConfigMsg.sec1.databit==0) printf("7\r\n");
							else printf("8\r\n");
							
							printf("[parity]     :");
							if(ConfigMsg.sec1.parity==0)      printf("NON\r\n");
							else if(ConfigMsg.sec1.parity==1) printf("ODD\r\n");
							else if(ConfigMsg.sec1.parity==2) printf("EVEN\r\n");
							
							printf("[stopbit]    :");
							if(ConfigMsg.sec1.stopbit==0)      printf("0.5\r\n");
							else if(ConfigMsg.sec1.stopbit==1) printf("1\r\n");
							else if(ConfigMsg.sec1.stopbit==2) printf("1.5\r\n");
							else if(ConfigMsg.sec1.stopbit==3) printf("2\r\n");
							
							printf("[flowcontrol]:");
							if(ConfigMsg.sec1.flowcontrol==0) printf("NON\r\n");
							else printf("CTS/RTS\r\n");
							
							printf("OK\r\n");	
						}
					break;

					case	8:			//AT+NAME=XX命令，配置设备名称
							config_content=str_delim(1,"=",input_temp);
							if(config_content==NULL || strlen(config_content)>15 || strlen(config_content)==0)
							printf("<Error Info>\r\n");
							else
							{
					      memset(ConfigMsg.device_name,0,16);
							  memcpy(ConfigMsg.device_name,config_content,strlen(config_content));							
								printf("[NAME] Value is:%s\r\nOK\r\n",ConfigMsg.device_name);
							}
					break;
					case  9:			//AT+PASS=   设置设备密码
							 config_content=str_delim(1,"=",input_temp);
							if(config_content==NULL||strlen(config_content)>15 || strlen(config_content)==0)
							 printf("<Error Info>\r\n");
							else
							{	
							 len=strlen(config_content);
							 memset(ConfigMsg.pwd,0,16);
							 for(i=0;i<len;i++)
							{
								if( ((config_content[i]>'0'-1) && (config_content[i]<'9'+1)) ||
									  ((config_content[i]>'a'-1) && (config_content[i]<'z'+1)) ||
								    ((config_content[i]>'A'-1) && (config_content[i]<'Z'+1))
									)
								ConfigMsg.pwd[i]=config_content[i];
								else
								{
								  printf("<Error Info>\r\n");
									break;
								}
							}
							if(i==len) printf("[PASS] Value is:%s\r\nOK\r\n",ConfigMsg.pwd);
							}
					break;
					case 10:			//AT+MARK=XX.XX.XX.XX命令，子网掩码配置
							config_content=str_delim(1,"=",input_temp);
							if(!VerifyIPAddress(config_content,tempvalue))
								printf("<Error Info>\r\n");
							else
							{
								ConfigMsg.net.sub[0]=tempvalue[0];
								ConfigMsg.net.sub[1]=tempvalue[1];
								ConfigMsg.net.sub[2]=tempvalue[2];
								ConfigMsg.net.sub[3]=tempvalue[3];
								printf("[MARK] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.sub[0],ConfigMsg.net.sub[1],ConfigMsg.net.sub[2],ConfigMsg.net.sub[3]);							 
					    }
					break;
					case	13:			//AT+DNSEN=1 使用DNS解析到的域名
						num_command=get_input_value(input_temp);
						if((num_command==0)||(num_command==1))
						{
							ConfigMsg.sec1.dns_flag=num_command;
							printf("[DNSEN] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.dns_flag);
							if(ConfigMsg.sec1.dns_flag==1 && ConfigMsg.sec1.domain!=NULL) //DNS
							rip_get_flag=0;
						  else rip_get_flag=1;
						}
						else
							printf("<Error Info>\r\n");
					break;
					case	14:			//AT+RESET=PASSWORD命令，重启设备
						config_content=str_delim(1,"=",input_temp);
						if( strlen((char*)config_content)==strlen((char*)ConfigMsg.pwd) && strstr(config_content,(char *)ConfigMsg.pwd))
						{
								printf("OK\r\n");
								write_config_to_eeprom();
								reboot();
						}
						else
							printf("Password Error\r\nERROR\r\n");
					break;
					case 	15:			//AT+C1_OP=XX命令，工作模式 0-TCP Server，1-TCP Client, 2-UDP 
								num_command=get_input_value(input_temp);
								if((num_command==0)||(num_command==1)||(num_command==2))
								{
									ConfigMsg.sec1.mode=num_command;									
									printf("[C1_OP] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.mode);
								}	
								else								
									printf("<Error Info>\r\n");
					break;
					case 	18:			//AT+C1_IT=XXXX,超时断开时间，
							num_command=get_input_value(input_temp);
							if(num_command<0||num_command>60000)	printf("<Error Info>\r\n");	
							else
							{								
								ConfigMsg.sec1.inactivity=num_command;							
								printf("[C1_IT] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.inactivity);
							}
						
					break;
					case	20:			//AT+DOMAIN="www.xxx.xx",域名解析
							config_content=str_delim(1,"=",input_temp);
							//at_AT(config_content,config_content);
					    if(strlen(config_content)>31 || strlen(config_content)==0)
							 printf("<Error Info>\r\n");	
							else
							{
								memset(ConfigMsg.sec1.domain,0,32);
								memcpy(ConfigMsg.sec1.domain,config_content,strlen(config_content));
								printf("[DOMAIN] Value is:%s\r\nOK\r\n",ConfigMsg.sec1.domain);
							}
					break;
					case	21:			//AT+DEFAULT=PASSWORD命令，回恢复出厂设置
						config_content=str_delim(1,"=",input_temp);
					  if( strlen((char*)config_content)==strlen((char*)ConfigMsg.pwd) && strstr(config_content,(char *)ConfigMsg.pwd))
						{
								printf("OK\r\n");
								set_default();
								write_config_to_eeprom();
      					reboot();
//							  reset_w5500();
//							  set_network();
						}
						else
							printf("Password Error\r\nERROR\r\n");

					break;
					case	24:			//AT+GATEWAY=XX.XX.XX.XX,配置网关
							config_content=str_delim(1,"=",input_temp);
							if(!VerifyIPAddress(config_content,tempvalue))
								printf("<Error Info>\r\n");
							else 						
							{
								ConfigMsg.net.gw[0]=tempvalue[0];
								ConfigMsg.net.gw[1]=tempvalue[1];
								ConfigMsg.net.gw[2]=tempvalue[2];
								ConfigMsg.net.gw[3]=tempvalue[3];
									printf("[GATEWAY] Value is:%d.%d.%d.%d\r\nOK\r\n",
								ConfigMsg.net.gw[0],ConfigMsg.net.gw[1],ConfigMsg.net.gw[2],ConfigMsg.net.gw[3]);
							}
					break;
					case	25:			//AT+IP_MODE=XX命令，配置获取IP方式
							num_command=get_input_value(input_temp);
							if((num_command==0)||(num_command==1))
							{								
								ConfigMsg.net.dhcp=num_command;								
								printf("[IP_MODE] Value is:%d\r\nOK\r\n",ConfigMsg.net.dhcp);																
							}	
							else							
								printf("<Error Info>\r\n");			
					break;
					case 	26:			//AT+C1_PORT=XXX命令,配置工作端口
							num_command=get_input_value(input_temp);
							if(num_command<0||num_command>65535)
								printf("<Error Info>\r\n");	
							else
							{
								ConfigMsg.sec1.lport=num_command;								
								printf("[C1_PORT] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.lport);																
							}
					break;
					case	27:			//AT+C1_BAUD=XXX命令，配置波特率
							num_command=get_input_value(input_temp);
							if((num_command>=0)&&(num_command<=15))
							{
								ConfigMsg.sec1.baudrate=num_command;							
								printf("[C1_BAUD] Value is:%d\r\n",ConfigMsg.sec1.baudrate);															
								printf("OK\r\n");
							}
							else
								printf("<Error Info>\r\n");	
					break;						
					case 	29:			//AT+WEB_PORT=xx命令，配置网页端口
								num_command=get_input_value(input_temp);
								if(num_command<0||num_command>65535)	
							   printf("<Error Info>\r\n");
								else
								{
									ConfigMsg.http_port=num_command;							
									printf("[WEB_PORT] Value is:%d\r\nOK\r\n",ConfigMsg.http_port);																		
								}
					break;
					case	30:			//AT+C1_DATAB=XX命令，配置数据位
								num_command=get_input_value(input_temp);
								if((num_command==0)||(num_command==1))	
								{
									ConfigMsg.sec1.databit=num_command;								
									printf("[C1_DATAB] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.databit);																	
								}
								else
									printf("<Error Info>\r\n");
					break;
					case 	31:			//AT+C1_STOPB=XX命令，配置停止位
								num_command=get_input_value(input_temp);
								if((num_command>=0)&&(num_command<4))	
								{
									ConfigMsg.sec1.stopbit=num_command;								
									printf("[C1_STOPB] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.stopbit);																
								}
								else
									printf("<Error Info>\r\n");
					break;
					case	32:			//AT+C1_SER_C=XX命令，配置串口流控
								num_command=get_input_value(input_temp);
								if((num_command==0)||(num_command==1))	
								{
									ConfigMsg.sec1.flowcontrol=num_command;							
									printf("[C1_SER_C] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.flowcontrol);															
								}
								else
									printf("<Error Info>\r\n");
					break;
					case	33:			//AT+C1_SER_T=XX命令，配置串口帧间隔
								num_command=get_input_value(input_temp);
								if(num_command<0||num_command>60000)
									printf("<Error Info>\r\n");	
								else
								{
									ConfigMsg.sec1.nagle_time=num_command;								
									printf("[C1_SER_T] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.nagle_time);													
								}
					break;
					case 	34:			//AT+C1_TCPAT=XX命令，配置心跳检测时间 单位5s
								num_command=get_input_value(input_temp);
									if(num_command<0||num_command>255)
									printf("<Error Info>\r\n");	
								else
								{
									ConfigMsg.sec1.keepalive_time=num_command;								
									printf("[C1_TCPAT] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.keepalive_time);						
								}
					break;

					case 	36:			//AT+C1_PARITY=XX命令，配置检验位
								num_command=get_input_value(input_temp);
								if((num_command==0)||(num_command==1)||(num_command==2))	
								{
									ConfigMsg.sec1.parity=num_command;								
									printf("[C1_PARITY] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.parity);																
								}
								else
									printf("<Error Info>\r\n");
					break;
								
					case 37:			//AT+RECONTIME=XX命令，配置TCP可重连时间
						           //AT+RECONTIME=XX命令
								num_command=get_input_value(input_temp);
								if(num_command<0||num_command>60000)
									printf("<Error Info>\r\n");	
								else
								{
									ConfigMsg.sec1.reconn=num_command;								
									printf("[AT+RECONTIME] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.reconn);													
								}
					break;
								
					case	38:			//AT+C1_LINK_P=XX命令   连接密码校验
								num_command=get_input_value(input_temp);
								if(num_command==0 ||num_command==1)
								{
									ConfigMsg.sec1.link_password=num_command;							
									printf("[C1_LINK_P] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_password);									
								}								
								else
								printf("<Error Info>\r\n");	

					break;
					case 	39:			//AT+C1_LINK_M=XX,设置连接后是否发送信息
									num_command=get_input_value(input_temp);
									if(num_command== 0||num_command==1||num_command==2||num_command==3)
									{
										ConfigMsg.sec1.link_message=num_command;
										printf("[C1_LINK_M] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_message);
									}
									else printf("<Error Info>\r\n");
					
					break;
					case	40:			//AT+C1_LINK_T=XX，配置连接条件	
								num_command=get_input_value(input_temp);
								if(num_command==0||num_command==1)
								{					
									ConfigMsg.sec1.link_condition=num_command;									
									printf("[C1_LINK_T] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_condition);								
								}
								else
									printf("<Error Info>\r\n");	
					break;
					case	43:			//AT+C1_BUF_CLS命令，清空串口buffer
								num_command=get_input_value(input_temp);
						
								if(num_command==0||num_command==1)
								{					
									ConfigMsg.sec1.link_clearbuf=num_command;									
									printf("[C1_BUF_CLS] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_clearbuf);														
								}	
							else
									printf("<Error Info>\r\n");	
					break;
					case 	44:			//AT+C1_SER_LEN命令，设置封包长度
								num_command=get_input_value(input_temp);
								if(num_command<0||num_command>2048)
									printf("<Error Info>\r\n");	
								else
								{
									ConfigMsg.sec1.pack_len=num_command;								
									printf("[C1_SER_LEN] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.pack_len);													
								}
					break;
					case	46:			//AT+C1_CLI_PP1=XX命令，配置目标端口1
							num_command=get_input_value(input_temp);
							if(num_command<0||num_command>65535)
								printf("<Error Info>\r\n");	
							else
							{
								  ConfigMsg.sec1.rport=num_command;							
									printf("[C1_CLI_PP1] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.rport);
							}
					break;
					case 	47:			//AT+C1_CLI_IP1=XX.XX.XX.XX或者"www.xx.xx"命令，设置目标IP1
							config_content=str_delim(1,"=",input_temp);
							if(!VerifyIPAddress(config_content,tempvalue))
								printf("<Error Info>\r\n");
							else
							{
								ConfigMsg.sec1.rip[0]=tempvalue[0];
								ConfigMsg.sec1.rip[1]=tempvalue[1];
								ConfigMsg.sec1.rip[2]=tempvalue[2];
								ConfigMsg.sec1.rip[3]=tempvalue[3];
								printf("[C1_CLI_IP1] Value is:%d.%d.%d.%d\r\nOK\r\n",
								ConfigMsg.sec1.rip[0],ConfigMsg.sec1.rip[1],ConfigMsg.sec1.rip[2],ConfigMsg.sec1.rip[3]);									
							}
					break;	
					case	49:			//AT+DEBUGMSGEN=XX命令  调试信息设置
							num_command=get_input_value(input_temp);
							if(num_command<0||num_command>1)
								printf("<Error Info>\r\n");	
							else
							{
								  ConfigMsg.debug_flag=num_command;							
									printf("[DEBUGMSGEN] Value is:%d\r\nOK\r\n",ConfigMsg.debug_flag);
							}
					break;		
					case  50:			//AT+NETBIOS
						 num_command=get_input_value(input_temp);
						if((num_command==0)||(num_command==1))
						{
							ConfigMsg.netbios_flag=num_command;						
							printf("[NETBIOS] Value is:%d\r\nOK\r\n",ConfigMsg.netbios_flag);
						}
						else
							printf("<Error Info>\r\n");
					break;
					
					case  51:			//AT+C1_BIND
						 num_command=get_input_value(input_temp);
						if((num_command==0)||(num_command==1))
						{
							ConfigMsg.sec1.bind_port=num_command;						
							printf("[C1_BIND] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.bind_port);
						}
						else
							printf("<Error Info>\r\n");
					break;	
					case 100:
							printf("AT command error!\r\n");
					break;			
					default:
					break;
				}
		break;			
		case 2:					//第2种命令模式，读取相关配置信息
				switch(get_result_sid)
				{
						case 0:				//AT命令
								printf("OK\r\n");
						break;
						case 1:				//AT+IP?命令，读取IP地址
								printf("[IP] Value is:%d.%d.%d.%d\r\nOK\r\n"
						,ConfigMsg.net.ip[0],ConfigMsg.net.ip[1],ConfigMsg.net.ip[2],ConfigMsg.net.ip[3]);							
						break;
						case 2:				//AT+MAC?命令，读取MAC地址
								printf("[MAC] Value is:%02X:%02X:%02X:%02X:%02X:%02X\r\nOK\r\n",
						ConfigMsg.mac[0],ConfigMsg.mac[1],ConfigMsg.mac[2],ConfigMsg.mac[3],ConfigMsg.mac[4],ConfigMsg.mac[5]);						
						break;
						case 3:				//AT+DNS?命令，读取域名解析服务器IP地址
								printf("[DNS] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.dns[0],ConfigMsg.net.dns[1],ConfigMsg.net.dns[2],ConfigMsg.net.dns[3]);
						break;
						case 4:				//AT+VER?命令，读取版本号
							  printf("[VER] Value is:V%d.%d\r\nOK\r\n",ConfigMsg.sw_ver[0],ConfigMsg.sw_ver[1]);								
						break;
						case 5:			//AT+EXIT命令，退出
						 printf("OK\r\n");
							if(change_config_flag==1)//全部为查询命令时不插除EEPROM
							{
								write_config_to_eeprom();
							  USART2_Init(ConfigMsg.sec1.baudrate,ConfigMsg.sec1.databit,ConfigMsg.sec1.stopbit,ConfigMsg.sec1.parity,ConfigMsg.sec1.flowcontrol);
							  reset_w5500();//重启W5500
							  set_network();//设置网络参数
							}	
	            reset_state_flag();							
  						WorkState.run_mode=0;//回到数据模式				    
						break;
						case 6:
						break;
						case 7:		//AT+ECHO?命令，判断是否为回显状态
								printf("[ECHO] Value is:%d\r\nOK\r\n",ConfigMsg.echo_flag);
						break;
						case 	8:			//AT+LIST?命令，读取命令列表
								list_printf();
						break;
						case	9:			//AT+TYPE?命令，读取设备类型
 								printf("[TYPE] Value is:%s\r\nOK\r\n",ConfigMsg.device_type);
						break;
						case	10:			//AT+NAME?命令，读取设备名称
								printf("[NAME] Value is:%s\r\nOK\r\n",ConfigMsg.device_name);
						break;
						case  11:			//AT+PASS?命令，读取设备密码
								printf("[PASS] Value is:%s\r\nOK\r\n",ConfigMsg.pwd);
						break;
						case	12:			//AT+MARK?命令，读取子网掩码
								printf("[MARK] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.sub[0],ConfigMsg.net.sub[1],ConfigMsg.net.sub[2],ConfigMsg.net.sub[3]);
						break;
						case	13:			//AT+START_MODE?命令,启动模式
							  printf("[START_MODE] Value is:%d\r\nOK\r\n",ConfigMsg.start_mode);
						break;
						case 	14:			//AT+COM1? 命令，读取串口参数
							printf("[COM1] Value is:%d,%d,%d,%d,%d\r\n",ConfigMsg.sec1.baudrate,
							ConfigMsg.sec1.databit, ConfigMsg.sec1.parity, ConfigMsg.sec1.stopbit,ConfigMsg.sec1.flowcontrol);
							printf("[baudrate]   :%d\r\n",select_band(ConfigMsg.sec1.baudrate));
		
							printf("[databit]    :");
							if(ConfigMsg.sec1.databit==0) printf("7\r\n");
							else printf("8\r\n");
							
							printf("[parity]     :");
							if(ConfigMsg.sec1.parity==0)      printf("NON\r\n");
							else if(ConfigMsg.sec1.parity==1) printf("ODD\r\n");
							else if(ConfigMsg.sec1.parity==2) printf("EVEN\r\n");
							
							printf("[stopbit]    :");
							if(ConfigMsg.sec1.stopbit==0)      printf("0.5\r\n");
							else if(ConfigMsg.sec1.stopbit==1) printf("1\r\n");
							else if(ConfigMsg.sec1.stopbit==2) printf("1.5\r\n");
							else if(ConfigMsg.sec1.stopbit==3) printf("2\r\n");
							
							printf("[flowcontrol]:");
							if(ConfigMsg.sec1.flowcontrol==0) printf("NON\r\n");
							else printf("YES\r\n");
							
						break;
						case	15:			//AT+DNSEN?
							printf("[DNSEN] Value is:%d\r\n",ConfigMsg.sec1.dns_flag);
						break;
						case	16:			//AT+RESET?重启设备		注：只写——不需要读
							//reboot();
						break;
						case 	17:			//AT+C1_OP?命令，读取工作模式 0-TCP Server，1-TCP Client, 2-TCP MIX , 3-UDP, 4-Disable
								printf("[C1_OP] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.mode);
						break;

						case 	20:			//AT+C1_IT?命令，读取超时断开时间
								printf("[C1_IT] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.inactivity);
						break;
						case	21:			//AT+NETRCV?命令，读取以太网收包数
								printf("[NETRCV] Value is:%d\r\nOK\r\n",WorkState.netrcv_num);
						break;
						case 	22:		//AT+DOMAIN?命令，读取要解析域名	
								printf("[DOMAIN] Value is:%s\r\nOK\r\n",ConfigMsg.sec1.domain);
						break;
						case	23:			//AT+DEFAULT?命令，回恢复出厂设置		注：只写——不需要读
						break;
						case	24:			//AT+NETSEND?命令？读取以太网发包数  
								printf("[NETSEND] Value is:%d\r\nOK\r\n",WorkState.netsend_num);
						break;
						case 	25:			//AT+RUNTIME?命令，读取运行时间  
								get_work_time(WorkState.on_time);
								printf("[RUNTIME] Value is:%03d-%02d-%02d-%02d\r\nOK\r\n",Data_Time.day,Data_Time.hour,Data_Time.min,Data_Time.sec);
						break;
						case	26:			//AT+GATEWAY?命令，读取网关地址
								printf("[GATEWAY] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.net.gw[0],ConfigMsg.net.gw[1],ConfigMsg.net.gw[2],ConfigMsg.net.gw[3]);
						break;
						case	27:			//AT+IP_MODE?命令，读取配置的获取IP方式值
								printf("[IP_MODE] Value is:%d\r\nOK\r\n",ConfigMsg.net.dhcp);
						break;
						case 	28:			//AT+C1_PORT?命令，读取工作端口
								printf("[C1_PORT] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.lport);
						break;
						case	29:			//AT+C1_BAUD?命令，读取波特率
								printf("[C1_BAUD] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.baudrate);
						break;
						case	30:		
						break;
						case 	31:			//AT+WEB_PORT?命令，读取网页端口
								printf("[WEB_PORT] Value is:%d\r\nOK\r\n",ConfigMsg.http_port);
						break;
						case	32:			//AT+C1_DATAB?命令，读取数据位
								printf("[C1_DATAB] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.databit);
						break;
						case 	33:			//AT+C1_STOPB?命令，读取停止位
								printf("[C1_STOPB] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.stopbit);
						break;
						case	34:			//AT+C1_SER_C?命令，串口流控
								printf("[C1_SER_C] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.flowcontrol);
						break;
						case	35:			//AT+C1_SER_T?命令，串口帧间隔
								printf("[C1_SER_T] Value is:%2d\r\nOK\r\n",ConfigMsg.sec1.nagle_time);
						break;
						case 	36:			//AT+C1_TCPAT?命令，读取心跳检测时间
								printf("[C1_TCPAT] Value is:%2d\r\nOK\r\n",ConfigMsg.sec1.keepalive_time);
						break;
						case	37:			//AT+TCPSTATUS?命令，读取TCP状态
//								printf("[TCPSTATUS] Value is:");
//								print_socket_status(0);
//								printf("\r\nOK\r\n");	
						break;
						case 	38:			//AT+C1_PARITY?命令，读取检验位
								printf("[C1_PARITY] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.parity);
						break;
						case	39:			//AT+RECONTIME?命令，读取TCP Client断开后重连时间
  							printf("[RECONTIME] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.reconn);
						break;
						case	40:			//AT+C1_LINK_P?命令，读取连接密码校验
								printf("[C1_LINK_P] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_password);
						break;
						case 	41:			//AT+C1_link_message?命令，读取状态，是否连接后发送信息
								printf("[C1_link_message] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_message);							
						break;
						case	42:			//AT+C1_LINK_T?命令，读取连接条件
								printf("[C1_LINK_T] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_condition);
						break;
						//case 	43:			//AT+UDER_NAME
								//printf("[USRE_NAME] Value is:%s\r\nOK\r\n",ConfigMsg.username);
						//break;
						case	44:			//AT+C1_RCV_NUM?命令，读取串口接收字节数								
								printf("[C1_RCV_NUM] Value is:%d\r\nOK\r\n",WorkState.sec1_rcv_num);
						break;
						case	45:			//AT+C1_BUF_CLS?命令，清空串口buffer
								printf("[C1_BUF_CLS] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.link_clearbuf);
						break;
						case 	46:			//AT+C1_SER_LEN?
								printf("[C1_SER_LEN] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.pack_len);
						break;
						case	47:			//AT+C1_TCP_CLS?命令，硬件连接断开关闭tcp
//								printf("[C1_TCP_CLS] Value is:%d\r\nOK\r\n",ConfigMsg.hwdis_tcpcls);
						break;
						case	48:			//AT+C1_CL1_PP1?命令，读取目标端口1
								printf("[C1_CL1_PP1] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.rport);
						break;
						case 	49:			//AT+C1_CL1_IP1?命令，读取目标IP1
								printf("[C1_CLI_IP1] Value is:%d.%d.%d.%d\r\nOK\r\n",ConfigMsg.sec1.rip[0],ConfigMsg.sec1.rip[1],ConfigMsg.sec1.rip[2],ConfigMsg.sec1.rip[3]);
						break;		
						case	50:			//AT+C1_SEND_NUM?命令，读取串口发送字节数
								printf("[C1_SEND_NUM] Value is:%d\r\nOK\r\n",WorkState.sec1_send_num);
						break;
						case  51:			//AT+DEBUGMDGEN?命令，读取连接状态 
								printf("[DEBUGMDGEN] Value is:%d\r\nOK\r\n",ConfigMsg.debug_flag);	
						break;
						case  52:			//AT+TEMP?命令，读取mcu内部温度
							//WorkState.temperature=get_temperature();
						    WorkState.temperature=0;
								printf("[TEMP] Value is:%d\r\nOK\r\n",WorkState.temperature);
							break;
						case  53:			//AT+C1_BIND?命令
						  printf("[C1_BIND] Value is:%d\r\nOK\r\n",ConfigMsg.sec1.bind_port);
							break;
						case  54:			//AT+NETBIOS?命令
						  printf("[NETBIOS] Value is:%d\r\nOK\r\n",ConfigMsg.netbios_flag);
							break;
						case  55:			//AT+SN?命令
						  printf("[SN] Value is:%s\r\nOK\r\n",ConfigMsg.device_id);
							break;
					  case  56:			//AT+PRE?命令
						  printf("DEFAULT:\r\n");
							printf("[NAME]      :%s\r\n","W5500S2E");
							//printf("[USERNAME]  :%s\r\n",DEFAULT_USERNAME);
							printf("[PASS]      :%s\r\n",DEFAULT_PWD);
							printf("[DOMAIN]    :%s\r\n",DEFAULT_DOMAIN);
						  printf("[IP]        :%d.%d.%d.%d\r\n",DEFAULT_LIP[0],DEFAULT_LIP[1],DEFAULT_LIP[2],DEFAULT_LIP[3]);
							printf("[MARK]      :%d.%d.%d.%d\r\n",DEFAULT_MARK[0],DEFAULT_MARK[1],DEFAULT_MARK[2],DEFAULT_MARK[3]);
						  printf("[GATEWAY]   :%d.%d.%d.%d\r\n",DEFAULT_GW[0],DEFAULT_GW[1],DEFAULT_GW[2],DEFAULT_GW[3]);
							printf("[DNS]       :%d.%d.%d.%d\r\n",DEFAULT_DNS[0],DEFAULT_DNS[1],DEFAULT_DNS[2],DEFAULT_DNS[3]);
							printf("[MAC]       :%02X:%02X:%02X:%02X:%02X:%02X\r\n",DEFAULT_MAC[0],DEFAULT_MAC[1],
						DEFAULT_MAC[2],DEFAULT_MAC[3],DEFAULT_MAC[4],DEFAULT_MAC[5]);
							printf("[WEB_PORT]  :%d\r\n",DEFAULT_WEB_PORT);
							printf("[C1_PORT]   :%d\r\n",DEFAULT_LPORT);	
              printf("[C1_BAUD]   :%d\r\n",BR_115200_INDEX);	
							printf("[C1_DATAB]  :%d\r\n",DATABIT_8_INDEX);
							printf("[C1_STOPB]  :%d\r\n",STOPBIT_1_INDEX);
							printf("[C1_PARITY] :%d\r\n",PARITY_NONE);
							printf("[C1_SER_T]  :%d\r\n",DEFAULT_NAGLE_TIME);	
							printf("[C1_SER_LEN]:%d\r\n",DEFAULT_PACK_LEN);						
							printf("[C1_CLI_IP1]:%d.%d.%d.%d\r\n",
							DEFAULT_RIP[0],DEFAULT_RIP[1],DEFAULT_RIP[2],DEFAULT_RIP[3]);
              printf("[C1_CL1_PP1]:%d\r\n",DEFAULT_RPORT); 
						
						  printf("CURRENT:\r\n");
						 	printf("[NAME]      :%s\r\n",ConfigMsg.device_name);
//							printf("[USERNAME]  :%s\r\n",ConfigMsg.username);
							printf("[PASS]      :%s\r\n",ConfigMsg.pwd);
							printf("[DOMAIN]    :%s\r\n",ConfigMsg.sec1.domain);
						  printf("[IP]        :%d.%d.%d.%d\r\n",ConfigMsg.net.ip[0],ConfigMsg.net.ip[1],ConfigMsg.net.ip[2],ConfigMsg.net.ip[3]);
							printf("[MARK]      :%d.%d.%d.%d\r\n",ConfigMsg.net.sub[0],ConfigMsg.net.sub[1],ConfigMsg.net.sub[2],ConfigMsg.net.sub[3]);
						  printf("[GATEWAY]   :%d.%d.%d.%d\r\n",ConfigMsg.net.gw[0],ConfigMsg.net.gw[1],ConfigMsg.net.gw[2],ConfigMsg.net.gw[3]);
							printf("[DNS]       :%d.%d.%d.%d\r\n",ConfigMsg.net.dns[0],ConfigMsg.net.dns[1],ConfigMsg.net.dns[2],ConfigMsg.net.dns[3]);
							printf("[MAC]       :%02X:%02X:%02X:%02X:%02X:%02X\r\n",ConfigMsg.mac[0],ConfigMsg.mac[1],
						ConfigMsg.mac[2],ConfigMsg.mac[3],ConfigMsg.mac[4],ConfigMsg.mac[5]);
							printf("[WEB_PORT]  :%d\r\n",ConfigMsg.http_port);
							printf("[C1_PORT]   :%d\r\n",ConfigMsg.sec1.lport);	
              printf("[C1_BAUD]   :%d\r\n",ConfigMsg.sec1.baudrate);	
							printf("[C1_DATAB]  :%d\r\n",ConfigMsg.sec1.databit);
							printf("[C1_STOPB]  :%d\r\n",ConfigMsg.sec1.stopbit);
							printf("[C1_PARITY] :%d\r\n",ConfigMsg.sec1.parity);	
							printf("[C1_SER_T]  :%d\r\n",ConfigMsg.sec1.nagle_time);	
							printf("[C1_SER_LEN]:%d\r\n",ConfigMsg.sec1.pack_len);
							printf("[C1_CLI_IP1]:%d.%d.%d.%d\r\n",
								ConfigMsg.sec1.rip[0],ConfigMsg.sec1.rip[1],ConfigMsg.sec1.rip[2],ConfigMsg.sec1.rip[3]);
              printf("[C1_CL1_PP1]:%d\r\n",ConfigMsg.sec1.rport);

							printf("OK\r\n");
							break;
						case 100:
								printf("Command Invalid\r\nERROR\r\n\r\n");
						break;			
						default:
						break;
					}
		break;
		default:
		break;	
	}
}

///* 解析AT命令配置信息*/
int get_input_value(char buff[])
{
	uint32 num=0,i;
	char *buf;
	buf=str_delim(1,"=",buff);
  if(buf==NULL) return -1;	
	for(i=0;buf[i]!='\0';i++)
	{
		if(buf[i]<0x30||buf[i]>0x39)
			return -1;
		else 
		{
			num = num*10+buf[i]-0x30;
		}
		if(i>5)
			return -1;
		if(i>0&&(buf[0]=='0'))
			return -1;			
	}
	return num;	
}
/*在AT命令后面添加"?",生成新AT命令*/
char *test_strcat(char *buf)
{
	char str1[1024];
	char *str2;
	memset(str1,0,strlen(str1));	
	memcpy(str1,buf,strlen(buf));
	str2=strcat(str1,str);
	return str2;	
}
// 分割字符串 num=0  delim 前 的字符串
char *str_delim(uint8_t num,char *delim,char *temp)
{
	int i;
	char *str[2]={0};
	char *tok=temp;	
	char *restr;
	for(i=0;i<2;i++)
	{
		tok=strtok(tok,delim);
		str[i]=tok;
		tok = NULL;
	}
	restr=str[num];
	return restr;
}

void at_to_AT(char *buf)
{
	uint8 i;
	char *s=buf;
	for(i=0;s[i]!='\0';i++)
	{
		if((s[i]>='a')&&(s[i]<='z'))
			buf[i]=s[i]-32;
		else
			buf[i]=s[i];
	}
}

void phase_mac(char *src,	char dest[])
{
	uint8 i,j;
	uint8 buf[12];
  for( i = 0; i < 12; i++) buf[i] = src[i];
  for (i = 0, j = 0; i <6; i++)
  {
    dest[i] = get_hex(buf[j], buf[j+1]);
    j += 2;
  } 
}
//检验输入的版本号
char check_ver(char* src,	char *dest)
{
	int i;
	char *str;
	char tsrc[50];
	char* tok = tsrc;
	
	strcpy(tsrc,src);
	
	for(i = 0; i < 2; i++)
	{
		tok = strtok(tok,".");
		if ( !tok ) return 0;
		str=tok;
		dest[i]=hex2dec(str);
		tok = NULL;
	}
	return 1;	
}
//十六进制转十进制函数
int hex2dec(char *buf)
{
	int num,num1,num2;
	if((buf[0]>=48&&buf[0]<=57)||(buf[0]>=97&&buf[0]<=102)||(buf[0]>=65&&buf[0]<=70))
	{
		if(buf[0]>=48&&buf[0]<=57)
		{
			num1=buf[0]-48;
			num1=num1*16;
		}
		if(buf[0]>=97&&buf[0]<=102)
		{
			num1=buf[0]-87;
			num1=num1*16;
		}
		if(buf[0]>=65&&buf[0]<=70)
		{
			num1=buf[0]-55;
			num1=num1*16;
		}	
	}
	if((buf[1]>=48&&buf[1]<=57)||(buf[1]>=97&&buf[1]<=102)||(buf[1]>=65&&buf[1]<=70))
	{
		if(buf[1]>=48&&buf[1]<=57)
		{
			num2=buf[1]-48;
		}
		if(buf[1]>=97&&buf[1]<=102)
		{
			num2=buf[1]-87;
		}
		if(buf[1]>=65&&buf[1]<=70)
		{
			num2=buf[1]-55;
		}		
	}	
	num=num1+num2;
	return num;
}
	
int get_data_string(char *buff)
{
	int i;
  if(buff[0]=='0'   
		 &&( buff[1]=='x'|| buff[1]=='X')
     &&( ( (buff[2]>'0'-1)&&(buff[2]<'9'+1) )||( (buff[2]>'a'-1)&&(buff[2]<'z'+1) )||( (buff[2]>'A'-1)&&(buff[2]<'Z'+1) ) )
	   &&( ( (buff[3]>'0'-1)&&(buff[3]<'9'+1) )||( (buff[3]>'a'-1)&&(buff[3]<'z'+1) )||( (buff[3]>'3'-1)&&(buff[2]<'Z'+1) ) )
	)
	i=get_hex(buff[2], buff[3]);
	else  return -1;
	return i;
}
void get_work_time(uint32 time)
{
	uint32 s=0,m=0,h=0;
	s=time;
	if(s>59)
	{
		m=s/60;
		Data_Time.sec=s%60;
		if(m>59)
		{
			h=m/60;
			Data_Time.sec=m%60;
			if(h>23)
			{
				Data_Time.day=h/24;
				Data_Time.hour=h%24;
			}
			else Data_Time.hour=h;				
		}
		else	Data_Time.min=m;	

	}
	else Data_Time.sec=s;	
}

void reset_state_flag(void)
{
		/*******************一些状态值****************************/
  if(ConfigMsg.sec1.dns_flag==1 && ConfigMsg.sec1.domain!=NULL &&
		( ConfigMsg.sec1.mode == TCP_CLIENT_MODE || ConfigMsg.sec1.mode==UDP_MODE)) //DNS 使能 TCP_SERVER_MODE 失效 
	   rip_get_flag=0;
	else rip_get_flag=1;
	
	if(ConfigMsg.sec1.link_condition==1 && ConfigMsg.sec1.mode == TCP_CLIENT_MODE) //链接条件  收到数据才链接
		client_can_link=0;
	else client_can_link=1;
	
	if(ConfigMsg.sec1.link_password==1 && ConfigMsg.sec1.mode == TCP_SERVER_MODE) //Client 第一次链接时要输入密码
		link_pass_flag=0;
	else link_pass_flag=1;
	
	if(ConfigMsg.net.dhcp == DHCP_MODE )ip_ok_flag=0; //初始化ip_ok_flag
	else ip_ok_flag=1;
}
char get_com_seting(char* src, uint8 * value)
{
	int i,len=0;
	int tnum;
	char tsrc[32];
	char* tok = tsrc;
	
	strcpy(tsrc,src);
	
	for(i = 0; i < 5; i++)
	{	
		tok = strtok(tok,",");//strtok内部有 this指针 
    if ( !tok ) return 0;
		len+=strlen(tok);
		if(tok[0] == '0' && tok[1] == 'x')
		{
			if(!ValidATOI(tok+2,0x10,&tnum)) return 0;
		}
		else if(!ValidATOI(tok,10,&tnum)) return 0;

		value[i] = tnum;
		
		if(tnum < 0 || tnum > 255) return 0;
		tok = NULL;
	}
	if(src[len+3]=='.')return 0; //字符串还有','
	return 1;	
}

void switch_at_state(void)
{
	 if(connected_flag==0||((connected_flag==1)&&(_kbhit()>2)))
		{
			plus_wait_flag=0;
			plus_wait_count=0;
			plus_count=0;
			get_config();//read config data	 以保存在EEPROM的配置为准
			printf("AT-Command Interpreter ready\r\n");
			WorkState.sec1_rcv_num=WorkState.sec1_rcv_num-3;
			change_to_at=0;
			sio_rd=0;
			sio_wr=0;
			//close s2e data socket
			//close(SOCK_DATA);
			disconnect(SOCK_DATA);
			close(SOCK_DATA);
			b_connecting_printed=0;
			b_listening_printed=0;	//
			change_config_flag=0;// 清除之前的配置状态
			WorkState.run_mode=1;	//当前工作于AT命令模式	
		}
}
