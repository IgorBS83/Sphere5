/****************************************************************************/
/*  C6455.cmd                                                               */
/*  Copyright (c) 2010 Texas Instruments Incorporated                       */
/*  Author: Rafael de Souza                                                 */
/*                                                                           */
/*    Description: This file is a sample linker command file that can be    */
/*                 used for linking programs built with the C compiler and  */
/*                 running the resulting .out file on an C6455              */
/*                 device.  Use it as a guideline.  You will want to        */
/*                 change the memory layout to match your specific C6xxx    */
/*                 target system.  You may want to change the allocation    */
/*                 scheme according to the size of your program.            */
/*                                                                          */
/****************************************************************************/

MEMORY
{
    L2RAM:      o = 0x00800000  l = 0x00200000  /* 2MB L2 Internal SRAM */
    L1PRAM:     o = 0x00E00000  l = 0x00008000  /* 32kB L1 Program SRAM/CACHE */ 
    L1DRAM:     o = 0x00F00000  l = 0x00008000  /* 32kB L1 Data SRAM/CACHE */
    EMIFA_CE2:  o = 0xA0000000  l = 0x00800000  /* 8MB EMIFA CE2 */ 
    EMIFA_CE3:  o = 0xB0000000  l = 0x00800000  /* 8MB EMIFA CE2 */
    EMIFA_CE4:  o = 0xC0000000  l = 0x00800000  /* 8MB EMIFA CE2 */ 
    EMIFA_CE5:  o = 0xD0000000  l = 0x00800000  /* 8MB EMIFA CE2 */
    DDR2_CE0:   o = 0xE0000000  l = 0x20000000  /* 512MB EMIFB CE0 */
} 

SECTIONS
{
    .text          >  L2RAM/////
    .switch        >  L2RAM/////
    .cinit         >  L2RAM/////
    .csl_vect      >  L2RAM

    .stack         >  L2RAM
    .bss           >  L2RAM
    .cio           >  L2RAM
    .const         >  L2RAM
    .data          >  L2RAM
    .sysmem        >  L2RAM
    .far           >  L2RAM
    .args          >  L2RAM
    .ppinfo        >  L2RAM
    .ppdata        >  L2RAM
  
    /* COFF sections */
    .pinit         >  L2RAM
  
    /* EABI sections */
    .binit         >  L2RAM
    .init_array    >  L2RAM
    .neardata      >  L2RAM
    .fardata       >  L2RAM
    .rodata        >  L2RAM
    .c6xabi.exidx  >  L2RAM
    .c6xabi.extab  >  L2RAM

	.usect ".boot_load" > EMIFA_CE3

//	cL2RAM: 	{} L2RAM
//	cEMIFA_CE5: {} EMIFA_CE5

}


