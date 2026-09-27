	.include "macros.inc"
	.include "gba.inc"

	.section .rodata

	.global .Leedb2
.Leedb2:
	.incrom 0xeedb2, 0xeedb8
	.global .Leedb8
.Leedb8:
	.incrom 0xeedb8, 0xeedbe
	.global .Leedbe
.Leedbe:
	.incrom 0xeedbe, 0xeedca
	.global .Leedca
.Leedca:
	.incrom 0xeedca, 0xeedd0
