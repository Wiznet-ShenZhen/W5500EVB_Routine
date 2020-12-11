#include <string.h>
#include "config.h"
#include "flash.h"
#include "tftp.h"
#include "ult.h"
#include "w5500.h"
#include "socket.h"

/* Global Variable ----------------------------------------------*/
static int		g_tftp_socket = -1;

static uint8_t g_filename[FILE_NAME_SIZE];

static uint32_t g_server_ip = 0;
static uint16_t g_server_port = 0;
static uint16_t g_local_port = 0;

static uint32_t g_tftp_state = STATE_NONE;
static uint16_t g_block_num = 0;

static uint32_t g_timeout = 5;
static uint32_t g_resend_flag = 0;
static uint32_t tftp_time_cnt = 0;
static uint32_t tftp_retry_cnt = 0;

static uint8_t *g_tftp_rcv_buf = NULL;

uint16 total_len;
static TFTP_OPTION default_tftp_opt;
//{
//	.code = (uint8_t *)"timeout",
//	.value = (uint8_t *)"5"
//};

uint8_t g_progress_state = TFTP_PROGRESS;

/* static function define ---------------------------------------*/
static void set_filename(uint8_t *file, uint32_t file_size)
{
	memcpy(g_filename, file, file_size);
}

void set_server_ip(uint32_t ipaddr)
{
	g_server_ip = ipaddr;
}

uint32_t get_server_ip()
{
	return g_server_ip;
}

void set_server_port(uint16_t port)
{
	g_server_port = port;
}

uint16_t get_server_port()
{
	return g_server_port;
}

void set_local_port(uint16_t port)
{
	g_local_port = port;
}

uint16_t get_local_port()
{
	return g_local_port;
}

uint16_t genernate_port()
{
	/* TODO */
	return 0;
}

void set_tftp_state(uint32_t state)
{
	g_tftp_state = state;
}

uint32_t get_tftp_state()
{
	return g_tftp_state;
}

void set_tftp_timeout(uint32_t timeout)
{
	g_timeout = timeout;
}

uint32_t get_tftp_timeout()
{
	return g_timeout;
}

void set_block_number(uint16_t block_number)
{
	g_block_num = block_number;
}

uint16_t get_block_number()
{
	return g_block_num;
}

int open_tftp_socket(uint8_t sock)
{
	uint8_t sd, sck_state;
	sd = socket(sock, Sn_MR_UDP, 51000,0);
	if(sd != 1) 
	{
		printf("[%s] socket error\r\n", __func__);
		return -1;
	}
	do
	{
		sck_state=getSn_SR(sd);
//		printf(" %2x",getSn_SR(sd));
	} 	
	while(sck_state != SOCK_UDP);
	return sd;
}

int send_udp_packet(int socket, uint8_t *packet, uint32_t len, uint32_t ip, uint16_t port)
{
	int snd_len;
	ip = htonl(ip);
	snd_len = sendto(socket, packet, len, (uint8_t *)&ip, port);
	if(snd_len != len) 
	{
		//DBG_PRINT(ERROR_DBG, "[%s] sendto error\r\n", __func__);
		return -1;
	}
	return snd_len;
}

int recv_udp_packet(int socket, uint8_t *packet, uint32_t len, uint32_t *ip, uint16_t *port)
{
	int ret;
	uint8_t sck_state;
	uint16_t recv_len;
	/* Receive Packet Process */
	sck_state=getSn_SR(socket);
//	printf(" %2x\r\n",sck_state);
	if(sck_state == SOCK_UDP) 
	{
		recv_len=getSn_RX_RSR(socket);
		if(recv_len) 
		{
			recv_len = recvfrom(socket, packet, len, (uint8_t *)ip, port);
			if(recv_len < 0) 
			{
				printf("[%s] recvfrom error\r\n", __func__);
				return -1;
			}
			*ip = ntohl(*ip);
			return recv_len;
		}
	}
	return -1;
}

void close_tftp_socket(int socket)
{
	close(socket);
}


void init_tftp(void)
{
	g_filename[0] = 0;

	set_server_ip(0);
	set_server_port(0);
	set_local_port(0);

	set_tftp_state(STATE_NONE);
	set_block_number(0);

	/* timeout flag */
	g_resend_flag = 0;
	tftp_retry_cnt = tftp_time_cnt = 0;

	g_progress_state = TFTP_PROGRESS;
}

void tftp_cancel_timeout(void)
{
	if(g_resend_flag) 
{
		g_resend_flag = 0;
		tftp_retry_cnt = tftp_time_cnt = 0;
	}
}

