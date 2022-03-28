
#ifndef __MakeXML_H__
#define __MakeXML_H__

#define DELETE_PORT	0
#define ADD_PORT	1

void MakePOSTHeader(char* dest, int content_length, int action);
void MakeGETHeader(char* dest);
void MakeSubscribe(char* dest, const unsigned int listen_port);

void MakeSOAPAddControl(char* dest, const char* protocol, const unsigned int extertnal_port, const char* internal_ip, const unsigned int internal_port, const char* description);
void MakeSOAPDeleteControl(char* dest, const char* protocol, const unsigned int extertnal_port);

#endif
