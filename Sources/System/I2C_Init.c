#include <csl.h>
#include "main.h"

#define ICMDR_IRS (1 << 5)
#define ICMDR_TRX (1 << 9)
#define ICMDR_MST (1 << 10)
#define ICMDR_STP (1 << 11)
#define ICMDR_STT (1 << 13)
#define ICMDR_NACKMOD (1 << 15)

#define ICSTR_NACK (1 << 1)
#define ICSTR_ARDY (1 << 2)
#define ICSTR_ICRRDY (1 << 3)
#define ICSTR_ICXRDY (1 << 4)
#define ICSTR_SCD	 (1 << 5)
#define ICSTR_BB 	(1 << 12)

typedef struct
{
	Uint32 ICOAR;//Own Address Register Section
	Uint32 ICIMR;//Interrupt Mask/Status Register Section
	Uint32 ICSTR;//Interrupt Status Register Section
	Uint32 ICCLKL;//Clock Low-Time Divider Register Section
	Uint32 ICCLKH;//Clock High-Time divider Register Section
	Uint32 ICCNT;//Data Count Register Section
	Uint32 ICDRR;//Data Receive Register Section
	Uint32 ICSAR;//Slave Address Register Section
	Uint32 ICDXR;//Data Transmit Register Section
	Uint32 ICMDR;//Mode Register Section
	Uint32 ICIVR;//Interrupt Vector Register Section
	Uint32 ICEMDR;//Extended Mode Register Section
	Uint32 ICPSC;//Prescaler Register Section
}
I2C_registers_type;

I2C_registers_type* I2C_registers = (I2C_registers_type*)0x02B04000;

void I2C_Init()
{
//	I2C_registers->ICPSC = 100 - 1;//10MHz
//	I2C_registers->ICCLKH = 15;//5-100MHz; 10-66.6;12-58.8;15-50.0
	I2C_registers->ICMDR &= ~ICMDR_IRS;
	I2C_registers->ICPSC = 20 - 1;//10MHz
//	I2C_registers->ICCLKH = 100 - 6;//5-100MHz; 10-66.6;12-58.8;15-50.0
	I2C_registers->ICCLKH = 50 - 6;//5-100MHz; 10-66.6;12-58.8;15-50.0
	I2C_registers->ICCLKL = I2C_registers->ICCLKH;
	I2C_registers->ICMDR |= ICMDR_IRS;

	I2C_registers->ICIMR = 0x7f;
	I2C_registers->ICOAR = 0x01;

}


//volatile Uint8 dev_addr;

