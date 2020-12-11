#ifndef __AUTO_UPDATE_H
#define	__AUTO_UPDATE_H

#include "types.h"
#include "w5500_conf.h"
#include "string.h"
#include "socket.h"
#include "w5500.h"
#include "utility.h"
#define  W5500_UPDATE  4
#define  AUTO_UPDATE_PORT 30000
extern uint8 update_flag;

void w5500_version(void);
char C2D(uint8 c);
uint32 ATOI32(char* str,uint16 base	);
void w5500_update(void);



#endif
