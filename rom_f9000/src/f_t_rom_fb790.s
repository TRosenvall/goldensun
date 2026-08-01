	.include "macros.inc"

@ XcmdNoOp
@ A bare `bx lr`. The table at .Lfba48 needs an entry that does nothing.
.thumb_func_start Func_fb790
	bx	lr
.func_end Func_fb790

	.section .rodata

@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb830
Lfb830:
.Lfb830:
	.incrom 0xfb830, 0xfb8e4
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb8e4
Lfb8e4:
.Lfb8e4:
	.incrom 0xfb8e4, 0xfb914
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb914
Lfb914:
.Lfb914:
	.incrom 0xfb914, 0xfb92c
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb92c
Lfb92c:
.Lfb92c:
	.incrom 0xfb92c, 0xfb9b0
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb9b0
Lfb9b0:
.Lfb9b0:
	.incrom 0xfb9b0, 0xfb9c8
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfb9c8
Lfb9c8:
.Lfb9c8:
	.incrom 0xfb9c8, 0xfba04
@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfba04
Lfba04:
.Lfba04:
	.incrom 0xfba04, 0xfba14

	.incdata Data_fba14, 0xfba14, 0xfba48

@ PROMOTED: referenced from rom_f9ef8.s across the split.
	.global	Lfba48
Lfba48:
.Lfba48:
	.incrom 0xfba48, 0xfc624