#define I2C_ANSWER_TIME 0x80000
void I2C_read_byte(I2C_process_type* i2c)
{
	volatile int delay_cnt = 0;
	i2c->rezult = 0;
	switch(1){
	default:
		I2C_registers->ICSAR = i2c->device_addr;
		I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
		I2C_registers->ICMDR |= ICMDR_MST;
		I2C_registers->ICMDR |= ICMDR_STP;//stop
		I2C_registers->ICCNT = 1;
		I2C_registers->ICSTR |= ICSTR_ICXRDY | ICSTR_NACK;
		I2C_registers->ICMDR |= ICMDR_STT;//start
		I2C_registers->ICDXR = i2c->reg_addr;
		while(((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0) && (delay_cnt < I2C_ANSWER_TIME)) delay_cnt++;
		if((I2C_registers->ICSTR & ICSTR_NACK) || (delay_cnt >= I2C_ANSWER_TIME)) break;

		I2C_registers->ICMDR &= ~ICMDR_TRX;//receiver
		I2C_registers->ICMDR |= ICMDR_MST;
		I2C_registers->ICSTR |= ICSTR_ICRRDY;
		I2C_registers->ICMDR |= ICMDR_STT;//start
		while((I2C_registers->ICSTR & (ICSTR_ICRRDY /*| ICSTR_NACK*/)) == 0);
		I2C_registers->ICSTR |= ICMDR_NACKMOD;
		I2C_registers->ICMDR |= ICMDR_STP;//stop

		i2c->reg_data = I2C_registers->ICDRR;
		i2c->rezult = 1;
	}
}

void I2C_write_byte(I2C_process_type* i2c)
{
	volatile int delay_cnt = 0;
	i2c->rezult = 0;
	switch(1){
	default:
		I2C_registers->ICSAR = i2c->device_addr;
		I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
		I2C_registers->ICMDR |= ICMDR_MST;
		I2C_registers->ICMDR |= ICMDR_STP;//stop
		I2C_registers->ICCNT = 2;
		I2C_registers->ICSTR |= ICSTR_ICXRDY | ICSTR_NACK;
		I2C_registers->ICMDR |= ICMDR_STT;//start
		I2C_registers->ICDXR = i2c->reg_addr;
		while(((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0) && (delay_cnt < I2C_ANSWER_TIME)) delay_cnt++;
		if((I2C_registers->ICSTR & ICSTR_NACK) || (delay_cnt >= I2C_ANSWER_TIME)) break;

		I2C_registers->ICSTR |= ICSTR_ICXRDY;
		I2C_registers->ICDXR = i2c->reg_data;
		while((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0);
		if(I2C_registers->ICSTR & ICSTR_NACK) break;
		i2c->rezult = 1;
	}
}

//void I2C_process(I2C_process_type* i2c)
//{
////	volatile Uint8 data;
////	if(I2C_registers->ICSTR & ICSTR_BB == 0){
////		I2C_registers->ICSAR = dev_addr++ & 0x7F;
////		I2C_read_byte(4, (Uint8*)&dev_addr);
////	}
////	else I2C_registers->ICSTR |= ICSTR_BB;
////		I2C_write_byte(4, dev_addr + 1);
////		data = I2C_read_byte(4);
////		I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
////		I2C_registers->ICMDR |= ICMDR_MST;
////		I2C_registers->ICMDR |= ICMDR_STT;//start
////		I2C_registers->ICDXR = 0;
////		while((I2C_registers->ICSTR & ICSTR_ICXRDY) == 0);
////		I2C_registers->ICSTR |= ICSTR_ICXRDY;
////		I2C_registers->ICMDR |= ICMDR_STP;//stop
////
////		I2C_registers->ICMDR &= ~ICMDR_TRX;//receiver
////		I2C_registers->ICMDR |= ICMDR_MST;
////		I2C_registers->ICMDR |= ICMDR_STT;//start
////		while((I2C_registers->ICSTR & ICSTR_ICRRDY) == 0);
////		I2C_registers->ICSTR |= ICSTR_ICRRDY;
////		I2C_registers->ICSTR |= ICMDR_NACKMOD;
////		I2C_registers->ICMDR |= ICMDR_STP;//stop
////		data = I2C_registers->ICDRR;
//
//}

//Uint8 I2C_read_byte(Uint8 addr, Uint8* data)
//{
//	volatile Uint8 rezult = 0;
//	switch(1){
//	default:
//		I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
//		I2C_registers->ICMDR |= ICMDR_MST;
//		I2C_registers->ICMDR |= ICMDR_STP;//stop
//		I2C_registers->ICCNT = 1;
//		I2C_registers->ICSTR |= ICSTR_ICXRDY | ICSTR_NACK;
//		I2C_registers->ICMDR |= ICMDR_STT;//start
//		I2C_registers->ICDXR = addr;
//		while((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0);
//		if(I2C_registers->ICSTR & ICSTR_NACK) break;
//
//		I2C_registers->ICMDR &= ~ICMDR_TRX;//receiver
//		I2C_registers->ICMDR |= ICMDR_MST;
//		I2C_registers->ICSTR |= ICSTR_ICRRDY;
//		I2C_registers->ICMDR |= ICMDR_STT;//start
//		while((I2C_registers->ICSTR & (ICSTR_ICRRDY /*| ICSTR_NACK*/)) == 0);
//		I2C_registers->ICSTR |= ICMDR_NACKMOD;
//		I2C_registers->ICMDR |= ICMDR_STP;//stop
//
//		*data = I2C_registers->ICDRR;
//		rezult = 1;
//	}
//	return rezult;
//}

//Uint8 I2C_write_byte(Uint8 addr, Uint8 data)
//{
//	volatile Uint8 rezult = 0;
//	switch(1){
//	default:
//		I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
//		I2C_registers->ICMDR |= ICMDR_MST;
//		I2C_registers->ICMDR |= ICMDR_STP;//stop
//		I2C_registers->ICCNT = 2;
//		I2C_registers->ICSTR |= ICSTR_ICXRDY | ICSTR_NACK;
//		I2C_registers->ICMDR |= ICMDR_STT;//start
//		I2C_registers->ICDXR = addr;
//		while((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0);
//		if(I2C_registers->ICSTR & ICSTR_NACK) break;
//		I2C_registers->ICSTR |= ICSTR_ICXRDY;
//		I2C_registers->ICDXR = data;
//		while((I2C_registers->ICSTR & (ICSTR_ICXRDY | ICSTR_NACK)) == 0);
//		if(I2C_registers->ICSTR & ICSTR_NACK) break;
//		rezult = 1;
//	}
//	return rezult;
////	I2C_registers->ICMDR |= ICMDR_TRX;//transmitter
////	I2C_registers->ICMDR |= ICMDR_MST;
////	I2C_registers->ICCNT = 2;
////	I2C_registers->ICMDR |= ICMDR_STT;//start
////	I2C_registers->ICDXR = addr;
////	while((I2C_registers->ICSTR & ICSTR_ICXRDY) == 0);
////	I2C_registers->ICSTR |= ICSTR_ICXRDY;
////	I2C_registers->ICDXR = data;
////	while((I2C_registers->ICSTR & ICSTR_ICXRDY) == 0);
////	I2C_registers->ICSTR |= ICSTR_ICXRDY;
////	I2C_registers->ICMDR |= ICMDR_STP;//stop
//}

