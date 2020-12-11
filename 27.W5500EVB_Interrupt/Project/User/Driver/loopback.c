#include "loopback.h"

#define tick_second 1
#define TX_RX_MAX_BUF_SIZE 2048

uint8 I_STATUS[MAX_SOCK_NUM];
uint8 ch_status[MAX_SOCK_NUM] = {0};/** 0:close, 1:ready, 2:connected */
uint8 TX_BUF[TX_RX_MAX_BUF_SIZE];
uint8 dest_ip[4] = {192,168,1,101};
uint16 dest_port = 5000;

void ISR(void)
{
	uint8 IR_val = 0, SIR_val = 0;
	uint8 tmp,s;
	
	IINCHIP_WRITE(IMR, 0x00);
	IINCHIP_WRITE(SIMR, 0x00);
    IINCHIP_ISR_DISABLE();

	IR_val = IINCHIP_READ(IR);
	SIR_val = IINCHIP_READ(SIR);


#ifdef DEBUG
      printf("\r\nIR_val : %02x", IR_val);
      printf("\r\nSIR_val : %02x\r\n", SIR_val);
      //printf("\r\nSn_MR(0): %02x", IINCHIP_READ(Sn_MR(0)));
      //printf("\r\nSn_SR(0): %02x\r\n", IINCHIP_READ(Sn_SR(0)));
#endif


	if (IR_val > 0) {
	   	if (IR_val & IR_CONFLICT)
	   	{
#ifdef DEBUG
			printf("IP conflict : %.2x\r\n", IR_val);
#endif
	   	}
	   	if (IR_val & IR_MAGIC)
	   	{
#ifdef DEBUG
	   		printf("Magic packet: %.2x\r\n",IR_val);
#endif
	   	}
		if (IR_val & IR_PPPoE)
	   	{
#ifdef DEBUG
	   		printf("PPPoE closed: %.2x\r\n",IR_val);
#endif
	   	}
		if (IR_val & IR_UNREACH)
	   	{
#ifdef DEBUG
	   		printf("Destination unreached: %.2x\r\n",IR_val);
#endif
	   	}
    /* interrupt clear */
    IINCHIP_WRITE(IR, IR_val);
	}
	for(s = 0;s < 8;s ++)
	{
		tmp = 0;
		if (SIR_val & IR_SOCK(s))
		{
			/* save interrupt value*/
			tmp = IINCHIP_READ(Sn_IR(s)); // can be come to over two times.
			I_STATUS[s] |= tmp;			
			tmp &= 0x0F; 
			//I_STATUS_FLAG[0]++;
			IINCHIP_WRITE(Sn_IR(s), tmp);	
#ifdef DEBUG
			printf("Sn_IR(s): %.2x\r\n",tmp);
#endif
			/*---*/
		}
	}


    IINCHIP_ISR_ENABLE();
	IINCHIP_WRITE(IMR, 0xF0);
	IINCHIP_WRITE(SIMR, 0xFF); 

//#ifdef DEBUG
//        printf("IR2: %02x, IMR2: %02x, Sn_IR(0): %02x, Sn_IMR(0): %02x\r\n", IINCHIP_READ(IR2), IINCHIP_READ(IMR2), IINCHIP_READ(Sn_IR(0)), IINCHIP_READ(Sn_IMR(0)));
//#endif
}

void SOCK_DISCON(SOCKET s)
{
	disconnect(s);
	ch_status[s] = 0;
	I_STATUS[s] &= ~(0x02);
}

void loopback_tcps(SOCKET s, uint16 port)
{	
	uint16 len = 0;//, cnt = 0;
	uint8 * data_buf = (uint8*) TX_BUF;
	uint8 tmp = 0;

		if(I_STATUS[s] == SOCK_CLOSED)
		{// cloes
			if(!ch_status[s]) 
			{
				ch_status[s] = 1;
				if(socket(s,Sn_MR_TCP,port,0x00) == 0)	  /* reinitialize the socket */
				{
					ch_status[s] = 0;
				}
				else	
				{
					listen(s);
				}
			}
		}  
		if(I_STATUS[s]&Sn_IR_CON)
		{// connected
			ch_status[s] = 2;
			I_STATUS[s] &= ~(0x01);
			//break;
		}
		if(I_STATUS[s]&Sn_IR_DISCON)
		{// discon			
			if ((getSn_RX_RSR(s)) > 0)		/* check Rx data */
			{				
				if (len > TX_RX_MAX_BUF_SIZE) 
				len = TX_RX_MAX_BUF_SIZE; /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
			    recv(s, data_buf, len);		/* read the received data */
						data_buf[len]=0x00;
				printf("%s\r\n",data_buf);
			}
			SOCK_DISCON(s);
			//break;
		}
		if(I_STATUS[s]&Sn_IR_RECV)
		{
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
		// recv
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					len = recv(s, data_buf, len); /* read the received data */
					data_buf[len]=0;  //防止多打印
					printf("%s\r\n",data_buf);
					send(s,data_buf,len);
					tmp = I_STATUS[s];
				}
			 }
			//break;
		}
		if(I_STATUS[s]&Sn_IR_SEND_OK)
		{
			// send ok
			I_STATUS[s] &= ~(0x10);
			//break;
		}

