	.include "macros.inc"
	.include "gba.inc"

@ Read-only blob split out of rom_77320_c_c_c.s when ModifyHP and ModifyPP
@ were elevated.  A .c and a data-only .s cannot share one stem, because
@ `asm/%.o: src/%.c` builds the .o from the .c and drops the .rodata.
@ .L7a828 is LIVE -- asm/rom_77000/rom_77320_a_c_c.s:260 has `ldr r3, =.L7a828`
@ -- so the .global must stay.

	.section .rodata
	.global .L7a828

.L7a828:
	.incrom 0x7a828, 0x7a830
