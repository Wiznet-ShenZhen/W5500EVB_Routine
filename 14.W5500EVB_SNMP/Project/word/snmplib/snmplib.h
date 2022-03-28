#ifndef _SNMPLIB_H_
#define _SNMPLIB_H_

//#define SUCCESS						0
#define OID_NOT_FOUND				-1
#define TABLE_FULL					-2
#define ILLEGAL_LENGTH				-3
#define INVALID_ENTRY_ID			-4
#define INVALID_DATA_TYPE			-5

#define NO_SUCH_NAME				2
#define BAD_VALUE					3

#define MAX_OID						12
#define MAX_STRING					40

#define SNMP_V1						0

#define COMMUNITY					"public\0"
#define COMMUNITY_SIZE				(strlen(COMMUNITY))

#define GET_REQUEST					0xa0
#define GET_NEXT_REQUEST			0xa1
#define GET_RESPONSE				0xa2
#define SET_REQUEST					0xa3

#define VALID_REQUEST(x)			((x == GET_REQUEST) || (x == GET_NEXT_REQUEST) || (x == SET_REQUEST))

#define SNMPDTYPE_INTEGER			0x02
#define SNMPDTYPE_OCTET_STRING		0x04
#define SNMPDTYPE_NULL_ITEM			0x05
#define SNMPDTYPE_OBJ_ID			0x06
#define SNMPDTYPE_SEQUENCE			0x30
#define SNMPDTYPE_SEQUENCE_OF		SNMPDTYPE_SEQUENCE

#define SNMPDTYPE_COUNTER			0x41
#define SNMPDTYPE_GAUGE  			0x42
#define SNMPDTYPE_TIME_TICKS		0x43
#define SNMPDTYPE_OPAQUE			0x44


#define UNUSED(x)					(void)x; // for IAR warning (declared but never referenced)
#define HTONL(x)					((((x)>>24) & 0xffL) | (((x)>>8) & 0xff00L) | (((x)<<8) & 0xff0000L) | (((x)<<24) & 0xff000000L))



#ifdef WIN32
typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
#else
#include "Types.h"
#endif


typedef struct {
	uint8 oidlen;
	uint8 oid[MAX_OID];
	uint8 dataType;
	uint8 dataLen;
	union {
		uint8 octetstring[MAX_STRING];
		uint32 intval;
	} u;
	void (*getfunction)(void *, uint8 *);
	void (*setfunction)(int32);
} dataEntryType;

struct messageStruct {
	uint8 buffer[1025];
	int32 len;
	int32 index;
};

typedef struct {
	int32 start;	/* Absolute Index of the TLV */
	int32 len;		/* The L value of the TLV */
	int32 vstart;   /* Absolute Index of this TLV's Value */
	int32 nstart;   /* Absolute Index of the next TLV */
} tlvStructType;

void WDEBUG(char *fmt, ...);
int32 findEntry(uint8 *oid, int32 len);
int32 getOID(int32 id, uint8 *oid, uint8 *len);
int32 getValue( uint8 *vptr, int32 vlen);
int32 getEntry(int32 id, uint8 *dataType, void *ptr, int32 *len);
int32 setEntry(int32 id, void *val, int32 vlen, uint8 dataType, int32 index);

int32 parseLength(const uint8 *msg, int32 *len);
int32 parseTLV(const uint8 *msg, int32 index, tlvStructType *tlv);
void insertRespLen(int32 reqStart, int32 respStart, int32 size);
int32 parseVarBind(int32 reqType, int32 index);
int32 parseSequence(int32 reqType, int32 index);
int32 parseSequenceOf(int32 reqType);
int32 parseRequest(void);
int32 parseCommunity(void);
int32 parseVersion(void);
int32 parseSNMPMessage(void);

void ipToByteArray(char *ip, uint8 *pDes);
int32 makeTrapVariableBindings(dataEntryType *oid_data, void *ptr, uint32 *len);
int32 SnmpXInit(void);
int32 SnmpXTrapSend(int8* managerIP, int8* agentIP, int8* community, dataEntryType enterprise_oid, uint32 genericTrap, uint32 specificTrap, uint32 va_count, ...);
int32 SnmpXDaemon(void);

#endif
