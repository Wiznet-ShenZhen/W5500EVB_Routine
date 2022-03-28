#include "stm32f10x.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
//#include <time.h>
#include "UPnP.h"
#include "MakeXML.h"
#include "w5500.h"
#include "socket.h"
#include "Types.h"
#include "ult.h"
//#define UPNP_DEBUG				/**< UPnP Debug */
#define CONT_BUFFER_SIZE	(1024*2)	/**< Content Buffer Size */
#define SEND_BUFFER_SIZE	(2048*2)	/**< Send Buffer Size */
#define RECV_BUFFER_SIZE	(1024*4)	/**< Receive Buffer Size */
#define PORT_SSDP		1901		/**< SSDP Port Number */
#define PORT_UPNP		5001		/**< UPnP Port Number */
#define PORT_UPNP_EVENTING	5002		/**< UPnP Eventing Port Number */

extern uint32 my_time;		/**< check the Timeout */

char UPnP_Step=0;		/**< UPnP Step Check */

char descURL[64]={'\0'};	/**< Description URL */
char descIP[16]={'\0'};		/**< Description IP */
char descPORT[16]={'\0'};	/**< Description Port */
char descLOCATION[64]={'\0'};	/**< Description Location */
char controlURL[64]={'\0'};	/**< Control URL */
char eventSubURL[64]={'\0'};	/**< Eventing Subscription URL */

char content[CONT_BUFFER_SIZE]={'\0'};		/**< HTTP Body */
char send_buffer[SEND_BUFFER_SIZE]={'\0'};		/**< Send Buffer */
char recv_buffer[RECV_BUFFER_SIZE]={'\0'};		/**< Receive Buffer */

/**< SSDP Header */
const char SSDP[] = "\
M-SEARCH * HTTP/1.1\r\n\
Host:239.255.255.250:1900\r\n\
ST:urn:schemas-upnp-org:device:InternetGatewayDevice:1\r\n\
Man:\"ssdp:discover\"\r\n\
MX:3\r\n\
\r\n\
";

/**
 * @brief	This function processes the SSDP message.
 * @return	0: success, -1: reply packet timeout, 1: received SSDP parse error
 */
signed char SSDPProcess(
  SOCKET sockfd 	/**< a socket number. */
  )
{
	char ret_value=0;
	long endTime=0;
	uint8 mcast_addr[4] = {239,255,255,250}; 
	//uint8 mcast_mac[6] = {0x28, 0x2C, 0xB2, 0xE9, 0x42, 0xD6};
  uint8 recv_addr[4];
  uint16 recv_port;

	// UDP Socket Open
        close(sockfd);
        socket(sockfd,Sn_MR_UDP,PORT_SSDP,0);/*初始化socket 0的套接字*/
        while(getSn_SR(sockfd)!=SOCK_UDP);

#ifdef UPNP_DEBUG
	printf("%s\r\n", SSDP);
#endif

	// Send SSDP
	if(sendto(sockfd, (const uint8*)SSDP, strlen(SSDP), mcast_addr, 1900) <= 0) 
            printf("SSDP Send error!!!!!!!\r\n");

	// Receive Reply
	memset(recv_buffer, '\0', RECV_BUFFER_SIZE);
	endTime = my_time + 3;
	while (recvfrom(sockfd, (uint8*)recv_buffer, RECV_BUFFER_SIZE, recv_addr, &recv_port) <= 0 && my_time < endTime);	// Check Receive Buffer of W5200
	if(my_time >= endTime){	// Check Timeout
		close(sockfd);
		return -1;
	}

	// UDP Socket Close
        close(sockfd);

#ifdef UPNP_DEBUG
	printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
#endif

	// Parse SSDP Message
	if((ret_value=parseSSDP(recv_buffer)) == 0) UPnP_Step=1;
	return ret_value;
}

/**
 * @brief	This function gets the description message from IGD(Internet Gateway Device).
 * @return	0: success, -2: Invalid UPnP Step, -1: reply packet timeout, 1: received xml parse error
 */
