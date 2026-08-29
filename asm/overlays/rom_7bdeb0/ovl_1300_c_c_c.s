	.include "macros.inc"

	.section .data

	.global .L1f00
.L1f00:
	.incbin "overlays/rom_7bdeb0/orig.bin", 0x1f00, (0x1f60-0x1f00)
