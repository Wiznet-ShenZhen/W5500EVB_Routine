#ifndef _AT_COMMAND_H_
#define _AT_COMMAND_H_
#include "stm32f10x.h"
#include "types.h"

extern uint8_t defaut_atcommand_mode[255];
extern uint8_t get_atcommand_mode[255];
extern uint8 ip_config_flag;
extern uint8 config_flag;
extern char str[];	

#define AT_DEBUG

#define	EQUAL							"="


#define AT_EXIT_OK 				2
#define AT_INPUT_OK 			3
#define AT_INPUT_EMPET 		4
#define AT_COMMAND_ERROR 	5


void atcommand_printf(void);
void list_printf(void);

uint8_t at_input_phase(void);
void process_at(void);
void switch_at_state(void);
void reset_state_flag(void);
int get_input_value(char buff[]);
char *test_strcat(char *buf);
//void str_delim( char *content,uint8_t num,char *delim,char *source)
char *str_delim(uint8_t num,char *delim,char *temp);
void at_to_AT(char *buf);
void phase_mac(char *src,	char dest[]);
char check_ver(char* src,	char *dest);
int hex2dec(char *buf);
int get_data_string(char*buff);
void get_work_time(uint32 time);

#endif	

