#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "stm32f10x.h"
#include "w5500.h"
#include "socket.h"
#include "config.h"
#include "device.h"
#include "ult.h"
#include "snmpLib.h"
#include "snmpDemo.h"
time_t startTime;
dataEntryType snmpData[] =
{
	// System MIB

	// SysDescr Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 1, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"WIZnet Embedded SNMP Agent"},
	NULL, NULL},

	// SysObjectID Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 2, 0},
	SNMPDTYPE_OBJ_ID, 8, {"\x2b\x06\x01\x02\x01\x01\x02\x00"},
	NULL, NULL},

	// SysUptime Entry
//	{8, {0x2b, 6, 1, 2, 1, 1, 3, 0}, 
//	SNMPDTYPE_TIME_TICKS, 0, {""},
//	currentUptime, NULL},

	// sysContact Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 4, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"support@wiznet.co.kr"},
	NULL, NULL},

	// sysName Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 5, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"http://www.wiznet.co.kr"},
	NULL, NULL},

	// Location Entry
	{8, {0x2b, 6, 1, 2, 1, 1, 6, 0},
	SNMPDTYPE_OCTET_STRING, 30, {"4F Humax Village"},
	NULL, NULL},

	// SysServices
	{8, {0x2b, 6, 1, 2, 1, 1, 7, 0},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, NULL},

	// WIZnet LED 
	{8, {0x2b, 6, 1, 4, 1, 0, 1, 0},
	SNMPDTYPE_OCTET_STRING, 30, {""},
	getWIZnetLed, NULL},

	{8, {0x2b, 6, 1, 4, 1, 0, 2, 0},
	SNMPDTYPE_INTEGER, 4, {""},
	NULL, setWIZnetLed}
};

const int32 maxData = (sizeof(snmpData) / sizeof(dataEntryType));

int32 wiznetLedStatus = 0;

void getWIZnetLed(void* ptr, uint8* len)
{
	if (wiznetLedStatus == 0)
		*len = sprintf((int8*)ptr, "LED OFF");
	else
		*len = sprintf((int8*)ptr, "LED ON");
}

void setWIZnetLed(int32 val)
{
	wiznetLedStatus = val;
	if (wiznetLedStatus == 0)
	{
		LED2 = 1; // LED in the W5500-EVB
		LED3 = 1; // LED in the W5500-EVB
	}
	else
	{
		LED2 = 0;
		LED3 = 0; // LED in the W5500-EVB
	}
}

void UserSnmpDemo(void)
{
	WDEBUG("\r\n\r\nStart UserSnmpDemo");

	SnmpXInit();

	{
		dataEntryType enterprise_oid = { 8, {0x2b, 6, 1, 4, 1, 0, 0x10, 0}, SNMPDTYPE_OBJ_ID, 8, {"\x2b\x06\x01\x04\x01\x00\x10\x00"},	NULL, NULL };

		dataEntryType trap_oid1 = { 8, {0x2b, 6, 1, 4, 1, 0, 11, 0}, SNMPDTYPE_OCTET_STRING, 30, {""}, NULL, NULL };
		dataEntryType trap_oid2 = { 8, {0x2b, 6, 1, 4, 1, 0, 12, 0}, SNMPDTYPE_INTEGER, 4, {""}, NULL, NULL };

		strcpy((int8*)trap_oid1.u.octetstring, "Alert!!!");
		trap_oid2.u.intval = 123456;

		//SnmpXTrapSend("222.98.173.250", "127.0.0.0", "public", enterprise_oid, 1, 0, 0);
		//SnmpXTrapSend("222.98.173.250", "127.0.0.0", "public", enterprise_oid, 6, 0, 2, &trap_oid1, &trap_oid2);
		SnmpXTrapSend("192.168.1.110", "192.168.1.111", "public", enterprise_oid, 1, 0, 0);
		SnmpXTrapSend("192.168.1.111", "127.0.0.0", "public", enterprise_oid, 6, 0, 2, &trap_oid1, &trap_oid2);
	}

	SnmpXDaemon();
}


