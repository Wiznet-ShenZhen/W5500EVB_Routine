#include <stdio.h>
#include <string.h>

#include "config.h"

#include "device.h"

#include "socket.h"
#include "w5500.h"
#include "dhcp.h"
#include "sockutil.h"
#include "util.h"//delay

//#define _DHCP_DEBUG_

uint8*   DHCP_CHADDR      = EXTERN_DHCP_MAC;      // DHCP Client MAC address.  20180625 
uint8*   GET_SN_MASK      = EXTERN_DHCP_SN;      // Subnet mask received from the DHCP server
uint8*   GET_GW_IP        = EXTERN_DHCP_GW;     // Gateway ip address received from the DHCP server
uint8*   GET_DNS_IP       = EXTERN_DHCP_DNS;    // DNS server ip address received from the DHCP server
uint8*   GET_SIP          = EXTERN_DHCP_SIP;    // Local ip address received from the DHCP server
uint8    DHCP_SIP[4]      = {0,};      // DNS server ip address is discovered
uint8    DHCP_REAL_SIP[4] = {0,};      // For extract my DHCP server in a few DHCP servers
uint8    OLD_SIP[4];                   // Previous local ip address received from DHCP server

uint32_t dhcp_tick_next   = DHCP_WAIT_TIME ;//20180625

// Network information from DHCP Server  20180625
uint8_t OLD_allocated_ip[4]   = {0, };    // Previous IP address
uint8_t DHCP_allocated_ip[4]  = {0, };    // IP address from DHCP
uint8_t DHCP_allocated_gw[4]  = {0, };    // Gateway address from DHCP
uint8_t DHCP_allocated_sn[4]  = {0, };    // Subnet mask from DHCP
uint8_t DHCP_allocated_dns[4] = {0, };    // DNS address from DHCP

uint8_t HOST_NAME[] = DEVICE_TYPE; 

uint8  dhcp_state       = STATE_DHCP_READY;          // DHCP client status
uint8  dhcp_retry_count = 0;     // retry count
uint8  DHCP_timeout     = 0;     // DHCP Timeout flag
uint32  dhcp_lease_time;         // Leased time
uint32  dhcp_time       = 0;
uint32  next_dhcp_time  = 0;     // DHCP Time 1s
uint32  dhcp_tick_cnt   = 0;     // 1ms
uint8  DHCP_timer;

uint32  DHCP_XID        = DEFAULT_XID;				
uint8 EXTERN_DHCPBUF[1024];      
RIP_MSG*  pDHCPMSG = (RIP_MSG*)EXTERN_DHCPBUF;   // Pointer for the DHCP message 

/*
*********************************************************************************************************
*              default_ip_assign  
*
* Description: The default handler of ip assign first
* Arguments  : None
* Returns    : None
* Note       : 
*********************************************************************************************************
*/
void default_ip_assign(void)
{
	setSIPR(DHCP_allocated_ip);
  setSUBR(DHCP_allocated_sn);
  setGAR (DHCP_allocated_gw);	
}
/*
*********************************************************************************************************
*              default_ip_update  
*
* Description: The default handler of ip chaged
* Arguments  : None
* Returns    : None
* Note       : 
*********************************************************************************************************
*/
void default_ip_update(void)
{
	/* WIZchip Software Reset */
   setMR(MR_RST);
   getMR(); // for delay
   default_ip_assign();
   setSHAR(DHCP_CHADDR);
}
/*
*********************************************************************************************************
*              default_ip_conflict  
*
* Description: The default handler of ip chaged
* Arguments  : None
* Returns    : None
* Note       : 
*********************************************************************************************************
*/
void default_ip_conflict(void)
{
	// WIZchip Software Reset
	setMR(MR_RST);
	getMR(); // for delay
	setSHAR(DHCP_CHADDR);
}

