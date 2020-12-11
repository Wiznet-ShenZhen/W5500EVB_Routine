#include "config.h"

CONFIG_MSG  ConfigMsg, RecvMsg;
void set_default(void)
{
  //set default mac
  memcpy(ConfigMsg.mac, mac,6);
  memcpy(ConfigMsg.lip, local_ip, 4);
  memcpy(ConfigMsg.sub,  subnet, 4);
  memcpy(ConfigMsg.gw,  gateway, 4);

  ConfigMsg.sw_ver[1]=FW_VER_LOW;
  ConfigMsg.sw_ver[0]=FW_VER_HIGH;
  ConfigMsg.debug = 1;
  
  write_config_to_flash();
}



