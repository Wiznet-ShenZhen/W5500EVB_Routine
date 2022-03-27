#include "types.h"
#include "stm32f10x.h"
#include "w5500.h"
#include "config.h"
#include "socket.h"
#include "string.h"
#include "stdio.h"
#include "util.h"
#include "24c16.h"
#include "iap.h"
#include "util.h"
#include "device.h"

extern uint8 flg;
extern uint8 update_flag;
extern uint8 download_flag;
uint8 recv_buffer[2048];
char get_msg[1024];
uint8 tail;
uint32 data_len;
uint8 server_ip[4] = { 34,73,14,154 };	//设云平台服务器的IP地址
uint8 server_port = 80;					//设云平台服务器的端口号
uint8 Buffer[2048] = { 0 };
int8 sub[10];
uint16 rx_len;
uint32 recv_count = 0;
char bin_name[100] = { " " };

void Log(uint8* data, uint32 len)
{
	uint32 i;

	for (i = 0; i < len; i++)
	{
		printf("%c", data[i]);
	}

	printf("\r\n\r\n");
}

void w5500_version(void)
{
	char postH[1024];
	char version[10];
	int8 a[1], b[1];
	int8 error[10];
	uint32 len = 0;

	switch (getSn_SR(SOCK_HTTP))
	{
	case SOCK_ESTABLISHED:
		if (getSn_IR(SOCK_HTTP) & Sn_IR_CON)
		{
			setSn_IR(SOCK_HTTP, Sn_IR_CON);
		}
		sprintf(postH, HTTP_POST);										//字符串格式化，把格式化的数据写入POSTH字符串数组中	
		send(SOCK_HTTP, (const uint8*)postH, sizeof(postH));
		Delay_ms(1000);

		if ((len = getSn_RX_RSR(SOCK_HTTP)) > 0)		//空闲接收缓存寄存器
		{
			len = recv(SOCK_HTTP, (uint8*)recv_buffer, len);

			//平台报错
			if (strstr((char*)recv_buffer, "error"))
			{
				mid((char*)recv_buffer, "error\":\"", "\"", error);
				printf("HTTP error:%s\r\n", error);
				update_flag = 2;
			}
			//没错
			else
			{
				mid((char*)recv_buffer, "\"ver\":", ",", version);		//截取版本号
				a[0] = version[1];
				b[0] = version[3];
				//新版本号和旧版本号比较
				if ((ATOI(a, 10)) > FW_VER_HIGH)
				{
					update_flag = 1;
					mid((char*)recv_buffer, "upload/", "\",", bin_name);	//截取固件的名称										 
					HTTP_GET(bin_name);

				}
				else if ((ATOI(a, 10)) == FW_VER_HIGH)
				{
					if ((ATOI(b, 10)) > FW_VER_LOW)
					{
						update_flag = 1;
						mid((char*)recv_buffer, "upload/", "\",", (char*)bin_name);
						HTTP_GET(bin_name);
					}
					else if ((ATOI(b, 10)) == FW_VER_LOW)
					{
						update_flag = 2;
					}
					else if ((ATOI(b, 10)) < FW_VER_LOW)
					{
						update_flag = 2;
					}
					else {}
				}
				else if ((ATOI(a, 10)) < FW_VER_HIGH)
				{
					update_flag = 2;
				}
				else {}
			}
		}
		close(SOCK_HTTP);
		break;
	case SOCK_CLOSE_WAIT:
	case SOCK_CLOSED:
		socket(SOCK_HTTP, Sn_MR_TCP, 30000, Sn_MR_ND);
		break;
	case SOCK_INIT:
		connect(SOCK_HTTP, server_ip, server_port);
		break;
	}
}

