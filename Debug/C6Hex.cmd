Sphere5.out
--binary
--map=Sphere5.map
--order=L

--boot
--bootorg=0xB0001000
--bootsection=.boot_load 0xB0000000

--image
--zero
--fill=0xFFFFFFFF

ROMS
{
	FLASH: org = 0xB0000000, len = 0x14000, romwidth = 8, memwidth = 8 files={Sphere5.bin}
}


/*
--outfile=MT.bin
--memwidth=8
--romwidth=8

*/