signed char GetDescriptionProcess(
  SOCKET sockfd 	/**< a socket number. */
  )
{
	char ret_value=0;
        long endTime=0;
        uint32 ipaddr;
        uint16 port;

	// Check UPnP Step
	if(UPnP_Step < 1) return -2;

	// Make HTTP GET Header
	memset(send_buffer, '\0', SEND_BUFFER_SIZE);
	MakeGETHeader(send_buffer);

#ifdef UPNP_DEBUG
	printf("%s\r\n", send_buffer);
#endif

        ipaddr = inet_addr((uint8*)descIP);
        ipaddr = swapl(ipaddr);
        port = ATOI(descPORT, 10);

	// Connect to IGD(Internet Gateway Device)
        socket(sockfd,Sn_MR_TCP,PORT_UPNP,Sn_MR_ND);/*打开socket的一个端口*/
        while(getSn_SR(sockfd)!=SOCK_INIT);
        {
           Delay_ms(100); 
        }
        
        if(connect(sockfd, (uint8*)&ipaddr, port)==0)
            printf("TCP Socket Error!!\r\n");

	// Send Get Discription Message
	while((getSn_SR(sockfd) != SOCK_ESTABLISHED));
	send(sockfd, (void *)send_buffer, strlen(send_buffer));

	// Receive Reply
	memset(recv_buffer, '\0', RECV_BUFFER_SIZE);
	Delay_ms(500);
	endTime = my_time + 3;
	while (recv(sockfd, (void *)recv_buffer, RECV_BUFFER_SIZE) <= 0 && my_time < endTime);	// Check Receive Buffer of W5200
	if(my_time >= endTime){	// Check Timeout
		close(sockfd);
		return -1;
	}

	// TCP Socket Close
	close(sockfd);

#ifdef UPNP_DEBUG
	printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
#endif

	// Parse Discription Message
	if((ret_value = parseDescription(recv_buffer)) == 0) UPnP_Step = 2;
	return ret_value;
}

/**
 * @brief	This function subscribes to the eventing message from IGD(Internet Gateway Device).
 * @return	0: success, -2: Invalid UPnP Step, -1: reply packet timeout
 */
signed char SetEventing(
  SOCKET sockfd 	/**< a socket number. */
  )
{
        long endTime=0;
        uint32 ipaddr;
        uint16 port;

	// Check UPnP Step
	if(UPnP_Step < 2) return -2;

	// Make Subscription message
	memset(send_buffer, '\0', SEND_BUFFER_SIZE);
	MakeSubscribe(send_buffer, PORT_UPNP_EVENTING);

#ifdef UPNP_DEBUG
	printf("%s\r\n", send_buffer);
#endif

        ipaddr = inet_addr((uint8*)descIP);
        ipaddr = swapl(ipaddr);
        port = ATOI(descPORT, 10);

        // Connect to IGD(Internet Gateway Device)
        socket(sockfd,Sn_MR_TCP,PORT_UPNP,Sn_MR_ND);/*打开socket的一个端口*/
        while(getSn_SR(sockfd)!=SOCK_INIT);
        {
           Delay_ms(100); 
        }
        
        if(connect(sockfd, (uint8*)&ipaddr, port)==0)
            printf("TCP Socket Error!!\r\n");

	// Send Get Discription Message
	while((getSn_SR(sockfd) != SOCK_ESTABLISHED));
	send(sockfd, (void *)send_buffer, strlen(send_buffer));

	// Receive Reply
	memset(recv_buffer, '\0', RECV_BUFFER_SIZE);
	Delay_ms(500);
	endTime = my_time + 3;
	while (recv(sockfd, (void *)recv_buffer, RECV_BUFFER_SIZE) <= 0 && my_time < endTime);	// Check Receive Buffer of W5200
	if(my_time >= endTime){	// Check Timeout
		close(sockfd);
		return -1;
	}

	// TCP Socket Close
	close(sockfd);

#ifdef UPNP_DEBUG
	printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
#endif

	return parseHTTP(recv_buffer);
}

