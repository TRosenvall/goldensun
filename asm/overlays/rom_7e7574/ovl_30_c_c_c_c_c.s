	.include "macros.inc"

	.section .data
	.global .L5ed8
	.global .L5f30
	.global .L5ed8
	.global .L5f18
	.global .L5f30
	.global .L5ed8
	.global .L5f18

.L5ed8:
	.incbin "overlays/rom_7e7574/orig.bin", 0x5ed8, (0x5f18-0x5ed8)
.L5f18:
	.incbin "overlays/rom_7e7574/orig.bin", 0x5f18, (0x5f30-0x5f18)
.L5f30:
	.incbin "overlays/rom_7e7574/orig.bin", 0x5f30, (0x5f90-0x5f30)
