#ifndef	__NTP_H__
#define	__NTP_H__
#include "types.h"
/* for ntpclient */ 
typedef signed char s_char;
typedef unsigned long long tstamp;
typedef unsigned int tdist;


typedef struct _ntpformat
{
   
        uint8  dstaddr[4];        /* destination (local) address */
        char    version;        /* version number */
        char    leap;           /* leap indicator */
        char    mode;           /* mode */
        char    stratum;        /* stratum */
        char    poll;           /* poll interval */
        s_char  precision;      /* precision */
        tdist   rootdelay;      /* root delay */
        tdist   rootdisp;       /* root dispersion */
        char    refid;          /* reference ID */
        tstamp  reftime;        /* reference time */
        tstamp  org;            /* origin timestamp */
        tstamp  rec;            /* receive timestamp */
        tstamp  xmt;            /* transmit timestamp */
      

} ntpformat;

typedef struct _datetime
{
  uint16 yy;
  uint8 mo;
  uint8 dd;
  uint8 hh;
  uint8 mm;
  uint8 ss;
} datetime;

#define ntp_port                123                     //ntp server port number
#define SECS_PERDAY     	86400UL             	// seconds in a day = 60*60*24
#define UTC_ADJ_HRS         	9              	        // SEOUL : GMT+9 
#define EPOCH                   1900                    // NTP start year
void get_seconds_from_ntp_server(uint8* buf,uint16 idx);
void ntpclient_init(void);
void do_ntp_client(void);
tstamp change_datetime_to_seconds(void);
void calc_date_time(tstamp seconds);
#endif

