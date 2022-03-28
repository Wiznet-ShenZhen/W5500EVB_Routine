#include <stdio.h>
#include <string.h>
#include "MakeXML.h"
#include "w5500.h"

extern char descURL[64];		/**< Description URL */
extern char descIP[16];			/**< Description IP */
extern char descPORT[6];		/**< Description Port */
extern char descLOCATION[64];		/**< Description Location */
extern char controlURL[64];		/**< Control URL */
extern char eventSubURL[64];		/**< Eventing Subscription URL */

/**< SOAP header & tail */
const char soap_start[]=
"\
<?xml version=\"1.0\"?>\r\n\
<SOAP-ENV:Envelope xmlns:SOAP-ENV=\"http://schemas.xmlsoap.org/soap/envelope/\" SOAP-ENV:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\"><SOAP-ENV:Body>\
";

const char soap_end[]=
"\
</SOAP-ENV:Body></SOAP-ENV:Envelope>\r\n\
";

/**< Delete Port Mapping */
const char DeletePortMapping_[]="<m:DeletePortMapping xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">";
const char _DeletePortMapping[]="</m:DeletePortMapping>";

/**< New Remote Host */
const char NewRemoteHost_[]="<NewRemoteHost xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">";
const char _NewRemoteHost[]="</NewRemoteHost>";

/**< New External Port */
const char NewExternalPort_[]="<NewExternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">";
const char _NewExternalPort[]="</NewExternalPort>";

/**< New Protocol */
const char NewProtocol_[]="<NewProtocol xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">";
const char _NewProtocol[]="</NewProtocol>";

/**< Add Port Mapping */
const char AddPortMapping_[]="<m:AddPortMapping xmlns:m=\"urn:schemas-upnp-org:service:WANIPConnection:1\">";
const char _AddPortMapping[]="</m:AddPortMapping>";

/**< New Internal Port */
const char NewInternalPort_[]="<NewInternalPort xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui2\">";
const char _NewInternalPort[]="</NewInternalPort>";

/**< New Internal Client */
const char NewInternalClient_[]="<NewInternalClient xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">";
const char _NewInternalClient[]="</NewInternalClient>";

/**< New Enabled */
const char NewEnabled[] ="<NewEnabled xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"boolean\">1</NewEnabled>";
const char NewEnabled_[]="<NewEnabled xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"boolean\">";
const char _NewEnabled[]="</NewEnabled>";

/**< New Port Mapping Description */
const char NewPortMappingDescription_[]="<NewPortMappingDescription xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"string\">";
const char _NewPortMappingDescription[]="</NewPortMappingDescription>";

/**< New Lease Duration */
const char NewLeaseDuration[] ="<NewLeaseDuration xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">0</NewLeaseDuration>";
const char NewLeaseDuration_[]="<NewLeaseDuration xmlns:dt=\"urn:schemas-microsoft-com:datatypes\" dt:dt=\"ui4\">";
const char _NewLeaseDuration[]="</NewLeaseDuration>";


/**
 * @brief	This function makes the HTTP POST Header.
 */
void MakePOSTHeader(char* dest, int content_length, int action)
{
	char local_length[6]={'\0'}, local_port[6]={'\0'};
	sprintf(local_length, "%d", content_length);
	strcat(dest, "POST ");
	strcat(dest, controlURL);
	strcat(dest, " HTTP/1.1\r\n");
	strcat(dest, "Content-Type: text/xml; charset=\"utf-8\"\r\n");
	strcat(dest, "SOAPAction: \"urn:schemas-upnp-org:service:WANIPConnection:1#");
	switch(action){
	case DELETE_PORT:
		strcat(dest, "DeletePortMapping\"");
		break;
	case ADD_PORT:
		strcat(dest, "AddPortMapping\"");
		break;
	}
	strcat(dest, "\r\nUser-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)\r\n");
	strcat(dest, "Host: ");
	strcat(dest, descIP);
	sprintf(local_port, ":%s", descPORT);
	strcat(dest, local_port);
	strcat(dest, "\r\nContent-Length: ");
	strcat(dest, local_length);
	strcat(dest, "\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nPragma: no-cache\r\n\r\n");
}

