#include <csl_intc.h>





CSL_IntcContext context;
CSL_IntcEventHandlerRecord EventRecord[10];
CSL_IntcEventHandlerRecord isr_gpio;
CSL_IntcGlobalEnableState state;
CSL_IntcObj intcObjTmrHi, intcObjTmrLo, intcObjEdma, intcObjGpio;
CSL_IntcHandle hIntcTmrHi, hIntcTmrLo, hIntcEdma, hIntcGpio;
CSL_IntcParam vectId;
CSL_Status intStat;

extern int GpioBlink(int);
extern interrupt void CommandRead();

extern volatile Uint32 IntHalfCadr;	//сигнал о приходе полукадра
extern volatile Uint32 flagCopyStarted;

#define EVTCLR0 	0x01800040
#define EVTMASK0 	0x01800080
#define MEVTFLAG0   0x018000A0
#define INTMUX1 	0x01800104

void Event_Mask(Uint32 event, Uint32 mask){
	*(Uint32*)(EVTMASK0 + (event >> 3)) ^= mask << (event & 0x1F);
}

void Intmux_Set(Uint32 event, CSL_IntcVectId intsel){

	Uint32 mask;
	switch((intsel - 4) & 0x3){
	case 0: mask = 0x000000FF; break;
	case 1: mask = 0x0000FF00; break;
	case 2: mask = 0x00FF0000; break;
	case 3: mask = 0xFF000000; break;
	}
	*(Uint32*)(INTMUX1 + ((intsel - 4) & 0xC)) &= ~mask;
	*(Uint32*)(INTMUX1 + ((intsel - 4) & 0xC)) |= (event << (((intsel - 4) & 0x3) << 3)) & mask;
}

interrupt void Reset()
{
//    void (*init)() = (void*) 0xB0000000;
//	(*init)();
//    asm(" nop 9");
//	Event_Clear(CSL_INTC_EVENTID_TINTLO0);
	GpioBlink(7);
}

interrupt void GPIOHalfCadr()
{
//	Event_Clear(CSL_INTC_EVENTID_GPINT1);
	IntHalfCadr = 1;
}

//interrupt void EdmaDone()
//{
////	*(Uint32*) 0x02A01070 = 1;//ICR
//	flagCopyStarted = 2;
//}


void IntInit(){

	context.eventhandlerRecord = EventRecord;
	context.numEvtEntries = 1;

	CSL_intcInit(&context);
	CSL_intcGlobalNmiEnable();
	intStat = CSL_intcGlobalDisable(&state);

	//прерывания от едма

//	vectId = CSL_INTC_VECTID_5;
//	hIntcEdma = CSL_intcOpen(&intcObjEdma, CSL_INTC_EVENTID_EDMA3CC_INT0, &vectId, &intStat);
//	CSL_intcHwControl(hIntcEdma, CSL_INTC_CMD_EVTENABLE, NULL);
//	CSL_intcHookIsr(CSL_INTC_VECTID_5, &EdmaDone);


	//прерывания от GPIO кадр

	vectId = CSL_INTC_VECTID_4;
	hIntcGpio = CSL_intcOpen(&intcObjGpio, CSL_INTC_EVENTID_GPINT1, &vectId, &intStat);
	CSL_intcHwControl(hIntcGpio, CSL_INTC_CMD_EVTENABLE, NULL);
	CSL_intcHookIsr(CSL_INTC_VECTID_4, &GPIOHalfCadr);



	//прерывания от GPIO протокол

//	vectId = CSL_INTC_VECTID_6;
//	hIntcGpio = CSL_intcOpen(&intcObjGpio, CSL_INTC_EVENTID_GPINT2, &vectId, &intStat);
//	CSL_intcHwControl(hIntcGpio, CSL_INTC_CMD_EVTENABLE, NULL);
//	CSL_intcHookIsr(CSL_INTC_VECTID_6, &CommandRead);

	//прерывания от таймера



//
//	*(Uint32*)(EVTMASK0 + 0x00) = 0xFFFFFFFF;
//	*(Uint32*)(EVTMASK0 + 0x04) = 0xFFFFFFFF;
//	*(Uint32*)(EVTMASK0 + 0x08) = 0xFFFFFFFF;
//	*(Uint32*)(EVTMASK0 + 0x0C) = 0xFFFFFFFF;
//
//	*(Uint32*)(EVTCLR0 + 0x00) = 0xFFFFFFFF;
//	*(Uint32*)(EVTCLR0 + 0x04) = 0xFFFFFFFF;
//	*(Uint32*)(EVTCLR0 + 0x08) = 0xFFFFFFFF;
//	*(Uint32*)(EVTCLR0 + 0x0C) = 0xFFFFFFFF;

//	Event_Mask(CSL_INTC_EVENTID_TINTLO0, 1);
//	Intmux_Set(CSL_INTC_EVENTID_TINTLO0, CSL_INTC_VECTID_7);

	vectId = CSL_INTC_VECTID_7;
	hIntcTmrHi = CSL_intcOpen(&intcObjTmrHi, CSL_INTC_EVENTID_TINTLO0, &vectId, &intStat);
	CSL_intcHwControl(hIntcTmrHi, CSL_INTC_CMD_EVTENABLE, NULL);
	CSL_intcHookIsr(CSL_INTC_VECTID_7, &Reset);



	intStat = CSL_intcGlobalEnable(&state);
}
