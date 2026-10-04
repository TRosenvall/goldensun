	.include "macros.inc"
	.include "gba.inc"

	.section .rodata
	.global .Leebe9
	.global .Leebec
	.global .Leec28
	.global .Leec2f
	.global .Leec36
	.global .Leec3d
	.global .Leec44
	.global .Leec52

.Leebe9:
	.incrom 0xeebe9, 0xeebec
.Leebec:
	.incrom 0xeebec, 0xeec28
.Leec28:
	.incrom 0xeec28, 0xeec2f
.Leec2f:
	.incrom 0xeec2f, 0xeec36
.Leec36:
	.incrom 0xeec36, 0xeec3d
.Leec3d:
	.incrom 0xeec3d, 0xeec44
.Leec44:
	.incrom 0xeec44, 0xeec52
.Leec52:
	.incrom 0xeec52, 0xeec5a
