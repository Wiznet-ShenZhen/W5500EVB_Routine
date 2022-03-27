#ifndef _DEVICE_H_
#define _DEVICE_H_
#include "stm32f10x.h"
#include "Types.h"
#include "stdio.h"

#define FW_VER_HIGH    1
#define FW_VER_LOW     0

#define  HTTP_POST		"POST /data.php HTTP/1.1\r\n"\
						"Host: ibinhub.com\r\n"\
						"Accept: image/gif,image/x-xbitmap,image/jpeg,image/pjpeg,*/*\r\n"\
						"User-Agent: Mozilla/4.0 (compatible;MSTE 5.5;Windows 98)\r\n"\
						"Content-Length: 48\r\n"\
						"Content-Type: application/x-www-form-urlencoded\r\n"\
						"Cache-Control: no-Cache\r\n"\
						"Connection: close\r\n"\
						"\r\n"\
						"key=73a1680954cebca82a9d26cbdae084df&format=json" 			

/*
1,核对代码
2，bin_name插入位置在字符串前后,格式化字符串需要两个%s，插入中间，你就把这个字符串分成两个字符串，参数放中间就可以了
  然后格式化字符会串需要三个%s	
3，post_name在使用前需要有定义
4，格式化字符差也会输出到post_name
*/

#define  HTTP_GET(bin_name) {sprintf(get_msg,	"GET /upload/%s HTTP/1.1\r\n"\
												"Host: ibinhub.com\r\n"\
												"Connection: keep-alive\r\n"\
												"Upgrade-Insecure-Requests: 1\r\n"\
												"User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/65.0.3325.181 Safari/537.36\r\n"\
												"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"\
												"Accept-Encoding: gzip, deflate\r\n\r\n",bin_name);}   

typedef  void (*pFunction)(void);

#define CONFIG_MSG_SIZE		sizeof(CONFIG_MSG)

extern uint8 reboot_flag;

void set_default(void);
void write_config_to_eeprom(void);

void set_network(void);

void reset_w5500(void);
void reboot_app(void);
void reboot(void);

void get_config(void);
void get_stm32_ID(void);
#endif

