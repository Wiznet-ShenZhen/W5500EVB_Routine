#include "config.h"

#define ARP_TYPE	    0x0806
#define ARP_TYPE_HI	    0x08
#define ARP_TYPE_LO     0x06

#define ETHER_TYPE	    0x0001
#define PRO_TYPE	    0x0800
#define HW_SIZE	        6
#define PRO_SIZE	    4
#define ARP_REQUEST	    0x0001
#define ARP_REPLY	    0x0002

typedef struct _ARPMSG
{
	uint8  dst_mac[6];	// ff.ff.ff.ff.ff.ff
	uint8  src_mac[6];	
	uint16 msg_type;   // ARP (0x0806)
	uint16 hw_type;		// Ethernet (0x0001)
	uint16 pro_type;	// IP	(0x0800)
	uint8  hw_size;		// 6
	uint8  pro_size;	    // 4
	uint16 opcode;		// request (0x0001), reply(0x0002)
	uint8  sender_mac[6];	
	uint8  sender_ip[4];		
	uint8  tgt_mac[6];	// 00.00.00.00.00.00
	uint8  tgt_ip[4];
	uint8  trailer[22];	// All zeros
}ARPMSG;

void arp(SOCKET s, uint16 pPort, uint8 *SrcIP, uint8 *SrcMac, uint8 *TgtIp, uint8 count);
void arp_request(SOCKET s, uint16 port, uint8 * SrcIP, uint8 *SrcMac, uint8 *TgtIp);
void arp_reply(SOCKET s, uint16 rlen);
void do_arp(void);
void Timer2_ISR(void);
