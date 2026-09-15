	.include "macros.inc"
	.include "gba.inc"

@ Rehomed from asm/rom_c0/rom_56cc_c_c.s when Func_8005c68 was elevated.
@ The .s carried BOTH the function and this .rodata block; a .c cannot keep a
@ stem's .rodata, so the data moves to its own object and stage1.ld's .rodata
@ line follows it. The bytes are unchanged -- still .incrom from the ROM.
@ Four string literals: "CAMELOT", "CAMELOTT", two "Trans Data" messages and
@ the flash chip id "FLASH_V123".

	.section .rodata
	.global .L79b0
	.global .L79b8

.L79b0:
	.incrom 0x79b0, 0x79b8
.L79b8:
	.incrom 0x79b8, 0x7a0c