void tftp_reg_timeout()
{
	if(g_resend_flag == 0) 
	{
		g_resend_flag = 1;
		tftp_retry_cnt = tftp_time_cnt = 0;
	}
}

void process_tftp_option(uint8_t *msg, uint32_t msg_len)
{
	/* TODO Option Process */
}

void send_tftp_rrq(uint8_t *filename, uint8_t *mode, TFTP_OPTION *opt, uint8_t opt_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t i, len;

	*((uint16_t *)pkt) = htons(TFTP_RRQ);
	pkt += 2;
	strcpy((char *)pkt, (const char *)filename);
	pkt += strlen((char *)filename) + 1;
	strcpy((char *)pkt, (const char *)mode);
	pkt += strlen((char *)mode) + 1;

	for(i = 0 ; i < opt_len ; i++) {
		strcpy((char *)pkt, (const char *)opt[i].code);
		pkt += strlen((char *)opt[i].code) + 1;
		strcpy((char *)pkt, (const char *)opt[i].value);
		pkt += strlen((char *)opt[i].value) + 1;
	}

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket,  snd_buf, len, get_server_ip(), TFTP_SERVER_PORT);
	set_tftp_state(STATE_RRQ);
	set_filename(filename, strlen((char *)filename) + 1);
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	printf(">> TFTP RRQ : FileName(%s), Mode(%s)\r\n", filename, mode);
#endif
}

#if 0	// 2014.07.01 sskim
void send_tftp_wrq(uint8_t *filename, uint8_t *mode, TFTP_OPTION *opt, uint8_t opt_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t i, len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_WRQ);
	pkt += 2;
	strcpy((char *)pkt, (const char *)filename);
	pkt += strlen((char *)filename) + 1;
	strcpy((char *)pkt, (const char *)mode);
	pkt += strlen((char *)mode) + 1;

	for(i = 0 ; i < opt_len ; i++) {
		strcpy((char *)pkt, (const char *)opt[i].code);
		pkt += strlen((char *)opt[i].code) + 1;
		strcpy((char *)pkt, (const char *)opt[i].value);
		pkt += strlen((char *)opt[i].value) + 1;
	}

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), TFTP_SERVER_PORT);
	set_tftp_state(STATE_WRQ);
	set_filename(filename, strlen((char *)filename) + 1);
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP WRQ : FileName(%s), Mode(%s)\r\n", filename, mode);
#endif
}
#endif

#if 0	// 2014.07.01 sskim
void send_tftp_data(uint16_t block_number, uint8_t *data, uint16_t data_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_DATA);
	pkt += 2;
	*((uint16_t *)pkt) = htons(block_number);
	pkt += 2;
	memcpy(pkt, data, data_len);
	pkt += data_len;

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	DBG_PRINT(IPC_DBG, ">> TFTP DATA : Block Number(%d), Data Length(%d)\r\n", block_number, data_len);
#endif
}
#endif

void send_tftp_ack(uint16_t block_number)
{
	uint8_t snd_buf[4];
	uint8_t *pkt = snd_buf;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_ACK);
	pkt += 2;
	*((uint16_t *)pkt) = htons(block_number);
	pkt += 2;

	send_udp_packet(g_tftp_socket , snd_buf, 4, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
//	printf(">> TFTP ACK : Block Number(%d)\r\n", block_number);
#endif
}

#if 0	// 2014.07.01 sskim
void send_tftp_oack(TFTP_OPTION *opt, uint8_t opt_len)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t i, len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_OACK);
	pkt += 2;

	for(i = 0 ; i < opt_len ; i++) {
		strcpy((char *)pkt, (const char *)opt[i].code);
		pkt += strlen((char *)opt[i].code) + 1;
		strcpy((char *)pkt, (const char *)opt[i].value);
		pkt += strlen((char *)opt[i].value) + 1;
	}

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	printf(">> TFTP OACK \r\n");
#endif
}
#endif

#if 0	// 2014.07.01 sskim
void send_tftp_error(uint16_t error_number, uint8_t *error_message)
{
	uint8_t snd_buf[MAX_MTU_SIZE];
	uint8_t *pkt = snd_buf;
	uint32_t len;

	*((uint16_t *)pkt) = htons((uint16_t)TFTP_ERROR);
	pkt += 2;
	*((uint16_t *)pkt) = htons(error_number);
	pkt += 2;
	strcpy((char *)pkt, (const char *)error_message);
	pkt += strlen((char *)error_message) + 1;

	len = pkt - snd_buf;

	send_udp_packet(g_tftp_socket , snd_buf, len, get_server_ip(), get_server_port());
	tftp_reg_timeout();
#ifdef __TFTP_DEBUG__
	printf(">> TFTP ERROR : Error Number(%d)\r\n", error_number);
#endif
}
#endif

