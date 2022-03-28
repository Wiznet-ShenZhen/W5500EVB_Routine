#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ctype.h"
#include "stm32f10x.h"
#include "w5500.h"
#include "socket.h"
#include "config.h"
#include "snmpLib.h"
#include "snmpDemo.h"

struct messageStruct request_msg;
struct messageStruct response_msg;

uint8 errorStatus, errorIndex;

#define COPY_SEGMENT_TEMP20110929(x) \
{ \
	request_msg.index += seglen; \
	memcpy(&response_msg.buffer[response_msg.index], &request_msg.buffer[x.start], seglen ); \
	response_msg.index += seglen; \
}

void WDEBUG(char* fmt, ...)
{
	char zlog_string[100];
	va_list ap;

	va_start(ap, fmt);
	vsprintf(zlog_string, fmt, ap);
	strcat(zlog_string, "\r\n");
	printf(zlog_string);
	va_end(ap);
}

int32 findEntry(uint8* oid, int32 len)
{
	int32 i;

	for (i = 0; i < maxData; i++)
	{
		if (len == snmpData[i].oidlen)
		{
			if (!memcmp(snmpData[i].oid, oid, len)) return(i);
		}
	}

	return OID_NOT_FOUND;
}


int32 getOID(int32 id, uint8* oid, uint8* len)
{
	int32 j;

	if (!((id >= 0) && (id < maxData))) return INVALID_ENTRY_ID;

	*len = snmpData[id].oidlen;

	for (j = 0; j < *len; j++)
	{
		oid[j] = snmpData[id].oid[j];
	}

	return SUCCESS;
}


int32 getValue(uint8* vptr, int32 vlen)
{
	int32 index = 0;
	int32 value = 0;

	while (index < vlen)
	{
		if (index != 0) value <<= 8;
		value |= vptr[index++];
	}

	return value;
}


int32 getEntry(int32 id, uint8* dataType, void* ptr, int32* len)
{
	if (!((id >= 0) && (id < maxData))) return INVALID_ENTRY_ID;

	*dataType = snmpData[id].dataType;

	switch (*dataType)
	{
	case SNMPDTYPE_OCTET_STRING:
	case SNMPDTYPE_OBJ_ID:
	{
		uint8* string = ptr;
		int32 j;

		if (snmpData[id].getfunction != NULL)
		{
			snmpData[id].getfunction((void*)&snmpData[id].u.octetstring, &snmpData[id].dataLen);
		}

		if ((*dataType) == SNMPDTYPE_OCTET_STRING)
		{
			snmpData[id].dataLen = (uint8)strlen((int8*)&snmpData[id].u.octetstring);
		}


		*len = snmpData[id].dataLen;
		for (j = 0; j < *len; j++)
		{
			string[j] = snmpData[id].u.octetstring[j];
		}
	}
	break;

	case SNMPDTYPE_INTEGER:
	case SNMPDTYPE_TIME_TICKS:
	case SNMPDTYPE_COUNTER:
	case SNMPDTYPE_GAUGE:
	{
		int32* value = (int32*)ptr;

		if (snmpData[id].getfunction != NULL)
		{
			snmpData[id].getfunction((void*)&snmpData[id].u.intval, &snmpData[id].dataLen);
		}

		*len = sizeof(uint32);
		*value = HTONL(snmpData[id].u.intval);
	}
	break;

	default:
		return INVALID_DATA_TYPE;
	}

	return SUCCESS;
}


int32 setEntry(int32 id, void* val, int32 vlen, uint8 dataType, int32 index)
{

	int32 retStatus = OID_NOT_FOUND;
	int32 j;

	if (snmpData[id].dataType != dataType)
	{
		errorStatus = BAD_VALUE;
		errorIndex = index;
		return INVALID_DATA_TYPE;
	}

	switch (snmpData[id].dataType)
	{
	case SNMPDTYPE_OCTET_STRING:
	case SNMPDTYPE_OBJ_ID:
	{
		uint8* string = val;
		for (j = 0; j < vlen; j++)
		{
			snmpData[id].u.octetstring[j] = string[j];
		}
		snmpData[id].dataLen = vlen;
	}
	retStatus = SUCCESS;
	break;

	case SNMPDTYPE_INTEGER:
	case SNMPDTYPE_TIME_TICKS:
	case SNMPDTYPE_COUNTER:
	case SNMPDTYPE_GAUGE:
	{
		snmpData[id].u.intval = getValue((uint8*)val, vlen);
		snmpData[id].dataLen = vlen;

		if (snmpData[id].setfunction != NULL)
		{
			snmpData[id].setfunction(snmpData[id].u.intval);
		}

	}
	retStatus = SUCCESS;
	break;

	default:
		retStatus = INVALID_DATA_TYPE;
		break;

	}

	return retStatus;
}


