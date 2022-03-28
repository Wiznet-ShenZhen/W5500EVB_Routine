
/* Includes ------------------------------------------------------------------*/
#include "hyperterminal.h"
#include "stm32f10x.h"
#include "config.h"
#include "ult.h"
#include "W5500.h"
#include "socket.h"
#include "UPnP.h"
#include "lockback.h"
#include "device.h"
#include <stdio.h>

uint16 TCP_LISTEN_PORT=5000;	
//uint16 UDP_LISTEN_PORT;	
//uint16 num;
//uint16 num1;
/*******************************************************************************
* Function Name  : Int2Str
* Description    : Convert an Integer to a string
* Input          : - str: The string
*                  - intnum: The intger to be converted
* Output         : None
* Return         : None
*******************************************************************************/
void Int2Str(char *str, u32 intnum)
{
    int i, Div = 1000000000, j = 0, Status = 0;
    for (i = 0; i < 10; i++)
    {
        str[j++] = (intnum / Div) + 48;
        intnum = intnum % Div;
        Div /= 10;
        if ((str[j-1] == '0') & (Status == 0))
        {
            j = 0;
        }
        else
        {
            Status++;
        }
    }
}

/*******************************************************************************
* Function Name  : Str2Int
* Description    : Convert a string to an integer
* Input 1        : - inputstr: The string to be converted
*                  - intnum: The intger value
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u8 Str2Int(char *inputstr, u32 *intnum)
{
    u8 i = 0, res = 0;
    u32 val = 0;
    if (inputstr[0] == '0' && (inputstr[1] == 'x' || inputstr[1] == 'X'))
    {
        if (inputstr[2] == '\0')
        {
            return 0;
        }
        for (i = 2; i < 11; i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                res = 1; /* return 1; */
                break;
            }
            if (ISVALIDHEX(inputstr[i]))
            {
                val = (val << 4) + CONVERTHEX(inputstr[i]);
            }
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11) res = 0; /* over 8 digit hex --invalid */
    }
    else /* max 10-digit decimal input */
    {
        for (i = 0;i < 11;i++)
        {
            if (inputstr[i] == '\0')
            {
                *intnum = val;
                /* return 1; */
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'k' || inputstr[i] == 'K') && (i > 0))
            {
                val = val << 10;
                *intnum = val;
                res = 1;
                break;
            }
            else if ((inputstr[i] == 'm' || inputstr[i] == 'M') && (i > 0))
            {
                val = val << 20;
                *intnum = val;
                res = 1;
                break;
            }
            else if (ISVALIDDEC(inputstr[i]))
                val = val * 10 + CONVERTDEC(inputstr[i]);
            else
            {
                /* return 0; Invalid input */
                res = 0;
                break;
            }
        }
        if (i >= 11) res = 0; /* Over 10 digit decimal --invalid */
    }
    return res;
}