#if (__MCU_TYPE__ == __MCU_STM32F103__)
		/**/
		if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) == 0x00) EXTI_GenerateSWInterrupt(EXTI_Line6);

#endif



		
}


void loopback_tcpc(SOCKET s, uint16 port)
{
	uint16 len; 						
	uint8 * data_buf = (u8*) TX_BUF;
	uint8	tmp = 0;
	switch (I_STATUS[s])
	{
		case SOCK_CLOSED://无中断
			// cloes
			if(!ch_status[s])
			{				
				ch_status[s] = 1;
				socket(s, Sn_MR_TCP, port, 0x00);
				connect(s, dest_ip, dest_port);
			}			
			break;
		case Sn_IR_CON: 
			// connected
			ch_status[s] = 2;
			I_STATUS[s] &= ~(0x01);
			break;
		case Sn_IR_DISCON: 
			// discon
			if ((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
			{
				if (len > TX_RX_MAX_BUF_SIZE) len = TX_RX_MAX_BUF_SIZE; /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
				len = recv(s, data_buf, len);		/* read the received data */
			}
			SOCK_DISCON(s);
			break;
		case Sn_IR_RECV: 
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					len = recv(s, data_buf, len); /* read the received data */
					data_buf[len]=0;  //防止多打印
					printf("%s\r\n",data_buf);
					send(s,data_buf,len);
					tmp = I_STATUS[s];
				}
			 }
			break;
		case Sn_IR_SEND_OK: 
			// send ok
			I_STATUS[s] &= ~(0x10);
			break;
		default:
			I_STATUS[s] =0;
		  ch_status[s] = 0;
		  break;
	}
}

void loopback_udp(SOCKET s, uint16 port)
{
	uint16 len;
	uint8 * data_buf = (u8*) TX_BUF;
	uint32_t destip = 0;
	uint16 destport;
  uint8 tmp;
	
	switch (I_STATUS[s])
	{
		case SOCK_CLOSED:                                               /* if a socket is closed */
			//printf("\r\n%d : Loop-Back UDP Started. port :%d", s, port);
			if(socket(s,Sn_MR_UDP,port,0x00)== 0)    /* reinitialize the socket */
				printf("\a%d : Fail to create socket.",s);
			break;
		case Sn_IR_SEND_OK: 
			// send ok
			I_STATUS[s] &= ~(0x10);
			break;
		case Sn_IR_RECV: 			
			IINCHIP_WRITE(IMR, 0x00);//开中断屏蔽IP,UNREACH,PPPoE,Magic Packet
			IINCHIP_WRITE(SIMR, 0x00);//开socket中断屏蔽  
			IINCHIP_ISR_DISABLE();
			tmp = I_STATUS[s];
			I_STATUS[s] &= ~(0x04); // RECV
			IINCHIP_ISR_ENABLE();
 
			IINCHIP_WRITE(IMR, 0xF0);
			IINCHIP_WRITE(SIMR, 0xFF);
			if (tmp & Sn_IR_RECV)
			{
				if((len = getSn_RX_RSR(s)) > 0)		/* check Rx data */
				{
					if (len > TX_RX_MAX_BUF_SIZE) 
					len = TX_RX_MAX_BUF_SIZE;	/* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
					len = recvfrom(s, data_buf, len,(uint8*)&destip,&destport);/* read the received data */
					data_buf[len]=0;  //防止多打印
					printf("%s\r\n",data_buf);
					sendto(s, data_buf, len,(uint8*)&destip,destport) ;
					tmp = I_STATUS[s];
				}
			 }
			break;
	}
}