void w5500_update(void)
{
	char* p;
	uint32 i = 0;
	uint32 len = 0;
	uint32 rxlen = 0;
	uint32 tmplen = 0;
	uint32 n_erased = 0;
	uint8 n_pages = 0;
	uint32 flashdest = ApplicationBackAddress;
	uint16 data;
	uint32 content_len = 0;

	switch (getSn_SR(W5500_UPDATE))
	{
	case SOCK_ESTABLISHED:

		if (getSn_IR(W5500_UPDATE) & Sn_IR_CON)
			setSn_IR(W5500_UPDATE, Sn_IR_CON);

		send(W5500_UPDATE, (const uint8*)get_msg, sizeof(get_msg));

		while (getSn_RX_RSR(W5500_UPDATE) <= 0)
			IWDG_ReloadCounter();

		printf("Receive start\r\n");

		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)
		{
			len = recv(W5500_UPDATE, Buffer, len);

			/*printf("First packet len<%d>\r\n", len);*/

			mid((char*)Buffer, "Content-Length: ", "\r\n", sub);

			content_len = ATOI32(sub, 10);

			at24c16_write(210, ((content_len & 0xff000000) >> 24));
			at24c16_write(211, ((content_len & 0x00ff0000) >> 16));
			at24c16_write(212, ((content_len & 0x0000ff00) >> 8));
			at24c16_write(213, content_len & 0x000000ff);

			Flash_Erase(ApplicationBackAddress, content_len);

			p = strstr((char*)Buffer, "\r\n\r\n") + 4;

			tmplen = len - ((uint8*)p - Buffer);

			/*printf("First packet data<%ld>\r\n", tmplen);*/

			printf("Receiving\r\n");

			rxlen = 0;

			Flash_Write_Ready();

			while (rxlen != content_len)
			{
				IWDG_ReloadCounter();

				//Need to write
				if (tmplen > 0)
				{
					Flash_Write(ApplicationBackAddress + rxlen, (uint8*)p, tmplen);

					rxlen += tmplen;
					tmplen = 0;

					/*printf("<%ld> / <%ld>\r\n", rxlen, content_len);*/

					printf(">");
				}

				len = getSn_RX_RSR(W5500_UPDATE);

				//Check receive pub
				if (len >= 0x200 || (len + rxlen) >= content_len)
				{
					//Since "Bad request info may follow behind data...."
					if ((len + rxlen) > content_len)
						len = content_len - rxlen;

					memset(Buffer, 0x00, len);

					tmplen = recv(W5500_UPDATE, Buffer, len);

					p = (char*)Buffer;
				}
			}

			Flash_Write_Done();

			printf("\r\nDone\r\n");

			//Drop useless data

			printf("Cleaning\r\n");

			len = 500;

			while (len--)
			{
				if (getSn_RX_RSR(W5500_UPDATE))
					recv(W5500_UPDATE, Buffer, sizeof(Buffer));
			}

			printf("Cleaning Done\r\n");

			download_flag = 1;
		}

		/*
		if ((len = getSn_RX_RSR(W5500_UPDATE)) > 0)
		{
			len = recv(W5500_UPDATE, (uint8*)Buffer, len);
			mid((char*)Buffer, "Content-Length: ", "\r\n", sub);
			p = strstr((char*)Buffer, "\r\n\r\n");
			tmplen = len - (p - (char*)Buffer) - 4;
			content_len = ATOI32(sub, 10);
			at24c16_write(210, ((content_len & 0xff000000) >> 24));
			at24c16_write(211, ((content_len & 0x00ff0000) >> 16));
			at24c16_write(212, ((content_len & 0x0000ff00) >> 8));
			at24c16_write(213, content_len & 0x000000ff);

			while (rxlen != content_len)
			{
				IWDG_ReloadCounter();

				if (rxlen == 0)
				{
					printf("updating");
					FLASH_Unlock();
					FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
					n_pages = FLASH_PagesMask(content_len);
					for (n_erased = 0; n_erased <= n_pages; n_erased++)
					{
						FLASH_ErasePage(ApplicationBackAddress + 0x400 * n_erased);
						IWDG_ReloadCounter();
					}
					if ((tmplen % 2) != 0)
					{
						flg = 1;
						tail = Buffer[len - 1];
					}
					for (i = 0; i < tmplen - 1; i = i + 2)
					{
						data = *(p + 4 + i + 1);
						data = (data << 8) + (*(p + 4 + i));
						FLASH_ProgramHalfWord(flashdest, data);
						recv_count++;
						flashdest += 2;
					}
					rxlen = tmplen;
					tmplen = 0;
				}
				else if (rxlen > 0)
				{
					memset(Buffer, 0xff, 2048);
					tmplen = getSn_RX_RSR(W5500_UPDATE);

					if (tmplen > 0)
					{
						if (flg == 1)
						{
							tmplen = recv(W5500_UPDATE, (uint8*)(Buffer + 1), tmplen);

							Buffer[0] = tail;
							//	flg=0;
							if (((tmplen + 1) % 2) != 0)
							{
								flg = 1;
								tail = Buffer[tmplen];
								//printf("tail:%d\r\n",tail);
								data_len = tmplen + 1;
							}
							else
							{
								flg = 0;
								tail = 0;
								data_len = tmplen + 1;
							}
						}
						else
						{
							tmplen = recv(W5500_UPDATE, (uint8*)Buffer, tmplen);
							if ((tmplen % 2) != 0)
							{
								flg = 1;
								tail = Buffer[tmplen - 1];

							}
							data_len = tmplen;
						}
						if ((rxlen + tmplen) == content_len)
						{

							data_len = data_len + 2;
						}
						printf(".");

						for (i = 0; i < data_len - 1; i += 2)
						{
							data = Buffer[i + 1];
							data = (data << 8) + Buffer[i];
							FLASH_ProgramHalfWord(flashdest, data);
							flashdest += 2;
							recv_count++;
						}
						rxlen += tmplen;
						tmplen = 0;
					}
				}
			}

			printf("done\r\n");

			printf("\r\n\r\n");
			FLASH_Lock();
			download_flag = 1;
		}
		*/
		//				update_flag = 2;
		break;

	case SOCK_CLOSE_WAIT:

		break;

	case SOCK_CLOSED:

		socket(W5500_UPDATE, Sn_MR_TCP, 30000, Sn_MR_ND);

		break;

	case SOCK_INIT:

		connect(W5500_UPDATE, server_ip, server_port);

		break;
	}
}
