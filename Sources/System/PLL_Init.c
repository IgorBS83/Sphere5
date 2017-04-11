#include <csl.h>

void PllInit(){

		volatile int i;
	   	Uint32* PLLCTL  = 	(Uint32*) 0x029A0100;
	    Uint32* PLLM  = 	(Uint32*) 0x029A0110;
	    Uint32* PREDIV  = 	(Uint32*) 0x029A0114;
	    Uint32* PLLCMD  = 	(Uint32*) 0x029A0138;
	    Uint32* PLLDIV4  = 	(Uint32*) 0x029A0160;

		//PLL to become	stable
		for(i = 0; i < 1400; i++){	asm(" nop 5");	}

		*PLLCTL  &= (Uint32) 0xDF; //In PLLCTL, write PLLENSRC = 0 (enable PLLEN bit).
		*PLLCTL  &= (Uint32) 0xFE; //In PLLCTL, write PLLEN = 0 (bypass mode).
//		asm(" nop 5");

//	    if((*PLLM != 0x0000001D) || (*PREDIV != 0x00008000))
	    {
			*PLLCTL |= (Uint32) 0x8; //In PLLCTL, write PLLRST = 1 (PLL is reset).

			*PLLM  	= (Uint32) 0x0000001D;
			*PREDIV  = (Uint32) 0x00008000;
			*PLLDIV4 = (Uint32) 0x00008002;
			*PLLCMD = (Uint32)0x1;
		//Wait for PLL to properly reset. See device-specific data manual for PLL reset time.
			for(i = 0; i < 16; i++){	asm(" nop 8");	}

			*PLLCTL  &= (Uint32) 0xF7;
		//Wait for PLL to lock. See device-specific data manual for PLL lock time.
			for(i = 0; i < 400; i++){	asm(" nop 5");	}

			*PLLCTL |= (Uint32) 0x1;
	    }

}