/**
 * @brief	This function listenes the eventing message from IGD(Internet Gateway Device).
 */
void eventing_listener(
  SOCKET sockfd 	/**< a socket number. */
  )
{
	uint16 len;
        const uint8 HTTP_OK[] = "HTTP/1.1 200 OK\r\n\r\n";

	switch (getSn_SR(sockfd))
	{
		case SOCK_INIT:
                  listen(sockfd);
                  break;
                case SOCK_ESTABLISHED:					/* if connection is established */
			Delay_ms(500);
			if((len = getSn_RX_RSR(sockfd)) > 0){
				recv(sockfd, (void *)recv_buffer, len);
				send(sockfd, (void *)HTTP_OK, strlen((void *)HTTP_OK));
				parseEventing(recv_buffer);
#ifdef UPNP_DEBUG
				printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
#endif
			}

		break;

		case SOCK_CLOSE_WAIT:					/* If the client request to close */
			if ((len = getSn_RX_RSR(sockfd)) > 0) 		/* check Rx data */
			{
				recv(sockfd, (void *)recv_buffer, len);	/* read the received data */
			}
			close(sockfd);
		break;

		case SOCK_CLOSED:					/* if a socket is closed */
			socket(sockfd,Sn_MR_TCP,PORT_UPNP_EVENTING,Sn_MR_ND);/*打开socket的一个端口*/
		break;

		default:
		break;
	}
}

/**
 * @brief	This function processes the delete port to IGD(Internet Gateway Device).
 * @return	0: success, -2: Invalid UPnP Step, -1: reply packet timeout, 1: received xml parse error, other: UPnP error code
 */
signed short DeletePortProcess(
  SOCKET sockfd,	/**< a socket number. */
  const char* protocol,	/**< a procotol name. "TCP" or "UDP" */
  const unsigned int extertnal_port	/**< an external port number. */
  )
{
	short len=0;
	long endTime=0;
	uint32 ipaddr;
	uint16 port;

	// Check UPnP Step
	if(UPnP_Step < 2) return -2;

	// Make "Delete Port" XML(SOAP)
	memset(content, '\0', CONT_BUFFER_SIZE);
	MakeSOAPDeleteControl(content, protocol, extertnal_port);

	// Make HTTP POST Header
	memset(send_buffer, '\0', SEND_BUFFER_SIZE);
	len = strlen(content);
	MakePOSTHeader(send_buffer, len, DELETE_PORT);
	strcat(send_buffer, content);

//#ifdef UPNP_DEBUG
	printf("%s\r\n", send_buffer);
//#endif

        ipaddr = inet_addr((uint8*)descIP);
        ipaddr = swapl(ipaddr);
        port = ATOI(descPORT, 10);
         
        // Connect to IGD(Internet Gateway Device)
        socket(sockfd,Sn_MR_TCP,PORT_UPNP,Sn_MR_ND);/*打开socket的一个端口*/
        while(getSn_SR(sockfd)!=SOCK_INIT);
        
        if(connect(sockfd, (uint8*)&ipaddr, port)==0)
            printf("TCP Socket Error!!\r\n");

	// Send "Delete Port" Message
	while(getSn_SR(sockfd) != SOCK_ESTABLISHED);
	send(sockfd, (void *)send_buffer, strlen(send_buffer));

	// Receive Reply
	memset(recv_buffer, '\0', RECV_BUFFER_SIZE);
	Delay_ms(500);
	endTime = my_time + 3;
	while (recv(sockfd, (void *)recv_buffer, RECV_BUFFER_SIZE) <= 0 && my_time < endTime);	// Check Receive Buffer of W5200
	if(my_time >= endTime){	// Check Timeout
		close(sockfd);
		return -1;
	}

	// TCP Socket Close
        close(sockfd);

//#ifdef UPNP_DEBUG
	printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
//#endif

	// Parse Replied Message
	return parseDeletePort(recv_buffer);
}

