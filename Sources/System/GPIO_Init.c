typedef unsigned int Uint32;

#include "Driver.h"

typedef enum{
	HL5 = (1 << 6),
	HL6 = (1 << 7),
	HL7 = (1 << 12),
	HL8 = (1 << 13)
}
DIODES;

typedef struct{
	Uint32 BINTEN;
	Uint32 res0;
	Uint32 DIR;
	Uint32 OUT_DATA;
	Uint32 SET_DATA;
	Uint32 CLR_DATA;
	Uint32 IN_DATA;
	Uint32 SET_RIS_TRIG;
	Uint32 CLR_RIS_TRIG;
	Uint32 SET_FAL_TRIG;
	Uint32 CLR_FAL_TRIG;
}
GPIO_type;//Device_State_Control_Registers

GPIO_type *GPIO_struct = (GPIO_type*)0x02B00008;

int GpioBlink(int);
int GpioSet(int GpioNum, Uint32 OnOff);

int GpioInit() {

//	Uint32* PERLOCK = (Uint32*)0x02AC0004;
//	Uint32* PERCFG0 = (Uint32*)0x02AC0008;
//	Uint32* PERSTAT0 = (Uint32*)0x02AC0014;
//
//    if ((*PERSTAT0 & 0x8000) == 0){
//		*PERLOCK = (Uint32) 0x0F0A0B00;
//		*PERCFG0 |= (Uint32) 0x400;
//		while((*PERSTAT0 & 0x8000) == 0){}
//    }


	*(Uint32*)0x02B00008 = 0x00000001;//BINTEN
	*(Uint32*)0x02B00010 = 0xFFFFCF3F;//DIR
	*(Uint32*)0x02B00024 = 0x00000004;//SET_RIS_TRIG
	*(Uint32*)0x02B0002C = 0x00000002;//SET_FAL_TRIG

	GpioSet(6, 0);	GpioSet(7, 0);	GpioSet(12, 0);	GpioSet(13, 0);
	return 0;
}


int GpioSet(int GpioNum, Uint32 OnOff){
	switch(OnOff)
	{
	case 0:
		*(Uint32*)0x02B0001C = 1 << GpioNum;//CLR_DATA
		break;
	case 1:
		*(Uint32*)0x02B00018 = 1 << GpioNum;//SET_DATA
		break;
	}
	return 0;
}


int GpioBlink(int GpioNum){
	Uint32 a = *(Uint32*)0x02B00014;//OUT_DATA
	if(a & (1 << GpioNum)) GpioSet(GpioNum, 0);
	else GpioSet(GpioNum, 1);
	return 0;
}

