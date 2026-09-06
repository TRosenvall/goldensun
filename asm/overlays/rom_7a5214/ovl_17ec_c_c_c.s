	.include "macros.inc"

	.section .data
	.global .L1ca8

.L1ca8:
	.incbin "overlays/rom_7a5214/orig.bin", 0x1ca8, (0x1da8-0x1ca8)
