#include "24c16.h"
#include "util.h"
#include "config.h"

/* eerom init */
void at24c16_init(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB |RCC_APB2Periph_AFIO , ENABLE);	
  //Port B bidirection SDA
  GPIO_InitStructure.GPIO_Pin = I2C_SDA;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
    
  //Port B output
  GPIO_InitStructure.GPIO_Pin = I2C_SCK;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/* eerom start! */
void at24c16_start(void)
{
  GPIO_SetBits(GPIOB, I2C_SDA);
  GPIO_SetBits(GPIOB, I2C_SCK);
  Delay_us(1);
  GPIO_ResetBits(GPIOB, I2C_SDA);
  GPIO_ResetBits(GPIOB, I2C_SCK);
	Delay_us(1);
}
/* eerom stop!*/
void at24c16_stop(void)
{
  GPIO_ResetBits(GPIOB, I2C_SDA);
  GPIO_SetBits(GPIOB, I2C_SCK);
  Delay_us(1);
  GPIO_SetBits(GPIOB, I2C_SDA);
}

/* write_1byte to eerom */
void write_1byte(unsigned char val)
{
  unsigned char i, tmp;
  tmp=val;
  
  for (i=0;i<8;i++)
  {
    GPIO_ResetBits(GPIOB, I2C_SCK);
    Delay_us(1);
    if (((tmp<<i) & 0x80)==0x80)
      GPIO_SetBits(GPIOB, I2C_SDA);
    else
      GPIO_ResetBits(GPIOB, I2C_SDA);
    Delay_us(1);
    GPIO_SetBits(GPIOB, I2C_SCK);
    Delay_us(1);
  }
  GPIO_ResetBits(GPIOB, I2C_SCK);
  Delay_us(1);
  GPIO_SetBits(GPIOB, I2C_SDA);
  Delay_us(1);
}

/* read_1byte from eerom */
unsigned char read_1byte(void)
{
  unsigned char i,j,k=0;
  
  GPIO_ResetBits(GPIOB, I2C_SCK);Delay_us(1);GPIO_SetBits(GPIOB, I2C_SDA);
  for (i=0;i<8;i++)
  {
    Delay_us(1);
    GPIO_SetBits(GPIOB, I2C_SCK);
    Delay_us(1);
    
    if(GPIO_ReadInputDataBit(GPIOB, I2C_SDA)==1) j=1; //???SDA becoms inout pin from output pin;
    else j=0;
    k=(k<<1) | j; 
    GPIO_ResetBits(GPIOB, I2C_SCK);
  }
  Delay_us(1);
  return (k);
}


void clock(void)
{
  uint16 i=0;
  
  GPIO_SetBits(GPIOB, I2C_SCK);
  Delay_us(1);
  
  while((GPIO_ReadInputDataBit(GPIOB, I2C_SDA)==1) && (i<255))
    i++;
  GPIO_ResetBits(GPIOB, I2C_SCK);
  Delay_us(1);
}

void at24c16_write(uint16 addr, uint8 val)
{
   uint8 l_addr;

  __disable_irq();
  at24c16_start();
  //write_1byte(0xa0);
  //clock();
  //uint8 h_addr=addr>>8;
  //write_1byte(h_addr);
  write_1byte(0xa0 | ((addr>>7 & 0xfe)));
  clock();
  l_addr=(addr%256);
  write_1byte(l_addr);
  clock();
  write_1byte(val);
  clock();
  at24c16_stop();
  __enable_irq();
  Delay_ms(5);
}

unsigned char at24c16_read(uint16 addr)
{
  //uint8 high,low;
  uint8 i;
  uint8 low;
  low = addr & 0x00ff;
  //high=(addr & 0xff00)>>8;

  __disable_irq();
  at24c16_start();
  //write_1byte(0xa0);
  //clock();
  //write_1byte(high);
  write_1byte(0xa0 | ((addr>>7 & 0xfe)));
  clock();
  write_1byte(low);
  clock();
  at24c16_start();
  write_1byte(0xa1);
  clock();
  i=read_1byte();
  at24c16_stop();
  //Delay_us(5);
  __enable_irq();
  return(i);
}
//eep block write
//eepAddr: eeprom start address
//dat: data array to be saved to eeprom
//index: data array start index
//len: how long to be write
void eep_block_write(uint16 eepAddr, uint8* dat, uint16 index, uint16 len)
{
 	  uint16 i;
  for(i=0; i<len; i++)
  {
    at24c16_write(eepAddr+i, dat[index+i]);
  }
}

void erase_eeprom(uint16 startAddr, uint16 len)
{	  
      uint16 i;
    for(i=startAddr;i<startAddr+len;i++)
  {
    at24c16_write(i,0xff);
  }
}

