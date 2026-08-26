	.include "macros.inc"

	.section .data
	.global .L5d50
	.global .L5d90
	.global .L5da8

.L5d50:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5d50, (0x5d90-0x5d50)
.L5d90:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5d90, (0x5da8-0x5d90)
.L5da8:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5da8, (0x5e08-0x5da8)
