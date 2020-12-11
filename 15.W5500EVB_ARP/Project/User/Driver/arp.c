#include "arp.h"
#include "ult.h"
#include "w5500.h"
#include "socket.h"
#include "sockutil.h"
#include <string.h>

extern uint8 pub_buf[1024];
extern uint8 tcp_conn_flag;

uint8 tx_arp_flag=1;
/*
* target_ip_addr[4]:需要知道远程设备的IP地址
*/
uint8 target_ip_addr[4]={192,168,1,101};

#define SOCK_ARP 0

uint8* pMACRAW;

ARPMSG pARPMSG;
ARPMSG * aARPMSG;

uint8 ret_arp_reply[128];

/*
 *功能：响应arp请求
 *参数：无
 *返回值：无
 ****************************/

void do_arp(void)
{
  uint16 rlen =0;
  SOCKET s = SOCK_ARP;
  uint16 aPort=5000;

  switch(getSn_SR(s))
  {
    case SOCK_CLOSED:
      close(s);                                  // close the SOCKET
      socket(s,Sn_MR_MACRAW,aPort,0);            // open the SOCKET with MACRAW mode
			break;    
    case SOCK_MACRAW:
      if(tx_arp_flag==1)
      {
				tx_arp_flag=0;
        arp_request(s, aPort, ConfigMsg.lip, ConfigMsg.mac, target_ip_addr);				
      }
      if((rlen = getSn_RX_RSR(s)) > 0)
			{
				arp_reply(s, rlen);
      }
      break;
  } 
}

void arp_request(SOCKET s, uint16 port, uint8 *SrcIp, uint8 *SrcMac, uint8 *TgtIp)
{
  uint32 tip = 0xFFFFFFFF;
  uint16 i =0;
  
  for( i=0; i<6 ; i++) 
	{
    pARPMSG.dst_mac[i] = 0xFF;//BROADCAST_MAC[i];
    pARPMSG.src_mac[i] = SrcMac[i];//BROADCAST_MAC[i];
    pARPMSG.sender_mac[i] = SrcMac[i];//BROADCAST_MAC[i];
    pARPMSG.tgt_mac[i] = 0x00;	
  }
  pARPMSG.sender_mac[5]=0x89;  
  pARPMSG.msg_type = htons(ARP_TYPE);
  pARPMSG.hw_type   = htons(ETHER_TYPE);
  pARPMSG.pro_type  = htons(PRO_TYPE);                  // IP	(0x0800)
  pARPMSG.hw_size    =  HW_SIZE;		        // 6
  pARPMSG.pro_size   = PRO_SIZE;		        // 4
  pARPMSG.opcode      =  htons(ARP_REQUEST);		// request (0x0001), reply(0x0002)
	
  for( i=0; i<4 ; i++) 
	{
    pARPMSG.sender_ip[i] = SrcIp[i];//BROADCAST_MAC[i];
    pARPMSG.tgt_ip[i] = TgtIp[i];
  } 
  if( sendto(s,(uint8*)&pARPMSG,sizeof(pARPMSG),(uint8 *)&tip,port) ==0)
	{
    printf( "\r\n Fail to send ping-reply packet  \r\n") ;				
  }
	else
	{
    if(pARPMSG.opcode==htons(ARP_REQUEST))
		{
      printf("\r\nWho has ");
      printf( "%.d.%.d.%.d.%.d ? ", pARPMSG.tgt_ip[0], pARPMSG.tgt_ip[1], pARPMSG.tgt_ip[2], pARPMSG.tgt_ip[3]) ;
      printf( "  Tell %.d.%.d.%.d.%.d\r\n", (pARPMSG.sender_ip[0]) , (pARPMSG.sender_ip[1]), (pARPMSG.sender_ip[2]), (pARPMSG.sender_ip[3]));							
    }
		else
		{
      printf("opcode has wrong value. check opcode !\r\n");
    }
  }

}

void arp_reply(SOCKET s, uint16 rlen)
{
  uint16 mac_destport;  
  uint8 mac_destip[4];

  recvfrom(s,(uint8 *)pub_buf,rlen,mac_destip,&mac_destport);

	if( pub_buf[12]==ARP_TYPE_HI && pub_buf[13]==ARP_TYPE_LO ){
  aARPMSG =(ARPMSG*) pub_buf;	
  if((aARPMSG->opcode) == htons(ARP_REPLY)) {

    memset(ret_arp_reply,0x00,128);
    sprintf((int8*)ret_arp_reply,"%.3u.%.3u.%.3u.%.3u is at %.2x.%.2x.%.2x.%.2x.%.2x.%.2x",
                          (aARPMSG->sender_ip[0]),  (aARPMSG->sender_ip[1]),(aARPMSG->sender_ip[2]),  (aARPMSG->sender_ip[3]),
                          (aARPMSG->sender_mac[0]),  (aARPMSG->sender_mac[1]),  (aARPMSG->sender_mac[2]),
                          (aARPMSG->sender_mac[3]), (aARPMSG->sender_mac[4]),  (aARPMSG->sender_mac[5]));
    
    printf( "%.u.%.u.%.u.%.u ", 
             (aARPMSG->sender_ip[0]),  (aARPMSG->sender_ip[1]), 
             (aARPMSG->sender_ip[2]),  (aARPMSG->sender_ip[3])) ;
    printf(" is at  ");
    printf( "%.2x.%.2x.%.2x.%.2x.%.2x.%.2x \r\b", 
             (aARPMSG->sender_mac[0]),  (aARPMSG->sender_mac[1]),  (aARPMSG->sender_mac[2]), 
             (aARPMSG->sender_mac[3]), (aARPMSG->sender_mac[4]),  (aARPMSG->sender_mac[5])) ;
    
  
    
  }else if((aARPMSG->opcode) == htons(ARP_REQUEST)) {

    printf( "Who has %.3u.%.3u.%.3u.%.3u ? ", 
             (aARPMSG->tgt_ip[0]),  (aARPMSG->tgt_ip[1]), 
             (aARPMSG->tgt_ip[2]),  (aARPMSG->tgt_ip[3])) ;
    printf(" Tell  ");
    printf( "%.2x.%.2x.%.2x.%.2x.%.2x.%.2x \r\b",  
             (aARPMSG->sender_mac[0]),  (aARPMSG->sender_mac[1]),  (aARPMSG->sender_mac[2]), 
             (aARPMSG->sender_mac[3]), (aARPMSG->sender_mac[4]),  (aARPMSG->sender_mac[5])) ;

  }else{
    printf(" This msg is not ARP reply : opcode is not 0x02 \n");	
  }
  }
}