int32 parseLength(const uint8* msg, int32* len)
{
	int32 i = 1;

	if (msg[0] & 0x80)
	{
		int32 tlen = (msg[0] & 0x7f) - 1;
		*len = msg[i++];

		while (tlen--)
		{
			*len <<= 8;
			*len |= msg[i++];
		}
	}
	else
	{
		*len = msg[0];
	}

	return i;
}


int32 parseTLV(const uint8* msg, int32 index, tlvStructType* tlv)
{
	int32 Llen = 0;

	tlv->start = index;

	Llen = parseLength((const uint8*)&msg[index + 1], &tlv->len);

	tlv->vstart = index + Llen + 1;

	switch (msg[index])
	{
	case SNMPDTYPE_SEQUENCE:
	case GET_REQUEST:
	case GET_NEXT_REQUEST:
	case SET_REQUEST:
		tlv->nstart = tlv->vstart;
		break;
	default:
		tlv->nstart = tlv->vstart + tlv->len;
		break;
	}

	return 0;
}


void insertRespLen(int32 reqStart, int32 respStart, int32 size)
{
	int32 indexStart, lenLength;
	uint32 mask = 0xff;
	int32 shift = 0;

	if (request_msg.buffer[reqStart + 1] & 0x80)
	{
		lenLength = request_msg.buffer[reqStart + 1] & 0x7f;
		indexStart = respStart + 2;

		while (lenLength--)
		{
			response_msg.buffer[indexStart + lenLength] =
				(uint8)((size & mask) >> shift);
			shift += 8;
			mask <<= shift;
		}
	}
	else
	{
		response_msg.buffer[respStart + 1] = (uint8)(size & 0xff);
	}
}


int32 parseVarBind(int32 reqType, int32 index)
{
	int32 seglen = 0, id;
	tlvStructType name, value;
	int32 size = 0;

	extern const int32 maxData;

	parseTLV(request_msg.buffer, request_msg.index, &name);

	if (request_msg.buffer[name.start] != SNMPDTYPE_OBJ_ID) return -1;

	id = findEntry(&request_msg.buffer[name.vstart], name.len);

	if ((reqType == GET_REQUEST) || (reqType == SET_REQUEST))
	{
		seglen = name.nstart - name.start;
		COPY_SEGMENT_TEMP20110929(name);
		size = seglen;
	}
	else if (reqType == GET_NEXT_REQUEST)
	{
		response_msg.buffer[response_msg.index] = request_msg.buffer[name.start];

		if (++id >= maxData)
		{
			id = OID_NOT_FOUND;
			seglen = name.nstart - name.start;
			COPY_SEGMENT_TEMP20110929(name);
			size = seglen;
		}
		else
		{
			request_msg.index += name.nstart - name.start;

			getOID(id, &response_msg.buffer[response_msg.index + 2], &response_msg.buffer[response_msg.index + 1]);

			seglen = response_msg.buffer[response_msg.index + 1] + 2;
			response_msg.index += seglen;
			size = seglen;
		}
	}

	parseTLV(request_msg.buffer, request_msg.index, &value);

	if (id != OID_NOT_FOUND)
	{
		uint8 dataType;
		int32 len;

		if ((reqType == GET_REQUEST) || (reqType == GET_NEXT_REQUEST))
		{
			getEntry(id, &dataType, &response_msg.buffer[response_msg.index + 2], &len);

			response_msg.buffer[response_msg.index] = dataType;
			response_msg.buffer[response_msg.index + 1] = len;
			seglen = (2 + len);
			response_msg.index += seglen;

			request_msg.index += (value.nstart - value.start);

		}
		else if (reqType == SET_REQUEST)
		{
			setEntry(id, &request_msg.buffer[value.vstart], value.len, request_msg.buffer[value.start], index);
			seglen = value.nstart - value.start;
			COPY_SEGMENT_TEMP20110929(value);
		}
	}
	else
	{
		seglen = value.nstart - value.start;
		COPY_SEGMENT_TEMP20110929(value);

		errorIndex = index;
		errorStatus = NO_SUCH_NAME;
	}

	size += seglen;

	return size;
}


