	.include "macros.inc"
	.include "gba.inc"

	.section .rodata
	.global .Laf2e4
	.global .Laf2a6
	.global .Laf2b1
	.global .Laf2bc
	.global .Laf2d0
	.global .Laf294
	.global .Laf29d

.Laf294:
	.incrom 0xaf294, 0xaf29d
.Laf29d:
	.incrom 0xaf29d, 0xaf2a6
.Laf2a6:
	.incrom 0xaf2a6, 0xaf2b1
.Laf2b1:
	.incrom 0xaf2b1, 0xaf2bc
.Laf2bc:
	.incrom 0xaf2bc, 0xaf2d0
.Laf2d0:
	.incrom 0xaf2d0, 0xaf2e4
.Laf2e4:
	.incrom 0xaf2e4, 0xaf2fc
