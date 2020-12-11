#ifndef __FW_UPDATA_H
#define	__FW_UPDATA_H

#include "types.h"
#include "w5500_conf.h"
#include "string.h"
#include "socket.h"
#include "w5500.h"
#define TCP_RE_PORT 30000
#define SOCK_FW 1

void do_fw_update(void);
#endif