void recv_tftp_rrq(uint8_t *msg, uint32_t msg_len)
{
	/* When TFTP Server Mode */
}

void recv_tftp_wrq(uint8_t *msg, uint32_t msg_len)
{
	/* When TFTP Server Mode */
}

void recv_tftp_data(uint8_t *msg, uint32_t msg_len)
{
	TFTP_DATA_T *data = (TFTP_DATA_T *)msg;

	data->opcode = ntohs(data->opcode);
	data->block_num = ntohs(data->block_num);
	#ifdef __TFTP_DEBUG__
//	printf("<< TFTP_DATA : opcode(%d), block_num(%d)\r\n", data->opcode, data->block_num);
	#endif

	switch(get_tftp_state())
	{
		case STATE_RRQ :
		case STATE_OACK :
			if(data->block_num == 1) 
			{
				set_tftp_state(STATE_DATA);
				set_block_number(data->block_num);
		//		printf(" data %s\r\n",(uint8 *)data->data);					
				save_data(data->data, msg_len - 4, data->block_num);
				tftp_cancel_timeout();
			}
			send_tftp_ack(data->block_num);
			break;
			
		case STATE_DATA :
			//	printf(" STATE_DATA\r\n");
			if(data->block_num == (get_block_number() + 1)) 
			{
				set_block_number(data->block_num);
				save_data(data->data, msg_len - 4, data->block_num);				
			//	printf(" data %s\r\n",(uint8 *)data->data);				
				tftp_cancel_timeout();
			}
			send_tftp_ack(data->block_num);

			if((msg_len - 4) < TFTP_BLK_SIZE) 
			{
				init_tftp();				
				g_progress_state = TFTP_SUCCESS;
				//printf(" TFTP_SUCCESS\r\n");
			}
			break;
		default :
			/* invalid message */
			break;
	}
}

void recv_tftp_ack(uint8_t *msg, uint32_t msg_len)
{
#ifdef __TFTP_DEBUG__
	printf("<< TFTP_ACK : \r\n");
#endif

	switch(get_tftp_state())
	{
		case STATE_WRQ :
			break;
		case STATE_ACK :
			break;
		default :
			/* invalid message */
			break;
	}
}

void recv_tftp_oack(uint8_t *msg, uint32_t msg_len)
{
#ifdef __TFTP_DEBUG__
	printf("<< TFTP_OACK : \r\n");
#endif

	switch(get_tftp_state())
	{
		case STATE_RRQ :
			process_tftp_option(msg, msg_len);	
			set_tftp_state(STATE_OACK);
			tftp_cancel_timeout();
			send_tftp_ack(0);
			break;

		case STATE_WRQ :
			process_tftp_option(msg, msg_len);	
			set_tftp_state(STATE_ACK);
			tftp_cancel_timeout();

			/* TODO DATA Transfer */
			//send_tftp_data(...);
			break;

		default :
			/* invalid message */
			break;
	}
}

void recv_tftp_error(uint8_t *msg, uint32_t msg_len)
{
	TFTP_ERROR_T *data= (TFTP_ERROR_T *)msg;

	data->opcode = ntohs(data->opcode);
	data->error_code = ntohs(data->error_code);

#ifdef __TFTP_DEBUG__
	printf("<< TFTP_ERROR : %d (%s)\r\n", data->error_code, data->error_msg);
	printf("[%s] Error Code : %d (%s)\r\n", __func__, data->error_code, data->error_msg);
#endif
	init_tftp();
	g_progress_state = TFTP_FAIL;
}

void recv_tftp_packet(uint8_t *packet, uint32_t packet_len, uint32_t from_ip, uint16_t from_port)
{
	uint16_t opcode;
	/* Verify Server IP */
	if(from_ip != get_server_ip()) 
	{
		#ifdef __TFTP_DEBUG__
		printf("[%s] Server IP faults\r\n", __func__);
		printf("from IP : %08x, Server IP : %08x\r\n", from_ip, get_server_ip());
		#endif
		return;
	}
	opcode = ntohs(*((uint16_t *)packet));
	/* Set Server Port */
	if((get_tftp_state() == STATE_WRQ) || (get_tftp_state() == STATE_RRQ)) 
	{
		set_server_port(from_port);
		#ifdef __TFTP_DEBUG__
		printf("[%s] Set Server Port : %d\r\n", __func__, from_port);
		#endif
	}
	switch(opcode)
	{
		case TFTP_RRQ :						/* When Server */
			recv_tftp_rrq(packet, packet_len);
			break;
		case TFTP_WRQ :						/* When Server */
			recv_tftp_wrq(packet, packet_len);
			break;
		case TFTP_DATA :
			recv_tftp_data(packet, packet_len);
			break;
		case TFTP_ACK :
			recv_tftp_ack(packet, packet_len);
			break;
		case TFTP_OACK :
			recv_tftp_oack(packet, packet_len);
			break;
		case TFTP_ERROR :
			recv_tftp_error(packet, packet_len);
			break;

		default :
			// Unknown Mesage
			break;
	}
}