int32 parseSequence(int32 reqType, int32 index)
{
	int32 seglen;
	tlvStructType seq;
	int32 size = 0, respLoc;

	parseTLV(request_msg.buffer, request_msg.index, &seq);

	if (request_msg.buffer[seq.start] != SNMPDTYPE_SEQUENCE) return -1;

	seglen = seq.vstart - seq.start;
	respLoc = response_msg.index;
	COPY_SEGMENT_TEMP20110929(seq);

	size = parseVarBind(reqType, index);
	insertRespLen(seq.start, respLoc, size);
	size += seglen;

	return size;
}


int32 parseSequenceOf(int32 reqType)
{
	int32 seglen;
	tlvStructType seqof;
	int32 size = 0, respLoc;
	int32 index = 0;

	parseTLV(request_msg.buffer, request_msg.index, &seqof);

	if (request_msg.buffer[seqof.start] != SNMPDTYPE_SEQUENCE_OF) return -1;

	seglen = seqof.vstart - seqof.start;
	respLoc = response_msg.index;
	COPY_SEGMENT_TEMP20110929(seqof);

	while (request_msg.index < request_msg.len)
	{
		size += parseSequence(reqType, index++);
	}

	insertRespLen(seqof.start, respLoc, size);

	return size;
}


int32 parseRequest()
{
	int32 ret, seglen;
	tlvStructType snmpreq, requestid, errStatus, errIndex;
	int32 size = 0, respLoc, reqType;

	parseTLV(request_msg.buffer, request_msg.index, &snmpreq);

	reqType = request_msg.buffer[snmpreq.start];

	if (!VALID_REQUEST(reqType)) return -1;

	seglen = snmpreq.vstart - snmpreq.start;
	respLoc = snmpreq.start;
	size += seglen;
	COPY_SEGMENT_TEMP20110929(snmpreq);

	response_msg.buffer[snmpreq.start] = GET_RESPONSE;

	parseTLV(request_msg.buffer, request_msg.index, &requestid);
	seglen = requestid.nstart - requestid.start;
	size += seglen;
	COPY_SEGMENT_TEMP20110929(requestid);

	parseTLV(request_msg.buffer, request_msg.index, &errStatus);
	seglen = errStatus.nstart - errStatus.start;
	size += seglen;
	COPY_SEGMENT_TEMP20110929(errStatus);

	parseTLV(request_msg.buffer, request_msg.index, &errIndex);
	seglen = errIndex.nstart - errIndex.start;
	size += seglen;
	COPY_SEGMENT_TEMP20110929(errIndex);

	ret = parseSequenceOf(reqType);
	if (ret == -1) return -1;
	else size += ret;

	insertRespLen(snmpreq.start, respLoc, size);

	if (errorStatus)
	{
		response_msg.buffer[errStatus.vstart] = errorStatus;
		response_msg.buffer[errIndex.vstart] = errorIndex + 1;
	}

	return size;
}


int32 parseCommunity()
{
	int32 seglen;
	tlvStructType community;
	int32 size = 0;

	parseTLV(request_msg.buffer, request_msg.index, &community);

	if (!((request_msg.buffer[community.start] == SNMPDTYPE_OCTET_STRING) && (community.len == COMMUNITY_SIZE)))
	{
		return -1;
	}

	if (!memcmp(&request_msg.buffer[community.vstart], (int8*)COMMUNITY, COMMUNITY_SIZE))
	{
		seglen = community.nstart - community.start;
		size += seglen;
		COPY_SEGMENT_TEMP20110929(community);

		size += parseRequest();
	}
	else
	{
		return -1;
	}

	return size;
}


int32 parseVersion()
{
	int32 size = 0, seglen;
	tlvStructType tlv;

	size = parseTLV(request_msg.buffer, request_msg.index, &tlv);

	if (!((request_msg.buffer[tlv.start] == SNMPDTYPE_INTEGER) && (request_msg.buffer[tlv.vstart] == SNMP_V1)))
		return -1;

	seglen = tlv.nstart - tlv.start;
	size += seglen;
	COPY_SEGMENT_TEMP20110929(tlv);
	size = parseCommunity();

	if (size == -1) return size;
	else return (size + seglen);
}


int32 parseSNMPMessage()
{
	int32 size = 0, seglen, respLoc;
	tlvStructType tlv;

	parseTLV(request_msg.buffer, request_msg.index, &tlv);

	if (request_msg.buffer[tlv.start] != SNMPDTYPE_SEQUENCE_OF) return -1;

	seglen = tlv.vstart - tlv.start;
	respLoc = tlv.start;
	COPY_SEGMENT_TEMP20110929(tlv);

	size = parseVersion();

	if (size == -1) return -1;
	else size += seglen;

	insertRespLen(tlv.start, respLoc, size);

	return 0;
}


