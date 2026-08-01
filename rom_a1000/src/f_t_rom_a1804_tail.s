	.include "macros.inc"
	.include "gba.inc"

	.section .rodata

@ PROMOTED: referenced from rom_a172c.s across the split
	.global	Laea4c
Laea4c:
.Laea4c:
	.incrom 0xaea4c, 0xaeb4c
