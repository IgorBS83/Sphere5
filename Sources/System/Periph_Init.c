#include <csl.h>
#define PERLOCK_LOCKVAL 0x0F0A0B00

typedef enum{
	PERSTAT_disabled = 0,
	PERSTAT_enabled = 1,
	PERSTAT_static_powerdown = 3,
	PERSTAT_disable_in_progress_state = 4,
	PERSTAT_enable_in_progress_state = 5,
	PERSTAT_field = 7
}
PERSTAT_STATE;

typedef enum{
	PERCFG_SRIO_disabled,
	PERCFG_SRIO_res0,
	PERCFG_SRIO_res1,
	PERCFG_SRIO_enabled
}
PERCFG_SRIO_STATE;

typedef enum{
	disabled,
	enabled
}
STATE;

typedef struct{
	Uint32 TCPCTL	:1;
	Uint32 res1	:1;
	Uint32 VCPCTL	:1;
	Uint32 res2	:1;
	Uint32 EMACCTL	:1;
	Uint32 res3	:1;
	Uint32 TIMER0CTL :1;
	Uint32 res4	:1;
	Uint32 TIMER1CTL :1;
	Uint32 res5	:1;
	Uint32 GPIOCTL	:1;
	Uint32 res6	:1;
	Uint32 I2CCTL	:1;
	Uint32 res7	:1;
	Uint32 McBSP0CTL :1;
	Uint32 res8	:1;
	Uint32 McBSP1CTL :1;
	Uint32 res9	:1;
	Uint32 HPICTL	:1;
	Uint32 res10	:1;
	Uint32 PCICTL	:1;
	Uint32 res11	:1;
	Uint32 UTOPIACTL :1;
	Uint32 res12	:7;
	Uint32 SRIOCTL	:2;
}
PERCFG0_type;

#define PERCFG0_TIMER0CTL 6
#define PERCFG0_TIMER1CTL 8
#define PERCFG0_GPIOCTL   10
#define PERCFG0_I2CCTL 	  12

#define PERSTAT0_TIMER0STAT 9
#define PERSTAT0_TIMER1STAT 12
#define PERSTAT0_GPIOSTAT   15
#define PERSTAT0_I2CSTAT    18

#define Device_State_Control_Registers 0x02AC0000
Uint32 *const reg_PERLOCK = (Uint32*)(Device_State_Control_Registers + 0x04);
Uint32 *const reg_PERCFG0 = (Uint32*)(Device_State_Control_Registers + 0x08);
Uint32 *const reg_PERSTAT0 = (Uint32*)(Device_State_Control_Registers + 0x14);

typedef struct{
	Uint32 EMIFACTL	:1;
	Uint32 DDR2CTL	:1;
	Uint32 res2	:30;
}
PERCFG1_type;

typedef struct{
	Uint32 TCPSTAT	:3;
	Uint32 VCPSTAT	:3;
	Uint32 EMACSTAT	:3;
	Uint32 TIMER0STAT:3;
	Uint32 TIMER1STAT:3;
	Uint32 GPIOSTAT	:3;
	Uint32 I2CSTAT	:3;
	Uint32 McBSP0STAT:3;
	Uint32 McBSP1STAT:3;
	Uint32 HPISTAT	:3;
	Uint32 res		:2;
}
PERSTAT0_type;

typedef struct{
	Uint32 PCISTAT	:3;
	Uint32 UTOPIASTAT	:3;
	Uint32 res		:26;
}
PERSTAT1_type;

typedef struct{
	Uint32 rez1;
	Uint32 PERLOCK;
	PERCFG0_type PERCFG0;
	Uint32 rez2;
	Uint32 rez3;
	PERSTAT0_type PERSTAT0;
	PERSTAT1_type PERSTAT1;
	Uint32 rez4;
	Uint32 EMACCFG;
	Uint32 rez5;
	Uint32 rez6;
	PERCFG1_type PERCFG1;
	Uint32 rez7[9];
	Uint32 EMUBUFPD;
	Uint32 rez8;
}
DSCR_type;//Device_State_Control_Registers

DSCR_type* DSCR = (DSCR_type*)0x02AC0000;

void PeriphInit()
{
	if((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_GPIOSTAT)) != (PERSTAT_enabled << PERSTAT0_GPIOSTAT)){
		*reg_PERLOCK = PERLOCK_LOCKVAL;
		*reg_PERCFG0 |= (PERSTAT_enabled << PERCFG0_GPIOCTL);
		while((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_GPIOSTAT)) != (PERSTAT_enabled << PERSTAT0_GPIOSTAT));
	}
	if((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_TIMER0STAT)) != (PERSTAT_enabled << PERSTAT0_TIMER0STAT)){
		*reg_PERLOCK = PERLOCK_LOCKVAL;
		*reg_PERCFG0 |= (PERSTAT_enabled << PERCFG0_TIMER0CTL);
		while((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_TIMER0STAT)) != (PERSTAT_enabled << PERSTAT0_TIMER0STAT));
	}
	if((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_TIMER1STAT)) != (PERSTAT_enabled << PERSTAT0_TIMER1STAT)){
		*reg_PERLOCK = PERLOCK_LOCKVAL;
		*reg_PERCFG0 |= (PERSTAT_enabled << PERCFG0_TIMER1CTL);
		while((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_TIMER1STAT)) != (PERSTAT_enabled << PERSTAT0_TIMER1STAT));
	}
	if((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_I2CSTAT)) != (PERSTAT_enabled << PERSTAT0_I2CSTAT)){
		*reg_PERLOCK = PERLOCK_LOCKVAL;
		*reg_PERCFG0 |= (PERSTAT_enabled << PERCFG0_I2CCTL);
		while((*reg_PERSTAT0 & (PERSTAT_field << PERSTAT0_I2CSTAT)) != (PERSTAT_enabled << PERSTAT0_I2CSTAT));
	}

}