void dumpCode(int8* header, int8* tail, unsigned char* buff, int len)
{
	int i;

	printf(header);

	for (i = 0; i < len; i++)
	{
		if (i % 16 == 0)	printf("0x%04x : ", i);
		printf("%02x ", buff[i]);

		if (i % 16 - 15 == 0)
		{
			int j;
			printf("  ");
			for (j = i - 15; j <= i; j++)
			{
				if (isprint(buff[j]))	printf("%c", buff[j]);
				else					printf(".");
			}
			printf("\r\n");
		}
	}

	if (i % 16 != 0)
	{
		int j;
		int spaces = (len - i + 16 - i % 16) * 3 + 2;
		for (j = 0; j < spaces; j++) 	printf(" ");
		for (j = i - i % 16; j < len; j++)
		{
			if (isprint(buff[j]))	printf("%c", buff[j]);
			else					printf(".");
		}
	}
	printf(tail);
}

void ipToByteArray(int8* ip, uint8* pDes)
{
	uint32 i, ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
	int8 buff[32];
	uint32 len = (uint32)strlen(ip);
	strcpy(buff, ip);

	for (i = 0; i < len; i++)
	{
		if (buff[i] == '.')		buff[i] = ' ';
	}

	sscanf(buff, "%u %u %u %u", &ip1, &ip2, &ip3, &ip4);
	pDes[0] = ip1; pDes[1] = ip2; pDes[2] = ip3; pDes[3] = ip4;
}


int32 makeTrapVariableBindings(dataEntryType* oid_data, void* ptr, uint32* len)
{
	uint32 j;

	((uint8*)ptr)[0] = 0x30;
	((uint8*)ptr)[1] = 0xff;
	((uint8*)ptr)[2] = 0x06;
	((uint8*)ptr)[3] = oid_data->oidlen;

	for (j = 0; j < oid_data->oidlen; j++)
	{
		((uint8*)ptr)[j + 4] = oid_data->oid[j];
	}

	switch (oid_data->dataType)
	{
	case SNMPDTYPE_OCTET_STRING:
	case SNMPDTYPE_OBJ_ID:
	{
		uint8* string = &((uint8*)ptr)[4 + oid_data->oidlen + 2];

		if (oid_data->dataType == SNMPDTYPE_OCTET_STRING)
		{
			oid_data->dataLen = (uint8)strlen((int8*)&oid_data->u.octetstring);
		}
		for (j = 0; j < oid_data->dataLen; j++)
		{
			string[j] = oid_data->u.octetstring[j];
		}

		((uint8*)ptr)[4 + oid_data->oidlen] = oid_data->dataType;
		((uint8*)ptr)[4 + oid_data->oidlen + 1] = oid_data->dataLen;
		((uint8*)ptr)[1] = 2 + oid_data->oidlen + 2 + oid_data->dataLen;
		*len = 4 + oid_data->oidlen + 2 + oid_data->dataLen;
	}
	break;

	case SNMPDTYPE_INTEGER:
	case SNMPDTYPE_TIME_TICKS:
	case SNMPDTYPE_COUNTER:
	case SNMPDTYPE_GAUGE:
	{
		oid_data->dataLen = 4;

		*(int32*)(&((uint8*)ptr)[4 + oid_data->oidlen + 2]) = HTONL(oid_data->u.intval);

		((uint8*)ptr)[4 + oid_data->oidlen] = oid_data->dataType;
		((uint8*)ptr)[4 + oid_data->oidlen + 1] = oid_data->dataLen;
		((uint8*)ptr)[1] = 2 + oid_data->oidlen + 2 + oid_data->dataLen;
		*len = 4 + oid_data->oidlen + 2 + oid_data->dataLen;
	}
	break;

	default:
		return INVALID_DATA_TYPE;
	}

	return SUCCESS;
}

int32 SnmpXInit()
{
	//	initTable();
	return 0;
}

