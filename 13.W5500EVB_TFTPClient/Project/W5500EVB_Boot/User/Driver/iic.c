/***********************************************

标题: iic.c
作者: 秋阳电子
网址：http://qiuyangdz.taobao.com
日期: 2013/03/15
版本：v1.0
功能: iic初始化以iic及数据读写
说明：iic1外设的初始化
*************************************************/
#include "stm32f10x.h"
#include "stm32f10x_i2c.h"
#include "iic.h"
#include <stdio.h>
/*************************************************

名称：iic_init(void)
功能：iic外设2初始化（管脚配置 总线速度 数据位宽）
输入参数：无
输出参数：无
返回值：  无
**************************************************/
void iic_init(void)
{
  I2C_InitTypeDef  I2C_InitStructure;
  GPIO_InitTypeDef  GPIO_InitStructure; 

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
    
  /* PB6,7 SCL and SDA */ 
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  I2C_DeInit(I2C2);
  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
  I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
  I2C_InitStructure.I2C_OwnAddress1 = 0x30;
  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
  I2C_InitStructure.I2C_ClockSpeed = 100000;  //300K

  I2C_Cmd(I2C2, ENABLE);
  I2C_Init(I2C2, &I2C_InitStructure);

  /*允许1字节1应答模式*/
  I2C_AcknowledgeConfig(I2C2, ENABLE);

}
/*************************************************

名称：void iic_rw(u8 *DataBuff,u8 ByteQuantity,u8 RegAddress,u8 SlaveAddress,u8 ControlByte)
功能：iic数据的读写
输入参数：
  u8 *data_buff     数据输入输出指针
  u8 byte_quantity  数据个数
  u8 reg_address    iic寄存器地址
  u8 slave_address  iic器件地址
  u8 control_byte   读写控制字 read = 1 write = 0
输出参数：u8 *data_buff iic读取的数据内容
返回值：无
**************************************************/
void iic_rw(u8 *DataBuff,u8 ByteQuantity,u8 RegAddress,u8 SlaveAddress,u8 ControlByte)
{
  u8 errorflag = 1,j,i = 1;
  while(i--)
  {
	/* 起始位 */
   I2C_GenerateSTART(I2C2, ENABLE);
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

   /* 发送器件地址(写)*/
   I2C_Send7bitAddress(I2C2, SlaveAddress, I2C_Direction_Transmitter);
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
   /*发送地址*/
   I2C_SendData(I2C2, RegAddress);
   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
   if(ControlByte == 0)
   {
     j=ByteQuantity;
     errorflag = 0;                     
     while(j--)
     {
	   I2C_SendData(I2C2, *DataBuff++); 
	   while(!(I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)||I2C_SR1_AF));
     }
	 I2C_GenerateSTOP(I2C2, ENABLE);
   }
   else
   {
     I2C_GenerateSTART(I2C2, ENABLE);
     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));

     /* 发送器件地址(写)*/
     I2C_Send7bitAddress(I2C2, SlaveAddress, I2C_Direction_Receiver);
     while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); /* EV7 */ 
     while (ByteQuantity)
     {
	   if(ByteQuantity == 1)
	   {
         I2C_AcknowledgeConfig(I2C2, DISABLE);	//最后一位后要关闭应答的
    	 I2C_GenerateSTOP(I2C2, ENABLE);			//发送停止位
	   }
	   while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)); /* EV7 */
	   *DataBuff = I2C_ReceiveData(I2C2);
	   DataBuff++;

	   /* Decrement the read bytes counter */
	   ByteQuantity--;
    }
	I2C_AcknowledgeConfig(I2C2, ENABLE);  //再次允许应答模式
	}
  }
}
/***************************END OF FILE**********************************************************************/
