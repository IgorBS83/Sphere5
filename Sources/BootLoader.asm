COPY_TABLE .equ 0xB0001000

 .sect ".boot_load"
 .global _boot
_boot:
 mvkl COPY_TABLE, a3
 mvkh COPY_TABLE, a3

 ldw *a3++,b1 ; entry piont
 nop 5

copyblock:
 ldw *a3++,b0	;byte count
 nop 5
 [!b0] b copydone
 ldw *a3++,a4	;RAM start adress
 nop 5

copydata:
 ldb *a3++,b5
 nop 5
 stb b5,*a4++
 sub b0,1,b0
 [b0] b copydata
 nop 9
 b copyblock	;step to next block
 nop 9
copydone:
 b .S2 b1
 nop 9


; .global Reset
;Reset:
;.cproc
; mvkl 0xB000, b1
; mvkh 0x0000, b1
; b .S2 b1
; nop 9
;.endproc