uint8 packet_trap[1024] = { 0, };
int32 SnmpXTrapSend(int8* managerIP, int8* agentIP, int8* community, dataEntryType enterprise_oid, uint32 genericTrap, uint32 specificTrap, uint32 va_count, ...)
{
	uint32 i;
	int32 packet_index = 0;
	int32 packet_buff1 = 0;
	int32 packet_buff2 = 0;
	int32 packet_buff3 = 0;
	uint8 trap_agentip[4] = { 0, };

	ipToByteArray(agentIP, trap_agentip);

	packet_trap[packet_index++] = 0x30; // ASN.1 Header

	packet_trap[packet_index] = 0xff; // pdu_length, temp
	packet_buff1 = packet_index++;

	packet_trap[packet_index++] = 0x02; // Version
	packet_trap[packet_index++] = 0x01;
	packet_trap[packet_index++] = 0x00;

	packet_trap[packet_index++] = 0x04; // Community
	packet_trap[packet_index++] = (uint8)strlen(community);
	memcpy(&(packet_trap[packet_index]), community, strlen(community));

	packet_index = packet_index + (uint8)strlen(community);

	packet_trap[packet_index++] = 0xa4; // trap
	packet_trap[packet_index] = 0xff; // length, temp
	packet_buff2 = packet_index++;

	packet_trap[packet_index++] = 0x06; // enterprise_oid
	packet_trap[packet_index++] = enterprise_oid.oidlen;
	for (i = 0; i < enterprise_oid.oidlen; i++)
	{
		packet_trap[packet_index++] = enterprise_oid.oid[i];
	}

	packet_trap[packet_index++] = 0x40; // agent ip
	packet_trap[packet_index++] = 0x04;
	packet_trap[packet_index++] = trap_agentip[0];
	packet_trap[packet_index++] = trap_agentip[1];
	packet_trap[packet_index++] = trap_agentip[2];
	packet_trap[packet_index++] = trap_agentip[3];

	packet_trap[packet_index++] = 0x02; // Generic Trap
	packet_trap[packet_index++] = 0x01;
	packet_trap[packet_index++] = (uint8)genericTrap;

	packet_trap[packet_index++] = 0x02; // Specific Trap
	packet_trap[packet_index++] = 0x01;
	packet_trap[packet_index++] = (uint8)specificTrap;

	packet_trap[packet_index++] = 0x43; // Timestamp
	packet_trap[packet_index++] = 0x01;
	packet_trap[packet_index++] = 0x00;

	packet_trap[packet_index++] = 0x30; // Sequence of variable-bindings
	packet_trap[packet_index] = 0xff;
	packet_buff3 = packet_index++;

	// variable-bindings
	{
		va_list ap;
		uint32 length_var_bindings = 0;
		uint32 length_buff = 0;

		va_start(ap, va_count);

		for (i = 0; i < va_count; i++)
		{
			dataEntryType* fff = va_arg(ap, dataEntryType*);
			makeTrapVariableBindings(fff, &(packet_trap[packet_index]), &length_buff);
			packet_index = packet_index + length_buff;
			length_var_bindings = length_var_bindings + length_buff;
		}
		packet_trap[packet_buff3] = length_var_bindings;
		va_end(ap);
	}
	packet_trap[packet_buff1] = packet_index - 2;
	packet_trap[packet_buff2] = packet_index - (9 + (uint8)strlen(community));
	{
		uint8 svr_addr[6];
		socket(SOCK_SNMP, Sn_MR_UDP, 162, 0);
		ipToByteArray(managerIP, svr_addr);
		sendto(SOCK_SNMP, packet_trap, packet_index, svr_addr, 162);

		close(SOCK_SNMP);
		return 0;
	}
}


int32 SnmpXDaemon()
{
	int32 snmpfd = 0;
	int32 fromlen = 0;
	int32 retStatus = 0;
	int32 len = 0;
	uint8 loopsnmpd = 1;
	uint8 svr_addr[6];
	uint16  svr_port;
	UNUSED(snmpfd);
	UNUSED(fromlen);
	UNUSED(retStatus);
	socket(SOCK_SNMP, Sn_MR_UDP, 161, 0);
	WDEBUG("Start SNMP Daemon(Agent) ");

	while (loopsnmpd)
	{

		if ((len = getSn_RX_RSR(SOCK_SNMP)) > 0)
		{
			request_msg.len = recvfrom(SOCK_SNMP, (uint8*)&request_msg.buffer[0], len, svr_addr, &svr_port);
		}
		else
		{
			request_msg.len = 0;
			continue;
		}
		if (request_msg.len > 0)
		{
			dumpCode("\r\n[Request]\r\n", "\r\n", request_msg.buffer, request_msg.len);

			request_msg.index = 0;
			response_msg.index = 0;
			errorStatus = errorIndex = 0;

			if (parseSNMPMessage() != -1)
			{
				sendto(SOCK_SNMP, response_msg.buffer, response_msg.index, svr_addr, svr_port);
			}

			dumpCode("\r\n[Response]\r\n", "\r\n", response_msg.buffer, response_msg.index);
		}
	}
	close(SOCK_SNMP);
	return(0);
}