/**
 * @brief	This function processes the add port to IGD(Internet Gateway Device).
 * @return	0: success, -2: Invalid UPnP Step, -1: reply packet timeout, 1: received xml parse error, other: UPnP error code
 */
signed short AddPortProcess(
  SOCKET sockfd,	/**< a socket number. */
  const char* protocol,	/**< a procotol name. "TCP" or "UDP" */
  const unsigned int extertnal_port,	/**< an external port number. */
  const char* internal_ip,	/**< an internal ip address. */
  const unsigned int internal_port,	/**< an internal port number. */
  const char* description	/**< a description of this portforward. */
  )
{
	short len=0;
        long endTime=0;
        uint32 ipaddr;
        uint16 port;

	// Check UPnP Step
	if(UPnP_Step < 2) return -2;

	// Make "Add Port" XML(SOAP)
	memset(content, '\0', CONT_BUFFER_SIZE);
	MakeSOAPAddControl(content, protocol, extertnal_port, internal_ip, internal_port, description);

	// Make HTTP POST Header
	memset(send_buffer, '\0', SEND_BUFFER_SIZE);
	len = strlen(content);
	MakePOSTHeader(send_buffer, len, ADD_PORT);
	strcat(send_buffer, content);

//#ifdef UPNP_DEBUG
	printf("%s\r\n", send_buffer);
//#endif

	ipaddr = inet_addr((uint8*)descIP);
	ipaddr = swapl(ipaddr);
	port = ATOI(descPORT, 10);

        // Connect to IGD(Internet Gateway Device)
        socket(sockfd,Sn_MR_TCP,PORT_UPNP,Sn_MR_ND);/*打开socket的一个端口*/
        while(getSn_SR(sockfd)!=SOCK_INIT);
        
        if(connect(sockfd, (uint8*)&ipaddr, port)==0)
            printf("TCP Socket Error!!\r\n");

	// Send "Delete Port" Message
	while(getSn_SR(sockfd) != SOCK_ESTABLISHED);
	send(sockfd, (void *)send_buffer, strlen(send_buffer));

	// Receive Reply
	memset(recv_buffer, '\0', RECV_BUFFER_SIZE);
	Delay_ms(500);
	endTime = my_time + 3;
	while (recv(sockfd, (void *)recv_buffer, RECV_BUFFER_SIZE) <= 0 && my_time < endTime);	// Check Receive Buffer of W5200
	if(my_time >= endTime){	// Check Timeout
		close(sockfd);
		return -1;
	}

	// TCP Socket Close
	close(sockfd);

//#ifdef UPNP_DEBUG
	printf("\r\nReceiveData\r\n%s\r\n", recv_buffer);
//#endif

	// Parse Replied Message
	return parseAddPort(recv_buffer);
}


/*-----String Parse Functions-----*/

/**
 * @brief	This function parses the HTTP header.
 * @return	0: success, 1: received xml parse error
 */
signed char parseHTTP(
  const char* xml 	/**< string for parse */
  )
{
	char *loc=0;
	if(strstr(xml, "200 OK") != NULL)
		return 0;
	else{
		loc = strstr(xml, "\r\n");
		memset(content, '\0', CONT_BUFFER_SIZE);
		strncpy(content, xml, loc-xml);
		printf("\r\nHTTP Error:\r\n%s\r\n\r\n", content);
		return 1;
	}
}

/**
 * @brief	This function parses the received SSDP message from IGD(Internet Gateway Device).
 * @return	0: success, 1: received xml parse error
 */
