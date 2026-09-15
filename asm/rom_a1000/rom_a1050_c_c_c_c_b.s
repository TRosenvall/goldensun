	.include "macros.inc"
	.include "gba.inc"

@ Rehomed from asm/rom_a1000/rom_a1050_c_c_c_c.s when Func_80a15f0 was
@ elevated. A .c cannot keep a stem's .rodata, so the data moves to its own
@ object and stage1.ld's .rodata line follows it. Bytes unchanged -- still
@ .incrom from the ROM. The four labels were exported (a .global emits no
@ bytes) when Func_80a153c was split out of the parent .s.

	.section .rodata

	.global .Laf20c
	.global .Laf210
	.global .Laf214
	.global .Laf218
.Laf20c:
	.incrom 0xaf20c, 0xaf210
.Laf210:
	.incrom 0xaf210, 0xaf214
.Laf214:
	.incrom 0xaf214, 0xaf218
.Laf218:
	.incrom 0xaf218, 0xaf21c