/**
 * @brief	This function makes the HTTP GET header.
 */
void MakeGETHeader(char* dest)
{
	char local_port[6]={'\0'};
	strcat(dest, "GET ");
	strcat(dest, descLOCATION);
	strcat(dest, " HTTP/1.1\r\n");
	strcat(dest, "Accept: text/xml, application/xml\r\n");
	strcat(dest, "User-Agent: Mozilla/4.0 (compatible; UPnP/1.0; Windows NT/5.1)\r\n");
	strcat(dest, "Host: ");
	strcat(dest, descIP);
	sprintf(local_port, ":%s", descPORT);
	strcat(dest, local_port);
	strcat(dest, "\r\nConnection: Keep-Alive\r\nCache-Control: no-cache\r\nPragma: no-cache\r\n\r\n");
}

/**
 * @brief	This function makes the Subscription message.
 */
void MakeSubscribe(char* dest, const unsigned int listen_port)
{
	char local_port[6]={'\0'}, ipaddr[16]={'\0'};
	unsigned char ip[4];
	strcat(dest, "SUBSCRIBE ");
	strcat(dest, eventSubURL);
	strcat(dest, " HTTP/1.1\r\n");
	strcat(dest, "Host: ");
	strcat(dest, descIP);
	sprintf(local_port, ":%s", descPORT);
	strcat(dest, local_port);
	strcat(dest, "\r\nUSER-AGENT: Mozilla/4.0 (compatible; UPnP/1.1; Windows NT/5.1)\r\n");
	strcat(dest, "CALLBACK: <http://");
	getSIPR(ip);
	sprintf(ipaddr, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	strcat(dest, ipaddr);
	sprintf(local_port, ":%d/>", listen_port);
	strcat(dest, local_port);
	strcat(dest, "\r\nNT: upnp:event\r\nTIMEOUT: Second-1800\r\n\r\n");
}

/**
 * @brief	This function makes the Add Port Control message in SOAP.
 */
void MakeSOAPAddControl(char* dest, const char* protocol, const unsigned int extertnal_port, const char* internal_ip, const unsigned int internal_port, const char* description)
{
	char local_port[6]={'\0'};
	strcat(dest, soap_start);
	strcat(dest, AddPortMapping_);
	strcat(dest, NewRemoteHost_);
	strcat(dest, _NewRemoteHost);
	strcat(dest, NewExternalPort_);
	sprintf(local_port, "%d", extertnal_port);
	strcat(dest, local_port);
	strcat(dest, _NewExternalPort);
	strcat(dest, NewProtocol_);
	strcat(dest, protocol);
	strcat(dest, _NewProtocol);
	strcat(dest, NewInternalPort_);
	sprintf(local_port, "%d", internal_port);
	strcat(dest, local_port);
	strcat(dest, _NewInternalPort);
	strcat(dest, NewInternalClient_);
	strcat(dest, internal_ip);
	strcat(dest, _NewInternalClient);
	strcat(dest, NewEnabled);
	strcat(dest, NewPortMappingDescription_);
	strcat(dest, description);
	strcat(dest, _NewPortMappingDescription);
	strcat(dest, NewLeaseDuration);
	strcat(dest, _AddPortMapping);
	strcat(dest, soap_end);
}

/**
 * @brief	This function makes the Delete Port Control message in SOAP.
 */
void MakeSOAPDeleteControl(char* dest, const char* protocol, const unsigned int extertnal_port)
{
	char local_port[6]={'\0'};
	strcat(dest, soap_start);
	strcat(dest, DeletePortMapping_);
	strcat(dest, NewRemoteHost_);
	strcat(dest, _NewRemoteHost);
	strcat(dest, NewExternalPort_);
	sprintf(local_port, "%d", extertnal_port);
	strcat(dest, local_port);
	strcat(dest, _NewExternalPort);
	strcat(dest, NewProtocol_);
	strcat(dest, protocol);
	strcat(dest, _NewProtocol);
	strcat(dest, _DeletePortMapping);
	strcat(dest, soap_end);
}