/*
//#include <csl.h>
//#include <csl_gpio.h>
//#include <cslr_dev.h>


CSL_GpioHandle hGpio;
CSL_GpioObj gpioObj;

int GpioInit() {

	Uint32* PERLOCK = (Uint32*)0x02AC0004;
	Uint32* PERCFG0 = (Uint32*)0x02AC0008;
	Uint32* PERSTAT0 = (Uint32*)0x02AC0014;

    if ((*PERSTAT0 & 0x8000) == 0){
		*PERLOCK = (Uint32) 0x0F0A0B00;
		*PERCFG0 |= (Uint32) 0x400;
		while((*PERSTAT0 & 0x8000) == 0){}
    }

	CSL_Status status;

	CSL_GpioHwSetup GpiohwSetup;
	CSL_GpioPinConfig GpioPinConfig;
	CSL_GpioContext pContext;

	Bool  gpioEn;
	// Unlock the control register
	gpioEn = (Bool) CSL_FEXT(((CSL_DevRegs*)CSL_DEV_REGS)->PERSTAT0, DEV_PERSTAT0_GPIOSTAT);
	if (gpioEn != TRUE){
		CSL_FINST(((CSL_DevRegs*)CSL_DEV_REGS)->PERLOCK, DEV_PERLOCK_LOCKVAL, UNLOCK);
		CSL_FINST(((CSL_DevRegs*)CSL_DEV_REGS)->PERCFG0, DEV_PERCFG0_GPIOCTL, ENABLE);
	   do {
			gpioEn = (Bool) CSL_FEXT(((CSL_DevRegs*)CSL_DEV_REGS)->PERSTAT0, DEV_PERSTAT0_GPIOSTAT);
	   } while (gpioEn != TRUE);
	}

	CSL_gpioInit(&pContext);
	hGpio = CSL_gpioOpen(&gpioObj, CSL_GPIO, NULL, &status);
	GpiohwSetup.extendSetup = NULL;
	CSL_gpioHwSetup(hGpio, &GpiohwSetup);
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_BANK_INT_ENABLE, NULL);

	//прерывания c первой ножки по восходящему фронту приход кадра
	GpioPinConfig.pinNum = CSL_GPIO_PIN1;
	GpioPinConfig.trigger = CSL_GPIO_TRIG_FALLING_EDGE;
	GpioPinConfig.direction = CSL_GPIO_DIR_INPUT;

	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);

	//прерывание последовательный канал
	GpioPinConfig.pinNum = CSL_GPIO_PIN2;
	GpioPinConfig.trigger = CSL_GPIO_TRIG_RISING_EDGE;
	GpioPinConfig.direction = CSL_GPIO_DIR_INPUT;

	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);

	GpioPinConfig.direction = CSL_GPIO_DIR_OUTPUT;
	GpioPinConfig.trigger = CSL_GPIO_TRIG_CLEAR_EDGE;
	GpioPinConfig.pinNum = CSL_GPIO_PIN6;
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CLEAR_BIT, &GpioPinConfig);
	GpioPinConfig.pinNum = CSL_GPIO_PIN7;
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CLEAR_BIT, &GpioPinConfig);
	GpioPinConfig.pinNum = CSL_GPIO_PIN12;
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CLEAR_BIT, &GpioPinConfig);
	GpioPinConfig.pinNum = CSL_GPIO_PIN13;
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CONFIG_BIT, &GpioPinConfig);
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_CLEAR_BIT, &GpioPinConfig);

	return 0;
}

int GpioSet(int GpioNum, Uint32 OnOff){
	CSL_GpioPinNum PinNum;
	CSL_GpioHwControlCmd cmd;
	switch(GpioNum){
		case 6:
			PinNum = CSL_GPIO_PIN6;
			if (OnOff == 0) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		case 7:
			PinNum = CSL_GPIO_PIN7;
			if (OnOff == 0) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		case 12:
			PinNum = CSL_GPIO_PIN12;
			if (OnOff == 0) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		default:
			PinNum = CSL_GPIO_PIN13;
			if (OnOff == 0) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
	}
	CSL_gpioHwControl(hGpio, cmd, &PinNum);
	return 0;
}

int GpioBlink(int GpioNum){
	CSL_GpioPinNum PinNum;
	CSL_GpioHwControlCmd cmd;
	CSL_BitMask16* a;
	CSL_gpioHwControl(hGpio, CSL_GPIO_CMD_GET_OUTDRVSTATE, &a);
	switch(GpioNum){
		case 6:
			PinNum = CSL_GPIO_PIN6;
			if (((Uint32)a << (31 - 6)) >> 31) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		case 7:
			PinNum = CSL_GPIO_PIN7;
			if (((Uint32)a << (31 - 7)) >> 31) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		case 12:
			PinNum = CSL_GPIO_PIN12;
			if (((Uint32)a << (31 - 12)) >> 31) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
		default:
			PinNum = CSL_GPIO_PIN13;
			if (((Uint32)a << (31 - 13)) >> 31) cmd = CSL_GPIO_CMD_CLEAR_BIT;
			else cmd = CSL_GPIO_CMD_SET_BIT;
			break;
	}
	CSL_gpioHwControl(hGpio, cmd, &PinNum);
	return 0;
}
*/
