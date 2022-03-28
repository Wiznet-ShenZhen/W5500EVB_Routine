#ifndef _SNMPDEMO_H_
#define _SNMPDEMO_H_

extern dataEntryType snmpData[];
extern const int32 maxData;

void initTable(void);
void currentUptime(void *ptr, uint8 *len);
void getWIZnetLed(void *ptr, uint8 *len);
void setWIZnetLed(int32 val);
void UserSnmpDemo(void);

#endif