/* Functions ----------------------------------------------------*/
void TFTP_init(uint8_t socket, uint8_t *buf)
{
	init_tftp();

	g_tftp_socket = open_tftp_socket(socket);
	g_tftp_rcv_buf = buf;
}

void TFTP_exit(void)
{
	init_tftp();

	close_tftp_socket(g_tftp_socket);
	g_tftp_socket = -1;

	g_tftp_rcv_buf = NULL;
}

int TFTP_run(void)
{
	uint16_t len, from_port;
	uint32_t from_ip;

	/* Timeout Process */
	if(g_resend_flag) 
	{
		if(tftp_time_cnt >= g_timeout) 
		{
			switch(get_tftp_state()) 
			{
			case STATE_WRQ:				
				break;

			case STATE_RRQ:
				send_tftp_rrq(g_filename, (uint8_t *)TRANS_BINARY, &default_tftp_opt, 1);
				break;
			case STATE_OACK:
			case STATE_DATA:
				send_tftp_ack(get_block_number());
				break;
			case STATE_ACK:				
				break;
			default:
				break;
			}
			tftp_time_cnt = 0;
			tftp_retry_cnt++;
			if(tftp_retry_cnt >= 5) 
			{
				init_tftp();
				g_progress_state = TFTP_FAIL;
			}
		}
	}

	/* Receive Packet Process */
	len = recv_udp_packet(g_tftp_socket, g_tftp_rcv_buf, MAX_MTU_SIZE, &from_ip, &from_port);
	if(len < 0) 
	{
	#ifdef __TFTP_DEBUG__
		printf("[%s] recv_udp_packet error\r\n", __func__);
	#endif
		return g_progress_state;
	}

	recv_tftp_packet(g_tftp_rcv_buf, len, from_ip, from_port);
	return g_progress_state;
}

void TFTP_read_request(uint32_t server_ip, uint8_t *filename)
{
	set_server_ip(server_ip);
	#ifdef __TFTP_DEBUG__
	printf("[%s] Set Tftp Server : %x\r\n", __func__, server_ip);
	#endif
	g_progress_state = TFTP_PROGRESS;
	send_tftp_rrq(filename, (uint8_t *)TRANS_BINARY, &default_tftp_opt, 1);
}

void tftp_timeout_handler(void)
{
	if(g_resend_flag) 
		tftp_time_cnt++;
}

static uint32 FLASH_BASE_ADDR = ApplicationAddress;
void save_data(uint8_t *data, uint32_t data_len, uint16_t block_number)
{
	static uint32_t cnt = 0;
	uint16 temp;
 uint16 i;
	if(block_number == 1)
	{
		FLASH_ErasePage(FLASH_BASE_ADDR);//擦除整页;	
	}
	cnt += data_len;
  FLASH_Unlock();	
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
//	FLASH_ErasePage(FLASH_BASE_ADDR);//擦除整页
	if(data_len < TFTP_BLK_SIZE)
	{
		memset(data + data_len, 0xff, TFTP_BLK_SIZE - data_len);
		for(i=0;i<TFTP_BLK_SIZE;i+=2)
    {
			 temp=((uint16)data[i+1]<<8)+(uint16)data[i];
			//	temp=(u16)data[i+1]<<8;
			//	temp+=(u16)data[i];
       FLASH_ProgramHalfWord(FLASH_BASE_ADDR,temp);
       FLASH_BASE_ADDR+=2;//地址增加2. 
    }
//		printf("\r\nwrite flash %d bytes\r\n", cnt);			
		FLASH_BASE_ADDR = ApplicationAddress ;
		cnt = 0;
	}
	else
	{
		for(i=0;i<data_len;i+=2)
		{
		// temp=(uint16)(data[i+1]*256+data[i]);
			temp=(u16)data[i+1]<<8;
			temp+=(u16)data[i];
			FLASH_ProgramHalfWord(FLASH_BASE_ADDR,temp);
			FLASH_BASE_ADDR+=2;//地址增加2. 
		}
	}
	FLASH_Lock();//上锁
}

