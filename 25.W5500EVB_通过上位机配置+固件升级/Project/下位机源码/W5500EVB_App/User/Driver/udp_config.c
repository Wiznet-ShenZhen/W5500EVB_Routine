#include "udp_config.h"

 uint8 sckState;
 uint16 len;
 uint8 rIP[4];     // 远程UDP的IP
 uint16 rPort;     // 远程UDP的端口 
 uint8 bIP[4]={255,255,255,255};
 
void do_udp_config(void)
{
 
  //clear RecvMsg
  memset(&RecvMsg,0,sizeof(RecvMsg));
  
  sckState=getSn_SR(SOCK_CONFIG);
	
	
  switch(sckState)
  {
  case SOCK_UDP:
    if((len=getSn_RX_RSR(SOCK_CONFIG))>0)
    {
			
      if(len>MAX_BUF_SIZE) len=MAX_BUF_SIZE;
      recvfrom(SOCK_CONFIG, (uint8 *)&RecvMsg, len, rIP, &rPort);
      //FIND: searching, SETT: setting, FIRM: firmware uploading
	    
      if((RecvMsg.op[0]=='F') && (RecvMsg.op[1]=='I') && (RecvMsg.op[2]=='N') && (RecvMsg.op[3]=='D'))//FIND??,????????????
      {
        RecvMsg = ConfigMsg;
        RecvMsg.op[0]='F';RecvMsg.op[1]='I';RecvMsg.op[2]='N';RecvMsg.op[3]='D';
       
        //response
        sendto(SOCK_CONFIG, ( uint8 *)&RecvMsg, sizeof(RecvMsg), bIP, rPort);
        printf("Find\r\n");
      }
      else if((RecvMsg.op[0]=='S') && (RecvMsg.op[1]=='E') && (RecvMsg.op[2]=='T') && (RecvMsg.op[3]=='T'))//SETT??,????????????
      {
				printf("SEET\r\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {					
          ConfigMsg = RecvMsg;//获取的配置信息复制到配置结构体
          write_config_to_flash();//将配置信息写入到Flash
          reboot();//软件复位
        }
      }
      else if((RecvMsg.op[0]=='F') && (RecvMsg.op[1]=='I') && (RecvMsg.op[2]=='R') && (RecvMsg.op[3]=='M'))//FIRM??,?????,??????
      {
				printf("FIRM\r\n");
        //printf("firmware upload\n");
        if((RecvMsg.mac[0]==ConfigMsg.mac[0]) && (RecvMsg.mac[1]==ConfigMsg.mac[1]) && (RecvMsg.mac[2]==ConfigMsg.mac[2]) && 
           (RecvMsg.mac[3]==ConfigMsg.mac[3]) && (RecvMsg.mac[4]==ConfigMsg.mac[4]) && (RecvMsg.mac[5]==ConfigMsg.mac[5]))
        {    
          //response
          sendto(SOCK_CONFIG, (uint8 *)&RecvMsg, sizeof(ConfigMsg),bIP, rPort);
        }
      }
    }
    break;
  case SOCK_CLOSED:
		
    socket(SOCK_CONFIG, Sn_MR_UDP, UDP_PORT, 0);
    break;
  }
}
