#include <csl_emifa.h>
#include <cslr_dev.h>

CSL_EmifaObj emifaObj;
CSL_EmifaHandle hEmifa;
CSL_EmifaHwSetup EmifahwSetup;
CSL_EmifaMemType SRam, Flash, Altera;
CSL_EmifaAsyncWait    asyncWait = CSL_EMIFA_ASYNCWAIT_DEFAULTS;
/*		{(CSL_EmifaArdyPol)CSL_EMIFA_ARDYPOL_HIGH, //Asynchronous Ready Pin Polarity
		(Uint8)0x80, 	//Maximum Extended Wait cycles
		(Uint8)0x03};	//Turn Around cycles*/

CSL_EmifaAsync         SRamMem = //CSL_EMIFA_ASYNCCFG_DEFAULTS;//RAM ECLKOUT = 6.6 ns
		{(Uint8)0x00,	//Select Strobe mode enable. When set to 1, the CEn pin will have read and write strobe timing.
						//1-Select strobe mode enabled, 0-Select strobe mode disabled
		(Uint8)0x00,	//WE Strobe mode enable. When set to 1, the BE[7:0] output pins will act as active low byte write
						//enables when accessing CEn space. When cleared to 0, the BE[7:0] output pins will act as active
						//low byte enables when accessing CEn. 1-WE Strobe mode enabled, 0-WE Strobe mode disabled
		(Uint8)0x00,	//Asynchronous ready input enable. Set to 1 to enable the asynchronous ready (ARDY) input pin
						//during accesses to the CEn space. When enabled, the ARDY pin can be used to extend the strobe
						//period during asynchronous accesses.0-ARDY pin disabled, 1-ARDY pin enabled
		(Uint8)1,//0x0F,	//Write Setup Width
		(Uint8)2,//0x3F,	//Write Strobe Width
		(Uint8)1,//0x07,	//Write hold width. Number of ECLKOUT cycles for which EA[19:0], BA[1:0], D[63:0], BE[7:0], and
						//CEn are held after AWE has been deasserted, minus one cycle.
		(Uint8)0,//0x0F,	//Read setup width. Number of ECLKOUT cycles from EA[19:0], BA[1:0], BE[7:0],
						//and CEn being set	to AOE asserted, minus one cycle.
		(Uint8)1,//0x3F,	//Read strobe width. Number of ECLKOUT cycles for which AOE is held active,
						//minus one cycle.
		(Uint8)0,//0x07, 	//Read hold width. Number of ECLKOUT cycles for which EA[19:0], BA[1:0], BE[7:0],
						//and CEn are held after AOE has been deasserted, minus one cycle
		(Uint8)3};//0x00};	//Asynchronous Memory Size /0-8b, 1-16b, 2-32b, 3-64b

CSL_EmifaAsync        FalshMem = CSL_EMIFA_ASYNCCFG_DEFAULTS;

CSL_EmifaAsync        AlteraMem = {
		(Uint8)0x00,
		(Uint8)0x00,
		(Uint8)0x00,
		(Uint8)0x01,
		(Uint8)0x02,
		(Uint8)0x01,
		(Uint8)0x02,
		(Uint8)0x03,
		(Uint8)0x02,
		(Uint8)0x03};


int EmifaInit(){

	CSL_Status status;

	CSL_FINST(((CSL_DevRegs*)CSL_DEV_REGS)->PERCFG1, DEV_PERCFG1_EMIFACTL, ENABLE);

    SRam.ssel = 0;//EMIFA_MEMTYPE_ASYNC
    SRam.async = &SRamMem;
    SRam.sync = NULL;

    Flash.ssel = 0;//EMIFA_MEMTYPE_ASYNC
    Flash.async = &FalshMem;
    Flash.sync = NULL;

    Altera.ssel = 0;//EMIFA_MEMTYPE_ASYNC
    Altera.async = &AlteraMem;
    Altera.sync = NULL;

    EmifahwSetup.asyncWait = &asyncWait;
    EmifahwSetup.ceCfg[0] = &Altera; 	//CE2
    EmifahwSetup.ceCfg[1] = &Flash;		//CE3
    EmifahwSetup.ceCfg[2] = NULL;		//CE4
    EmifahwSetup.ceCfg[3] = &SRam;		//CE5

	CSL_emifaInit(NULL);
	hEmifa = CSL_emifaOpen( &emifaObj, CSL_EMIFA, NULL, &status);
	CSL_emifaHwSetup(hEmifa, &EmifahwSetup);
	return 0;
}