/*******************************************************************************
* Function Name  : GetIntegerInput
* Description    : Get an integer from the HyperTerminal
* Input          : - num: The inetger
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u8 GetIntegerInput(u32 *num)
{
    char inputstr[16];
    while (1)
    {
        GetInputString(inputstr);
        if (inputstr[0] == '\0') continue;
        if ((inputstr[0] == 'a' || inputstr[0] == 'A') && inputstr[1] == '\0')
        {
            SerialPutString("User Cancelled \r\n");
            return 0;
        }
        if (Str2Int(inputstr, num) == 0)
        {
            SerialPutString("Error, Input again: \r\n");
        }
        else
        {
            return 1;
        }
    }
}

/*******************************************************************************
* Function Name  : SerialKeyPressed
* Description    : Test to see if a key has been pressed on the HyperTerminal
* Input          : - key: The key pressed
* Output         : None
* Return         : 1: Correct
*                  0: Error
*******************************************************************************/
u8 SerialKeyPressed(char *key)
{
	FlagStatus flag  ;
	/* First clear Rx buffer */
	flag = USART_GetFlagStatus(USART1, USART_FLAG_RXNE);
	if ( flag == SET)
	{
		*key = (char)USART1->DR;
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
* Function Name  : GetKey
* Description    : Get a key from the HyperTerminal
* Input          : None
* Output         : None
* Return         : The Key Pressed
*******************************************************************************/
char GetKey(void)
{
	char key = 0;
	/* Waiting for user input */
	while (1)
	{
		if (SerialKeyPressed((char*)&key)) break;
	}
	return key;
}

/*******************************************************************************
* Function Name  : SerialPutChar
* Description    : Print a character on the HyperTerminal
* Input          : - c: The character to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutChar(char c)
{
    USART_SendData(USART1, c);
//	 num=c;
    while ((USART1->SR & USART_SR_TXE ) != USART_SR_TXE );
}

/*******************************************************************************
* Function Name  : SerialPutString
* Description    : Print a string on the HyperTerminal
* Input          : - s: The string to be printed
* Output         : None
* Return         : None
*******************************************************************************/
void SerialPutString(char *s)
{
	while (*s != '\0')
	{
		SerialPutChar(*s);
		s ++;
	}
}

/*******************************************************************************
* Function Name  : GetInputString
* Description    : Get Input string from the HyperTerminal
* Input          : - buffP: The input string
* Output         : None
* Return         : None
*******************************************************************************/
void GetInputString (char *buffP)
{
	int bytes_read = 0;
	char c = 0;
	do
	{
		c = GetKey();
		if (c == '\r')
      break;
    if (c == '\b') /* Backspace */
    {
      if (bytes_read > 0)
      {
        SerialPutString("\b \b");
        bytes_read --;
      }
      continue;
    }
    if (bytes_read >= CMD_STRING_SIZE )
    {
      SerialPutString("Command string size overflow\r\n");
      bytes_read = 0;
      continue;
    }
    if (c >= 0x20 && c <= 0x7E)
    {
      buffP[bytes_read++] = c;
      SerialPutChar(c);
		}
	}
  while (1);
  SerialPutString("\n\r");
  buffP[bytes_read] = '\0';
}


char* STRTOK(char* strToken, const char* strDelimit)
{
       static char* pCurrent;
       char* pDelimit;
 
       if ( strToken != NULL )
             pCurrent = strToken;
       else
             strToken = pCurrent;
 
//       if ( *pCurrent == NULL ) return NULL;
 
       while ( *pCurrent )
       {
             pDelimit = (char*)strDelimit;
             while ( *pDelimit )
             {
                    if ( *pCurrent == *pDelimit )
                    {
                           //*pCurrent = NULL;
                           *pCurrent = 0;
                           ++pCurrent;
                           return strToken;
                    }
                    ++pDelimit;
             }
             ++pCurrent;
       }
 
       return strToken;
}


/*******************************************************************************
* Function Name  : Main_Menu
* Description    : Display/Manage a Menu on HyperTerminal Window
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Main_Menu(void)
{
	static char choice[3];
	static char msg[256], ipaddr[12], protocol[4];
	static unsigned short ret, external_port, internal_port;
	static bool bTreat;
	static u8 Sip[4];
	static char key = 0;
//	static wiz_NetInfo netinfo;


	while (1)
	{
		/* Display Menu on HyperTerminal Window */
		bTreat = (bool)RESET ;
   	        SerialPutString("\r\n====================== W5500_Control_Point ===================\r\n");
		SerialPutString("This Application is basic example of UART interface with\r\n");
		SerialPutString("Windows Hyper Terminal. \r\n");
		SerialPutString("\r\n==========================================================\r\n");
		SerialPutString("                          APPLICATION MENU :\r\n");
		SerialPutString("\r\n==========================================================\r\n\n");
		SerialPutString(" 1 - Set LD1 on \r\n");
		SerialPutString(" 2 - Set LD1 off \r\n");
		SerialPutString(" 3 - Show network setting\r\n");
		SerialPutString(" 4 - Set  network setting\r\n");
		SerialPutString(" 5 - Run TCP Loopback\r\n");
		SerialPutString(" 6 - Run UDP Loopback\r\n");
		SerialPutString(" 7 - UPnP PortForwarding: AddPort\r\n");
		SerialPutString(" 8 - UPnP PortForwarding: DeletePort\r\n");
		
		SerialPutString("Enter your choice : ");
		GetInputString(choice);
		/* Set LD1 on */
		if (strcmp(choice,"1")== 0)
		{
		bTreat = (bool)SET;
	            LED1=0;
              LED2=0;
		}
		/* Set LD1 off */
		if ((strcmp(choice,"2") == 0))
		{
			bTreat = (bool)SET;
                LED1=1;
                LED2=1;
		}
		if (strcmp(choice,"3") == 0)
		{
		bTreat = (bool)SET;
                          set_network(); 
		}

		if (strcmp(choice,"4") == 0)
		{
			bTreat = (bool)SET;
			// IP address
			SerialPutString("\r\nIP address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, ConfigMsg.lip))
			{
				SerialPutString("\aInvalid.");
			}

			// Subnet mask
			SerialPutString("\r\nSubnet mask : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, ConfigMsg.sub))
			{
				SerialPutString("\aInvalid.");
			}
			
			// gateway address
			SerialPutString("\r\nGateway address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, ConfigMsg.gw))
			{
				SerialPutString("\aInvalid.");
			}

			// DNS address
			SerialPutString("\r\nDNS address : ");
			GetInputString(msg);
			if(!VerifyIPAddress(msg, ConfigMsg.dns))
			{
				SerialPutString("\aInvalid.");
			}

            printf("\r\nIP : %d.%d.%d.%d",ConfigMsg.lip[0],ConfigMsg.lip[1],ConfigMsg.lip[2],ConfigMsg.lip[3]);
			printf("\r\nSN : %d.%d.%d.%d", ConfigMsg.sub[0],ConfigMsg.sub[1],ConfigMsg.sub[2],ConfigMsg.sub[3]);
			printf("\r\nGW : %d.%d.%d.%d",ConfigMsg.gw[0],ConfigMsg.gw[1],ConfigMsg.gw[2],ConfigMsg.gw[3]);
			printf("\r\nDNS server : %d.%d.%d.%d", ConfigMsg.dns[0],ConfigMsg.dns[1],ConfigMsg.dns[2],ConfigMsg.dns[3]);
                                      setSHAR(ConfigMsg.mac);/*ÅäÖÃMacµØÖ·*/
                                      setSUBR(ConfigMsg.sub);/*ÅäÖÃ×ÓÍøÑÚÂë*/
                                      setGAR(ConfigMsg.gw);/*ÅäÖÃÄ¬ÈÏÍø¹Ø*/
                                      setSIPR(ConfigMsg.lip);/*ÅäÖÃIpµØÖ·*/
		}
		

		if (strcmp(choice,"5") == 0)
		{
			bTreat = (bool)SET;

			SerialPutString("\r\nRun TCP loopback");
			printf("\r\nRun TCP loopback, port number [%d] is listened", (u16)TCP_LISTEN_PORT);
			SerialPutString("\r\nTo Exit, press [Q]");

			while(1) {

				if ((SerialKeyPressed((char*)&key) == 1) && (key == 'Q')) {
					SerialPutString("\r\n Stop ");
					break;
				}

				loopback_tcps(7, (u16)TCP_LISTEN_PORT);
			}

		}


		if (strcmp(choice,"6") == 0)
		{
			bTreat = (bool)SET;
		  
			SerialPutString("\r\nRun UDP loopback");
			printf("\r\nRun UDP loopback, port number [%d] is listened", (u16)UDP_LISTEN_PORT);
			SerialPutString("\r\nTo Exit, press [Q]");
			
			while(1) 
			{

				if ((SerialKeyPressed((char*)&key) == 1) && (key == 'Q')) {
					SerialPutString("\r\n Stop ");				
					break;
				}
				loopback_udp(7, (u16)UDP_LISTEN_PORT);
			}

		}
		if (strcmp(choice,"7")== 0)
		{
		  	bTreat = (bool)SET;
			
			SerialPutString("\r\nType a Protocol(TCP/UDP) : ");
			GetInputString(msg);
			strncpy(protocol, msg, 3);
			protocol[3] = '\0';

			SerialPutString("\r\nType a External Port Number : ");
			GetInputString(msg);
			external_port = ATOI(msg, 10);

			SerialPutString("\r\nType a Internal Port Number : ");
			GetInputString(msg);
			internal_port = ATOI(msg, 10);
			TCP_LISTEN_PORT=internal_port;
			close(7);
			// Try to Add Port Action
			getSIPR(Sip);
			sprintf(ipaddr, "%d.%d.%d.%d", Sip[0], Sip[1], Sip[2], Sip[3]);
			if((ret = AddPortProcess(SOCK_UPNP, protocol, external_port, ipaddr, internal_port, "W5500_uPnPGetway"))==0) printf("AddPort Success!!\r\n");
			else printf("AddPort Error Code is %d\r\n", ret);

		}


		if (strcmp(choice,"8")== 0)
		{
			bTreat = (bool)SET;
			
			SerialPutString("\r\nType a Protocol(TCP/UDP) : ");
			GetInputString(msg);
			strncpy(protocol, msg, 3);
			protocol[3] = '\0';

			SerialPutString("\r\nType a External Port Number : ");
			
//			TCP_LISTEN_PORT=num;
//			UDP_LISTEN_PORT=num;
//			printf("%d\r\n",TCP_LISTEN_PORT);
			GetInputString(msg);
			external_port = ATOI(msg, 10);

			// Try to Delete Port Action
			if((ret = DeletePortProcess(SOCK_UPNP, protocol, external_port))==0) printf("DeletePort Success!!\r\n");
			else printf("DeletePort Error Code is %d\r\n", ret);

		}


		/* OTHERS CHOICE*/
		if (bTreat == (bool)RESET)
		{
			SerialPutString(" wrong choice  \r\n");
		}			

		eventing_listener(SOCK_UPNP_EVENTING);

	} /* While(1)*/
}/* Main_Menu */

/*******************(C)COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/


