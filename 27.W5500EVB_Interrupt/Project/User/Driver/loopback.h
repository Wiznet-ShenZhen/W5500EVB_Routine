#ifndef _LOOPBACK_H
#define _LOOPBACK_H

#include "config.h"
#include "device.h"
#include "spi2.h"
#include "socket.h"
#include "w5500.h"
#include "types.h"
void loopback_tcps(SOCKET s, uint16 port);
void loopback_tcpc(SOCKET s, uint16 port);
void loopback_udp(SOCKET s, uint16 port);
void ISR(void);
#endif
