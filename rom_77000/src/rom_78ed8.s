	.include "macros.inc"
	.include "gba.inc"

	.section .rodata

@ PROMOTED: Func_78ed8 now lives in C and reaches this across a file boundary.
	.global	L844ec
L844ec:
.L844ec:
	.incrom 0x844ec, 0x84a8c
