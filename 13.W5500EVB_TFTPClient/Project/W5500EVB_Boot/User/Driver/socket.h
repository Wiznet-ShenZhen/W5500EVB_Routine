
#ifndef	_SOCKET_H_
#define	_SOCKET_H_

#include "stm32f10x.h"
#include "Types.h"

/*Socket 端口选择，可按自己的习惯定义*/
#define SOCK_TCPS             0
#define SOCK_TCPC             1
#define SOCK_TFTP							1
#define SOCK_UDPS             2
#define SOCK_DHCP             3																	/* 定义NetBIOS采用的socket*/
#define SOCK_DNS              4


#define SF_IO_NONBLOCK           0x01              ///< Socket nonblock io mode. It used parameter in \ref socket().

#define SOCK_OK               1        ///< Result is OK about socket process.
#define SOCK_BUSY             0        ///< Socket is busy on processing the operation. Valid only Non-block IO Mode.
#define SOCK_FATAL            -1000    ///< Result is fatal error about socket process.

extern uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag); // Opens a socket(TCP or UDP or IP_RAW mode)
extern void close(SOCKET s); // Close socket
extern uint8 connect(SOCKET s, uint8 * addr, uint16 port); // Establish TCP connection (Active connection)
extern void disconnect(SOCKET s); // disconnect the connection
extern uint8 listen(SOCKET s);	// Establish TCP connection (Passive connection)
extern uint16 send(SOCKET s, const uint8 * buf, uint16 len); // Send data (TCP)
extern uint16 recv(SOCKET s, uint8 * buf, uint16 len);	// Receive data (TCP)
extern uint16 sendto(SOCKET s, const uint8 * buf, uint16 len, uint8 * addr, uint16 port); // Send data (UDP/IP RAW)
extern uint16 recvfrom(SOCKET s, uint8 * buf, uint16 len, uint8 * addr, uint16  *port); // Receive data (UDP/IP RAW)

#ifdef __MACRAW__
void macraw_open(void);
uint16 macraw_send( const uint8 * buf, uint16 len ); //Send data (MACRAW)
uint16 macraw_recv( uint8 * buf, uint16 len ); //Recv data (MACRAW)
#endif

#endif
/* _SOCKET_H_ */