signed char parseSSDP(
  const char* xml 	/**< string for parse */
  )
{
	const char LOCATION_[]="LOCATION: ";
	char *LOCATION_start=0, *LOCATION_end=0;

	if(parseHTTP(xml) != 0) return 1;

	// Find Description URL("http://192.168.0.1:3121/etc/linuxigd/gatedesc.xml")
	if((LOCATION_start = strstr(xml, LOCATION_)) == NULL) return 1;
	if((LOCATION_end = strstr(LOCATION_start, "\r\n")) == NULL) return 1;
	strncpy(descURL, LOCATION_start+strlen(LOCATION_), LOCATION_end-LOCATION_start-strlen(LOCATION_));

	// Find IP of IGD("http://192.168.0.1")
	if((LOCATION_start = strstr(descURL, "http://")) == NULL) return 1;
	if((LOCATION_end = strstr(LOCATION_start+7, ":")) == NULL) return 1;
	strncpy(descIP, LOCATION_start+7, LOCATION_end-LOCATION_start-7);

	// Find PORT of IGD("3121")
	if((LOCATION_start = LOCATION_end+1) == NULL) return 1;
	if((LOCATION_end = strstr(LOCATION_start, "/")) == NULL) return 1;
	strncpy(descPORT, LOCATION_start, LOCATION_end-LOCATION_start);

	// Find Description Location("/etc/linuxigd/gatedesc.xml")
	if((LOCATION_start = LOCATION_end) == NULL) return 1;
	if((LOCATION_end = LOCATION_start + strlen(LOCATION_start)) == NULL) return 1;
	strncpy(descLOCATION, LOCATION_start, LOCATION_end-LOCATION_start);

	return 0;
}

/**
 * @brief	This function parses the received description message from IGD(Internet Gateway Device).
 * @return	0: success, 1: received xml parse error
 */
signed char parseDescription(
  const char* xml 	/**< string for parse */
  )
{
	const char controlURL_[]="<controlURL>";
	const char eventSubURL_[]="<eventSubURL>";
	char *URL_start=0, *URL_end=0;

	if(parseHTTP(xml) != 0) return 1;
        //printf("\r\n%s\r\n", xml);
	// Find Control URL("/etc/linuxigd/gateconnSCPD.ctl")
	if((URL_start = strstr(xml, "urn:schemas-upnp-org:service:WANIPConnection:1")) == NULL) return 1;
	if((URL_start = strstr(URL_start, controlURL_)) == NULL) return 1;
	if((URL_end = strstr(URL_start, "</controlURL>")) == NULL) return 1;

	strncpy(controlURL, URL_start+strlen(controlURL_), URL_end-URL_start-strlen(controlURL_));

	// Find Eventing Subscription URL("/etc/linuxigd/gateconnSCPD.evt")
	if((URL_start = strstr(xml, "urn:schemas-upnp-org:service:WANIPConnection:1")) == NULL) return 1;
	if((URL_start = strstr(URL_start, eventSubURL_)) == NULL) return 1;
	if((URL_end = strstr(URL_start, "</eventSubURL>")) == NULL) return 1;

	strncpy(eventSubURL, URL_start+strlen(eventSubURL_), URL_end-URL_start-strlen(eventSubURL_));

	return 0;
}

/**
 * @brief	This function parses and prints the received eventing message from IGD(Internet Gateway Device).
 */