void (*dhcp_ip_assign)(void)   = default_ip_assign;     /* handler to be called when the IP address from DHCP server is first assigned */
void (*dhcp_ip_update)(void)   = default_ip_update;     /* handler to be called when the IP address from DHCP server is updated */
void (*dhcp_ip_conflict)(void) = default_ip_conflict;   /* handler to be called when the IP address from DHCP server is conflict */

/*
*********************************************************************************************************
*            reset_DHCP_timeout  
*
* Description: reset timeout value and retry count
* Arguments  :
* Returns    : 
* Note       : 
*********************************************************************************************************
*/
void reset_DHCP_timeout(void)//20180625
{
	dhcp_time = 0;
	dhcp_tick_next = DHCP_WAIT_TIME;
	dhcp_retry_count = 0;
}
/*
*********************************************************************************************************
*              check_DHCP_timeout  
*
* Description: Check DHCP for timeout
* Arguments  : None
* Returns    : DHCP State
* Note       : 
*********************************************************************************************************
*/
uint8_t check_DHCP_timeout(void)//20180625
{
	uint8_t ret = DHCP_RUNNING;
	if (dhcp_retry_count < MAX_DHCP_RETRY) {
		if (dhcp_tick_next < dhcp_time) {
   
			switch ( dhcp_state ) {
				case STATE_DHCP_DISCOVER :
					#ifdef _DHCP_DEBUG_  
					printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
				  #endif
					send_DHCP_DISCOVER();
				  break;		
				case STATE_DHCP_REQUEST :
					#ifdef _DHCP_DEBUG_  
					printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");
				  #endif
					send_DHCP_REQUEST();
				  break;
				case STATE_DHCP_REREQUEST :
					#ifdef _DHCP_DEBUG_  
					printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");	
          #endif				
					send_DHCP_REQUEST();
				  break;
				default :
				break;
			}
			dhcp_time = 0;
			dhcp_tick_next = dhcp_time + DHCP_WAIT_TIME;
			dhcp_retry_count++;
		}
	} else { // timeout occurred

		switch(dhcp_state) {
			case STATE_DHCP_DISCOVER:
				dhcp_state = STATE_DHCP_READY;
				ret = DHCP_FAILED;
				break;
			case STATE_DHCP_REQUEST:
			case STATE_DHCP_REREQUEST:
				send_DHCP_DISCOVER();
			  dhcp_time = 0;		
				dhcp_state = STATE_DHCP_DISCOVER;
				break;
			default :
				break;
		}
		reset_DHCP_timeout();
	}
	return ret;
}
/*
*********************************************************************************************************
*              makeDHCPMSG  
*
* Description: make the common DHCP message
* Arguments  : None
* Returns    : None
* Note       : 
*********************************************************************************************************
*/
void makeDHCPMSG(void)
{
   uint8_t  bk_mac[6];
   uint8_t* ptmp;
   uint8_t  i;
	
   getSHAR(bk_mac);

	pDHCPMSG->op      = DHCP_BOOTREQUEST;
	pDHCPMSG->htype   = DHCP_HTYPE10MB;
	pDHCPMSG->hlen    = DHCP_HLENETHERNET;
	pDHCPMSG->hops    = DHCP_HOPS;
	ptmp              = (uint8_t*)(&pDHCPMSG->xid);
	*(ptmp+0)         = (uint8_t)((DHCP_XID & 0xFF000000) >> 24);
	*(ptmp+1)         = (uint8_t)((DHCP_XID & 0x00FF0000) >> 16);
  *(ptmp+2)         = (uint8_t)((DHCP_XID & 0x0000FF00) >>  8);
	*(ptmp+3)         = (uint8_t)((DHCP_XID & 0x000000FF) >>  0);   
	pDHCPMSG->secs    = DHCP_SECS;
	ptmp              = (uint8_t*)(&pDHCPMSG->flags);	
	*(ptmp+0)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0xFF00) >> 8);
	*(ptmp+1)         = (uint8_t)((DHCP_FLAGSBROADCAST & 0x00FF) >> 0);

	pDHCPMSG->ciaddr[0] = 0;
	pDHCPMSG->ciaddr[1] = 0;
	pDHCPMSG->ciaddr[2] = 0;
	pDHCPMSG->ciaddr[3] = 0;

	pDHCPMSG->yiaddr[0] = 0;
	pDHCPMSG->yiaddr[1] = 0;
	pDHCPMSG->yiaddr[2] = 0;
	pDHCPMSG->yiaddr[3] = 0;

	pDHCPMSG->siaddr[0] = 0;
	pDHCPMSG->siaddr[1] = 0;
	pDHCPMSG->siaddr[2] = 0;
	pDHCPMSG->siaddr[3] = 0;

	pDHCPMSG->giaddr[0] = 0;
	pDHCPMSG->giaddr[1] = 0;
	pDHCPMSG->giaddr[2] = 0;
	pDHCPMSG->giaddr[3] = 0;

	pDHCPMSG->chaddr[0] = DHCP_CHADDR[0];
	pDHCPMSG->chaddr[1] = DHCP_CHADDR[1];
	pDHCPMSG->chaddr[2] = DHCP_CHADDR[2];
	pDHCPMSG->chaddr[3] = DHCP_CHADDR[3];
	pDHCPMSG->chaddr[4] = DHCP_CHADDR[4];
	pDHCPMSG->chaddr[5] = DHCP_CHADDR[5];

	for (i = 6; i < 16; i++)  pDHCPMSG->chaddr[i] = 0;
	for (i = 0; i < 64; i++)  pDHCPMSG->sname[i]  = 0;
	for (i = 0; i < 128; i++) pDHCPMSG->file[i]   = 0;

	// MAGIC_COOKIE
	pDHCPMSG->OPT[0] = (uint8_t)((MAGIC_COOKIE & 0xFF000000) >> 24);
	pDHCPMSG->OPT[1] = (uint8_t)((MAGIC_COOKIE & 0x00FF0000) >> 16);
	pDHCPMSG->OPT[2] = (uint8_t)((MAGIC_COOKIE & 0x0000FF00) >>  8);
	pDHCPMSG->OPT[3] = (uint8_t) (MAGIC_COOKIE & 0x000000FF) >>  0;
}
/*
*********************************************************************************************************
*              SEND DHCP DISCOVER
*
* Description: This function sends DHCP DISCOVER message to DHCP server.
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void send_DHCP_DISCOVER(void)
{
	uint8_t ip[4];
	uint16_t k = 0;
  uint8_t host_name[18]; 
	
  makeDHCPMSG();
	
   k = 4;     // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
   
	// Option Request Param
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DISCOVER;
	
	// Client identifier
	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];
	
	// host name
	pDHCPMSG->OPT[k++] = hostName;
	sprintf((char*)host_name,"%.11s-%02X%02X%02X",DEVICE_TYPE,DHCP_CHADDR[3],DHCP_CHADDR[4],DHCP_CHADDR[5]); 
  	
  pDHCPMSG->OPT[k++] = strlen((char*)host_name);
  
  strcpy((char*)(&(pDHCPMSG->OPT[k])),(char*)host_name);
  
  k+=strlen((char*)host_name);		

	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x06;	// length of request
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = endOption;

	// send broadcasting packet
	ip[0] = 255;
	ip[1] = 255;
	ip[2] = 255;
	ip[3] = 255;

#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_DISCOVER\r\n");
#endif
	sendto(SOCK_DHCP, (uint8_t *)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}
/*
*********************************************************************************************************
*              SEND DHCP REQUEST
*
* Description: This function sends DHCP REQUEST message to DHCP server.
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void send_DHCP_REQUEST(void)
{
//	int i;
	uint8_t ip[4];
	uint16_t k = 0;
  uint8_t host_name[18]; 
	
  makeDHCPMSG();

   if(dhcp_state == STATE_DHCP_LEASED || dhcp_state == STATE_DHCP_REREQUEST)
   {
   	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
   	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);
   	pDHCPMSG->ciaddr[0] = DHCP_allocated_ip[0];
   	pDHCPMSG->ciaddr[1] = DHCP_allocated_ip[1];
   	pDHCPMSG->ciaddr[2] = DHCP_allocated_ip[2];
   	pDHCPMSG->ciaddr[3] = DHCP_allocated_ip[3];
   	ip[0] = DHCP_SIP[0];
   	ip[1] = DHCP_SIP[1];
   	ip[2] = DHCP_SIP[2];
   	ip[3] = DHCP_SIP[3];   	   	   	
   }
   else
   {
   	ip[0] = 255;
   	ip[1] = 255;
   	ip[2] = 255;
   	ip[3] = 255;   	   	   	
   }
   
   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
	
	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_REQUEST;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];

   if(ip[3] == 255)  // if(dchp_state == STATE_DHCP_LEASED || dchp_state == DHCP_REREQUEST_STATE)
   {
		pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
		pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];
	
		pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
		pDHCPMSG->OPT[k++] = 0x04;
		pDHCPMSG->OPT[k++] = DHCP_SIP[0];
		pDHCPMSG->OPT[k++] = DHCP_SIP[1];
		pDHCPMSG->OPT[k++] = DHCP_SIP[2];
		pDHCPMSG->OPT[k++] = DHCP_SIP[3];
	}

	// host name
	pDHCPMSG->OPT[k++] = hostName;
	
	sprintf((char*)host_name,"%.11s-%02X%02X%02X",DEVICE_TYPE,DHCP_CHADDR[3],DHCP_CHADDR[4],DHCP_CHADDR[5]);
  
  
  pDHCPMSG->OPT[k++] = (uint8)strlen((char*)host_name);
  
  strcpy((char*)(&(pDHCPMSG->OPT[k])),(char*)host_name);
 
  
  k+=(uint8)strlen((char*)host_name);			
	
	pDHCPMSG->OPT[k++] = dhcpParamRequest;
	pDHCPMSG->OPT[k++] = 0x08;
	pDHCPMSG->OPT[k++] = subnetMask;
	pDHCPMSG->OPT[k++] = routersOnSubnet;
	pDHCPMSG->OPT[k++] = dns;
	pDHCPMSG->OPT[k++] = domainName;
	pDHCPMSG->OPT[k++] = dhcpT1value;
	pDHCPMSG->OPT[k++] = dhcpT2value;
	pDHCPMSG->OPT[k++] = performRouterDiscovery;
	pDHCPMSG->OPT[k++] = staticRoute;
	pDHCPMSG->OPT[k++] = endOption;

#ifdef _DHCP_DEBUG_
	printf("> Send DHCP_REQUEST\r\n");
#endif
  sendto(SOCK_DHCP, (uint8_t *)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}
/*
*********************************************************************************************************
*              SEND DHCP RELEASE
*
* Description: This function sends DHCP RELEASE message to DHCP server.
* Arguments  : None.              
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void send_DHCP_DECLINE(void)
{
//	int i;
	uint8_t ip[4];
	uint16_t k = 0;
	
	makeDHCPMSG();

   k = 4;      // beacaue MAGIC_COOKIE already made by makeDHCPMSG()
   
	*((uint8_t*)(&pDHCPMSG->flags))   = ((DHCP_FLAGSUNICAST & 0xFF00)>> 8);
	*((uint8_t*)(&pDHCPMSG->flags)+1) = (DHCP_FLAGSUNICAST & 0x00FF);

	// Option Request Param.
	pDHCPMSG->OPT[k++] = dhcpMessageType;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_DECLINE;

	pDHCPMSG->OPT[k++] = dhcpClientIdentifier;
	pDHCPMSG->OPT[k++] = 0x07;
	pDHCPMSG->OPT[k++] = 0x01;
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[0];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[1];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[2];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[3];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[4];
	pDHCPMSG->OPT[k++] = DHCP_CHADDR[5];

	pDHCPMSG->OPT[k++] = dhcpRequestedIPaddr;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[0];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[1];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[2];
	pDHCPMSG->OPT[k++] = DHCP_allocated_ip[3];

	pDHCPMSG->OPT[k++] = dhcpServerIdentifier;
	pDHCPMSG->OPT[k++] = 0x04;
	pDHCPMSG->OPT[k++] = DHCP_SIP[0];
	pDHCPMSG->OPT[k++] = DHCP_SIP[1];
	pDHCPMSG->OPT[k++] = DHCP_SIP[2];
	pDHCPMSG->OPT[k++] = DHCP_SIP[3];

	pDHCPMSG->OPT[k++] = endOption;

	//send broadcasting packet
	ip[0] = 0xFF;
	ip[1] = 0xFF;
	ip[2] = 0xFF;
	ip[3] = 0xFF;

#ifdef _DHCP_DEBUG_
	printf("\r\n> Send DHCP_DECLINE\r\n");
#endif

	sendto(SOCK_DHCP, (uint8_t *)pDHCPMSG, sizeof(RIP_MSG), ip, DHCP_SERVER_PORT);
}
/*
*********************************************************************************************************
*              PARSE REPLY MSG
*
* Description: This function parses the reply message from DHCP server.
* Arguments  : None             
* Returns    : success - return type, fail - 0
* Note       : 
*********************************************************************************************************
*/
int8_t parseDHCPMSG(void)//20180625
{
	uint8_t svr_addr[6];
	uint16_t  svr_port;
	uint16_t len;

	uint8_t * p;
	uint8_t * e;
	uint8_t type;
	uint8_t opt_len;
   
  if((len = getSn_RX_RSR(SOCK_DHCP)) > 0)
   {		
   	  len = recvfrom(SOCK_DHCP, (uint8_t *)pDHCPMSG, len, svr_addr, &svr_port);
   #ifdef _DHCP_DEBUG_   
      printf("DHCP message : %d.%d.%d.%d(%d) %d received. \r\n",svr_addr[0],svr_addr[1],svr_addr[2], svr_addr[3],svr_port,len);
   #endif   
   }
   else return 0;
	if (svr_port == DHCP_SERVER_PORT) {
      // compare mac address
		if ( (pDHCPMSG->chaddr[0] != DHCP_CHADDR[0]) || (pDHCPMSG->chaddr[1] != DHCP_CHADDR[1]) ||
		     (pDHCPMSG->chaddr[2] != DHCP_CHADDR[2]) || (pDHCPMSG->chaddr[3] != DHCP_CHADDR[3]) ||
		     (pDHCPMSG->chaddr[4] != DHCP_CHADDR[4]) || (pDHCPMSG->chaddr[5] != DHCP_CHADDR[5])   )		
			    return 0;    
    type = 0;
		p = (uint8_t *)(&pDHCPMSG->op);//2
		p = p + 240;      // 240 = sizeof(RIP_MSG) + MAGIC_COOKIE size in RIP_MSG.opt - sizeof(RIP_MSG.opt)
		e = p + (len - 240);
		
		while ( p < e ) {
			switch ( *p ) {
   			case endOption :
   			   p = e;   // for break while(p < e)
   				break;
        case padOption :
   				p++;
   				break;
   			case dhcpMessageType :
   				p++;
   				p++;
   				type = *p++;
   				break;
   			case subnetMask :
   				p++;
   				p++;
   				DHCP_allocated_sn[0] = *p++;
   				DHCP_allocated_sn[1] = *p++;
   				DHCP_allocated_sn[2] = *p++;
   				DHCP_allocated_sn[3] = *p++;
   				break;
   			case routersOnSubnet :
   				p++;
   				opt_len = *p++;       
   				DHCP_allocated_gw[0] = *p++;
   				DHCP_allocated_gw[1] = *p++;
   				DHCP_allocated_gw[2] = *p++;
   				DHCP_allocated_gw[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dns :
   				p++;                  
   				opt_len = *p++;       
   				DHCP_allocated_dns[0] = *p++;
   				DHCP_allocated_dns[1] = *p++;
   				DHCP_allocated_dns[2] = *p++;
   				DHCP_allocated_dns[3] = *p++;
   				p = p + (opt_len - 4);
   				break;
   			case dhcpIPaddrLeaseTime :
   				p++;
   				opt_len = *p++;
   				dhcp_lease_time  = *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
   				dhcp_lease_time  = (dhcp_lease_time << 8) + *p++;
        #ifdef _DHCP_DEBUG_  
//               dhcp_lease_time = 10;
//				printf("dhcp_lease_time:%d\r\n",(int)dhcp_lease_time);
 				#endif
   				break;
   			case dhcpServerIdentifier :
   				p++;
   				opt_len = *p++;
   				DHCP_SIP[0] = *p++;
   				DHCP_SIP[1] = *p++;
   				DHCP_SIP[2] = *p++;
   				DHCP_SIP[3] = *p++;
   				break;
   			default :
   				p++;
   				opt_len = *p++;
   				p += opt_len;
   				break;
			} // switch
		} // while
	} // if
	return	type;
}
/*
*********************************************************************************************************
*              CHECK TIMEOUT
*
* Description: This function checks the timeout of DHCP in each state.
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void check_DHCP_Timeout(void)
{
  if (dhcp_retry_count < MAX_DHCP_RETRY) 
  {
    if (dhcp_time > next_dhcp_time) 
    {
      dhcp_time = 0;
      next_dhcp_time = dhcp_time + DHCP_WAIT_TIME;
      dhcp_retry_count++;
      switch ( dhcp_state ) 
      {
      case STATE_DHCP_DISCOVER :
  #ifdef _DHCP_DEBUG_			   
        printf("<<timeout>> state : STATE_DHCP_DISCOVER\r\n");
  #endif				
        send_DHCP_DISCOVER();
        break;
      case STATE_DHCP_REQUEST :
  #ifdef _DHCP_DEBUG_			   
        printf("<<timeout>> state : STATE_DHCP_REQUEST\r\n");
  #endif				
        send_DHCP_REQUEST();
        break;
      case STATE_DHCP_REREQUEST :
  #ifdef _DHCP_DEBUG_			   
        printf("<<timeout>> state : STATE_DHCP_REREQUEST\r\n");
  #endif				
        send_DHCP_REQUEST();
        break;
      default :
        break;
      }
    }
  } 
  else 
  {
    reset_DHCP_timeout();
    DHCP_timeout = 1;
    
    send_DHCP_DISCOVER();
    dhcp_state = STATE_DHCP_DISCOVER;
#ifdef _DHCP_DEBUG_		
    printf("timeout : state : STATE_DHCP_DISCOVER\r\n");
#endif		
  }
}

/*
*********************************************************************************************************
*              check_DHCP_leasedIP
*
* Description: check if a leased IP is valid
* Arguments  : None.
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
int8_t check_DHCP_leasedIP(void)
{
	uint8_t tmp;
	int32_t ret;

	//WIZchip RCR value changed for ARP Timeout count control
	tmp = getRCR();
	setRCR(0x03);
#ifdef _DHCP_DEBUG_
  printf("<Check the IP Conflict %d.%d.%d.%d: ",DHCP_allocated_ip[0],DHCP_allocated_ip[1],DHCP_allocated_ip[2],DHCP_allocated_ip[3]);
  #endif
	// IP conflict detection : ARP request - ARP reply
	// Broadcasting ARP Request for check the IP conflict using UDP sendto() function
	ret = sendto(SOCK_DHCP, (uint8_t *)"CHECK_IP_CONFLICT", 17, DHCP_allocated_ip, 5000);

	// RCR value restore
	setRCR(tmp);

	if(ret){
		// Received ARP reply or etc : IP address conflict occur, DHCP Failed
		send_DHCP_DECLINE();
		ret = dhcp_time;
		while((dhcp_time - ret) < 2) ;   // wait for 1s over; wait to complete to send DECLINE message;

		return 0;
	}
	else 
	{
			// UDP send Timeout occurred : allocated IP address is unique, DHCP Success
#ifdef _DHCP_DEBUG_
		printf("\r\n> Check leased IP - OK\r\n");
#endif
		return 1;		
	}
}	

/*
*********************************************************************************************************
*              DHCP_timer_handler
*
* Description: timer interrupt handler(For checking dhcp lease time).
* Arguments  : None.           
* Returns    : None.
* Note       : Increase 'my_time' each one second.
*********************************************************************************************************
*/
void DHCP_timer_handler(void)
{
  if(dhcp_tick_cnt++ > 1000)
  {
    dhcp_tick_cnt = 0;
    dhcp_time++;
  }
}

/*
*********************************************************************************************************
*              Init_dhcp_client
*
* Description: Initialize the DHCP client
* Arguments  : None.             
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
void init_dhcp_client(void)
{
  uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
  //uint8 ip_broadcast[4]={255,};
  uint8 ip_0[4]={0,};
  DHCP_XID = 0x12345678;
  memset(OLD_SIP,0,sizeof(OLD_SIP));
  memset(DHCP_SIP,0,sizeof(DHCP_SIP));
  memset(DHCP_REAL_SIP,0,sizeof(GET_SN_MASK));

  iinchip_init();
  
  setSHAR(ConfigMsg.mac);
  setSUBR(ip_0);
  setGAR(ip_0);
  setSIPR(ip_0);
  printf("MAC=%02x:%02x:%02x:%02x:%02x:%02x\r\n",DHCP_CHADDR[0],DHCP_CHADDR[1],DHCP_CHADDR[2],DHCP_CHADDR[3],DHCP_CHADDR[4],DHCP_CHADDR[5]);  
  sysinit(txsize, rxsize); 
  //clear ip setted flag

  dhcp_state = STATE_DHCP_READY;
#ifdef _DHCP_DEBUG
   printf("init_dhcp_client:%u\r\n",SOCK_DHCP);
#endif   
}
/*
*********************************************************************************************************
*              DHCP_run
*
* Description: Do the DHCP client
* Arguments  : None.             
* Returns    : None.
* Note       : 
*********************************************************************************************************
*/
uint8_t DHCP_run(void)//20180625
{
	uint8_t  type;
	uint8_t  ret;

	if(dhcp_state == STATE_DHCP_STOP) return DHCP_STOPPED;

	if(getSn_SR(SOCK_DHCP) != SOCK_UDP)
	   socket(SOCK_DHCP, Sn_MR_UDP, DHCP_CLIENT_PORT, 0x00);

	ret = DHCP_RUNNING;	
	type = parseDHCPMSG();
	
	switch ( dhcp_state ) {
	   case STATE_DHCP_READY :
         DHCP_allocated_ip[0] = 0;
         DHCP_allocated_ip[1] = 0;
         DHCP_allocated_ip[2] = 0;
         DHCP_allocated_ip[3] = 0;
		     
   		   send_DHCP_DISCOVER();
		     dhcp_time = 0;
   		   dhcp_state = STATE_DHCP_DISCOVER;
   		   break;
		 case STATE_DHCP_DISCOVER :
			   if (type == DHCP_OFFER)
					 {
#ifdef _DHCP_DEBUG_
				    printf("> Receive DHCP_OFFER\r\n");
#endif
            DHCP_allocated_ip[0] = pDHCPMSG->yiaddr[0];
            DHCP_allocated_ip[1] = pDHCPMSG->yiaddr[1];
            DHCP_allocated_ip[2] = pDHCPMSG->yiaddr[2];
            DHCP_allocated_ip[3] = pDHCPMSG->yiaddr[3];
            
				    send_DHCP_REQUEST();
						dhcp_time = 0;
				    dhcp_state = STATE_DHCP_REQUEST;
			    } 
				else 
						ret = check_DHCP_timeout();
        break;
		case STATE_DHCP_REQUEST :
			if (type == DHCP_ACK) 
				{
#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_ACK\r\n");
#endif
				  if (check_DHCP_leasedIP()) 
					{
					// Network info assignment from DHCP
						printf("W5500EVB ip:%d.%d.%d.%d\r\n",DHCP_allocated_ip[0],DHCP_allocated_ip[1],DHCP_allocated_ip[2],DHCP_allocated_ip[3]);
	          printf("W5500EVB sn:%d.%d.%d.%d\r\n",DHCP_allocated_sn[0],DHCP_allocated_sn[1],DHCP_allocated_sn[2],DHCP_allocated_sn[3]);
	          printf("W5500EVB gw:%d.%d.%d.%d\r\n",DHCP_allocated_gw[0],DHCP_allocated_gw[1],DHCP_allocated_gw[2],DHCP_allocated_gw[3]);
						dhcp_ip_assign();
						reset_DHCP_timeout();
						dhcp_state = STATE_DHCP_LEASED;
				  } 
					else 
					{
					// IP address conflict occurred
					 reset_DHCP_timeout();
					 dhcp_ip_conflict();
				   dhcp_state = STATE_DHCP_READY;
				  }
			} 
				else if (type == DHCP_NAK) 
				{
#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK\r\n");
#endif
				  reset_DHCP_timeout();
				  dhcp_state = STATE_DHCP_DISCOVER;
			  } 
				else 
					ret = check_DHCP_timeout();
		    break;
		case STATE_DHCP_LEASED :
		   ret = DHCP_IP_LEASED;		  
			 if ((dhcp_lease_time != DEFAULT_LEASETIME) && ((dhcp_lease_time/2) < dhcp_time)) {
				
#ifdef _DHCP_DEBUG_
 				printf("> Maintains the IP address \r\n");
#endif
				type = 0;
				OLD_allocated_ip[0] = DHCP_allocated_ip[0];
				OLD_allocated_ip[1] = DHCP_allocated_ip[1];
				OLD_allocated_ip[2] = DHCP_allocated_ip[2];
				OLD_allocated_ip[3] = DHCP_allocated_ip[3];
				
				DHCP_XID++;
				send_DHCP_REQUEST();				
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_REREQUEST;
			 }
		    break;
		case STATE_DHCP_REREQUEST :
		   ret = DHCP_IP_LEASED;
			if (type == DHCP_ACK) {
				dhcp_retry_count = 0;
				if (OLD_allocated_ip[0] != DHCP_allocated_ip[0] || 
				    OLD_allocated_ip[1] != DHCP_allocated_ip[1] ||
				    OLD_allocated_ip[2] != DHCP_allocated_ip[2] ||
				    OLD_allocated_ip[3] != DHCP_allocated_ip[3]) 
				{
					ret = DHCP_IP_CHANGED;
					dhcp_ip_update();
               #ifdef _DHCP_DEBUG_
                  printf(">IP changed.\r\n");
               #endif					
				}
         #ifdef _DHCP_DEBUG_
            else printf(">IP is continued.\r\n");
         #endif            				
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_LEASED;
			} 
			else if (type == DHCP_NAK) 
				{
#ifdef _DHCP_DEBUG_
				printf("> Receive DHCP_NACK, Failed to maintain ip\r\n");
#endif
				reset_DHCP_timeout();
				dhcp_state = STATE_DHCP_DISCOVER;
			} 
			else ret = check_DHCP_timeout();
	   	break;
		default :
   		break;
	}
	return ret;
}

