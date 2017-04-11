#include <csl_tmr.h>
//#include "Driver.h"

typedef struct{
	Uint32 free : 1;
	Uint32 soft : 1;
	Uint32 res0 : 14;
	Uint32 clkdiv: 4;
	Uint32 res1 : 12;
}
EMUMGT_CLKSPD_type;//Emulation Management and Clock Speed Register

typedef struct{
	Uint32 TSTAT_LO		:1;
	Uint32 INVOUTP_LO	:1;
	Uint32 INVINP_LO	:1;
	Uint32 CP_LO		:1;
	Uint32 PWID_LO		:2;
	Uint32 ENAMODE_LO	:2;
	Uint32 CLKSRC_LO	:1;
	Uint32 TIEN_LO		:1;
	Uint32 res0			:6;
	Uint32 TSTAT_HI		:1;
	Uint32 INVOUTP_HI	:1;
	Uint32 res1			:1;
	Uint32 CP_HI		:1;
	Uint32 PWID_HI		:2;
	Uint32 ENAMODE_HI	:2;
	Uint32 res2			:8;
}
TCR_type;

typedef struct{
	Uint32 TIMLORS	:1;
	Uint32 TIMHIRS	:1;
	Uint32 TIMMODE	:2;
	Uint32 res0		:4;
	Uint32 PSCHI	:4;
	Uint32 TDDRHI	:4;
	Uint32 res1		:16;
}
TGCR_type;

typedef struct{
	Uint32 res0	:12;
	Uint32 wdikey	:2;
	Uint32 wden	:1;
	Uint32 wdflag		:1;
	Uint32 wdkey	:16;
}
WDTCR_type;

typedef struct{
	Uint32 res0;
	EMUMGT_CLKSPD_type EMUMGT_CLKSPD;
	Uint32 res1;
	Uint32 res2;
	Uint32 CNTLO;
	Uint32 CNTHI;
	Uint32 PRDLO;
	Uint32 PRDHI;
	TCR_type TCR;
	TGCR_type TGCR;
	WDTCR_type WDTCR;
}
Timer_Registers_type;//Device_State_Control_Registers




Timer_Registers_type *Timer0_Registers = (Timer_Registers_type*)0x02940000;
Timer_Registers_type *Timer1_Registers = (Timer_Registers_type*)0x02980000;

enum{
	TCR_ENAMODE_disabled,
	TCR_ENAMODE_enabled_one_time,
	TCR_ENAMODE_enabled_continuously
}
TCR_ENAMODE;

enum{
	TGCR_TIMMODE_64bit_general,
	TGCR_TIMMODE_dual_32bit_unchained,
	TGCR_TIMMODE_64bit_watchdog,
	TGCR_TIMMODE_dual_32bit_chained
}
TGCR_TIMMODE;

CSL_TmrHandle hTmr;
CSL_TmrObj tmrObj;
CSL_TmrHwSetup TmrhwSetup = CSL_TMR_HWSETUP_DEFAULTS;
CSL_TmrEnamode TimeCountMode = CSL_TMR_ENAMODE_CONT;

void TimerReset()
{
	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_RESET_TIMLO, NULL);
	CSL_tmrHwSetup(hTmr,&TmrhwSetup);
	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_START_TIMLO, (void *)&TimeCountMode); //Start the timer in GPtimer64 OR Chained mode
}

int Timer_Init(){
	unsigned long long TIMER0_PERIOD = 1200000000 / Timer0_Registers->EMUMGT_CLKSPD.clkdiv * 2 / 3;

	Timer0_Registers->TCR.ENAMODE_LO = TCR_ENAMODE_disabled;
	Timer0_Registers->CNTHI = 0;
	Timer0_Registers->CNTLO = 0;
	Timer0_Registers->PRDHI = TIMER0_PERIOD >> 32;
	Timer0_Registers->PRDLO = TIMER0_PERIOD;
	Timer0_Registers->TGCR.TIMMODE = TGCR_TIMMODE_64bit_general;
	Timer0_Registers->TGCR.TIMHIRS = 1;
	Timer0_Registers->TGCR.TIMLORS = 1;
	Timer0_Registers->TCR.ENAMODE_LO = TCR_ENAMODE_enabled_continuously;

	Timer1_Registers->TCR.ENAMODE_LO = TCR_ENAMODE_disabled;
	Timer1_Registers->CNTHI = 0;
	Timer1_Registers->CNTLO = 0;
	Timer1_Registers->PRDHI = TIMER0_PERIOD >> 32;
	Timer1_Registers->PRDLO = TIMER0_PERIOD * 5;
	Timer1_Registers->TGCR.TIMMODE = TGCR_TIMMODE_64bit_watchdog;
	Timer1_Registers->WDTCR.wden = 1;
	Timer1_Registers->WDTCR.wdkey = 0xA5C6;
	Timer1_Registers->WDTCR.wdkey = 0xDA7E;


	return 0;
}

/*
int Timer_Init(){

	CSL_Status status;


	Bool  timer0En;

    CSL_FINST(((CSL_DevRegs*)CSL_DEV_REGS)->PERLOCK, DEV_PERLOCK_LOCKVAL, UNLOCK);
    // Enable the Powersaver clock for TIMER
    CSL_FINST(((CSL_DevRegs*)CSL_DEV_REGS)->PERCFG0, DEV_PERCFG0_TIMER0CTL, ENABLE);

    do {
    	timer0En = (Bool) CSL_FEXT(((CSL_DevRegs*)CSL_DEV_REGS)->PERSTAT0, DEV_PERSTAT0_TIMER0STAT);
    } while ((timer0En) != TRUE);

    memset(&tmrObj, 0, sizeof(CSL_TmrObj));
// настройка таймера
	CSL_tmrInit(NULL);
	hTmr = CSL_tmrOpen(&tmrObj, 0, NULL, &status);
	TmrhwSetup.tmrClksrcLo = CSL_TMR_CLKSRC_INTERNAL;
	//1.2 GHz 1 секунда
//	TmrhwSetup.tmrTimerPeriodHi = 0x08800000;//4786;
	TmrhwSetup.tmrTimerCounterLo = 0;
	TmrhwSetup.tmrTimerPeriodLo = 0x20000000;//8C00;
	TmrhwSetup.tmrTimerMode = CSL_TMR_TIMMODE_DUAL_UNCHAINED; //The timer is in 64-bit GP timer mode
	CSL_tmrHwSetup(hTmr,&TmrhwSetup);
	//запуск таймера
//	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_RESET_TIMHI, NULL);
	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_RESET_TIMLO, NULL);
	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_START_TIMLO, (void *)&TimeCountMode); //Start the timer in GPtimer64 OR Chained mode
//	CSL_tmrHwControl(hTmr, CSL_TMR_CMD_START_TIMHI, (void *)&TimeCountMode); //Start the timer in GPtimer64 OR Chained mode
//	CSL_tmrClose (hTmr);
	return 0;
}
*/