void parseEventing(
  const char* xml 	/**< string for parse */
  )
{
	const char PossibleConnectionTypes_[]="<PossibleConnectionTypes>";
	const char ConnectionStatus_[]="<ConnectionStatus>";
	const char ExternalIPAddress_[]="<ExternalIPAddress>";
	const char PortMappingNumberOfEntries_[]="<PortMappingNumberOfEntries>";
	char *start=0, *end=0;

	// Find Possible Connection Types
	if((start = strstr(xml, PossibleConnectionTypes_)) != NULL){
		if((end = strstr(start, "</PossibleConnectionTypes>")) != NULL){
			memset(content, '\0', CONT_BUFFER_SIZE);
			strncpy(content, start+strlen(PossibleConnectionTypes_), end-start-strlen(PossibleConnectionTypes_));
			printf("Receive Eventing(PossibleConnectionTypes): %s\r\n", content);
		}
	}

	// Find Connection Status
	if((start = strstr(xml, ConnectionStatus_)) != NULL){
		if((end = strstr(start, "</ConnectionStatus>")) != NULL){
			memset(content, '\0', CONT_BUFFER_SIZE);
			strncpy(content, start+strlen(ConnectionStatus_), end-start-strlen(ConnectionStatus_));
			printf("Receive Eventing(ConnectionStatus): %s\r\n", content);
		}
	}

	// Find External IP Address
	if((start = strstr(xml, ExternalIPAddress_)) != NULL){
		if((end = strstr(start, "</ExternalIPAddress>")) != NULL){
			memset(content, '\0', CONT_BUFFER_SIZE);
			strncpy(content, start+strlen(ExternalIPAddress_), end-start-strlen(ExternalIPAddress_));
			printf("Receive Eventing(ExternalIPAddress): %s\r\n", content);
		}
	}

	// Find Port Mapping Number Of Entries
	if((start = strstr(xml, PortMappingNumberOfEntries_)) != NULL){
		if((end = strstr(start, "</PortMappingNumberOfEntries>")) != NULL){
			memset(content, '\0', CONT_BUFFER_SIZE);
			strncpy(content, start+strlen(PortMappingNumberOfEntries_), end-start-strlen(PortMappingNumberOfEntries_));
			printf("Receive Eventing(PortMappingNumberOfEntries): %s\r\n", content);
		}
	}
}

/**
 * @brief	This function parses the received UPnP error message from IGD(Internet Gateway Device).
 * @return	0: success, 1: received xml parse error, other: UPnP error code
 */
signed short parseError(
  const char* xml 	/**< string for parse */
  )
{
	const char faultstring_[]="<faultstring>";
	const char errorCode_[]="<errorCode>";
	const char errorDescription_[]="<errorDescription>";
	char *start, *end;
	short ret=0;

	// Find Fault String
	if((start = strstr(xml, faultstring_)) == NULL) return 1;
	if((end   = strstr(xml, "</faultstring>")) == NULL) return 1;
	memset(content, '\0', CONT_BUFFER_SIZE);
	strncpy(content, start+strlen(faultstring_), end-start-strlen(faultstring_));
	printf("faultstring: %s\r\n", content);

	// Find Error Code
	if((start = strstr(xml, errorCode_)) == NULL) return 1;
	if((end   = strstr(xml, "</errorCode>")) == NULL) return 1;
	memset(content, '\0', CONT_BUFFER_SIZE);
	strncpy(content, start+strlen(errorCode_), end-start-strlen(errorCode_));
	printf("errorCode: %s\r\n", content);
        ret = ATOI(content, 10);

	// Find Error Description
	if((start = strstr(xml, errorDescription_)) == NULL) return 1;
	if((end   = strstr(xml, "</errorDescription>")) == NULL) return 1;
	memset(content, '\0', CONT_BUFFER_SIZE);
	strncpy(content, start+strlen(errorDescription_), end-start-strlen(errorDescription_));
	printf("errorDescription: %s\r\n\r\n", content);

	return ret;
}

/**
 * @brief	This function parses the received delete port message from IGD(Internet Gateway Device).
 * @return	0: success, 1: received xml parse error, other: UPnP error code
 */
signed short parseDeletePort(
  const char* xml 	/**< string for parse */
  )
{
	parseHTTP(xml);
	if(strstr(xml, "u:DeletePortMappingResponse xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\"") == NULL){
		return parseError(xml);
	}

	return 0;
}

/**
 * @brief	This function parses the received add port message from IGD(Internet Gateway Device).
 * @return	0: success, 1: received xml parse error, other: UPnP error code
 */
signed short parseAddPort(
  const char* xml 	/**< string for parse */
  )
{
	parseHTTP(xml);
	if(strstr(xml, "u:AddPortMappingResponse xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\"") == NULL){
		return parseError(xml);
	}

	return 0;
}
