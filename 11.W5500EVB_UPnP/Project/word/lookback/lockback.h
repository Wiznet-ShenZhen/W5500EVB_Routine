#ifndef _LOCKBACK_H
#define _LOCKBACK_H

#include "types.h"
void loopback_tcps(SOCKET s, uint16 port);
void loopback_tcpc(SOCKET s, uint16 port);
void loopback_udp(SOCKET s, uint16 port);
#endif
