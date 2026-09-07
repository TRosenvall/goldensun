	.include "macros.inc"

	.section .data
	.global gOvl_02008920
	.global .L948
	.global .L978
	.global .L990
	.global .L9f0
	.global .L818
	.global .L8d8

.L818:
	.incbin "overlays/rom_79aad8/orig.bin", 0x818, (0x8d8-0x818)
.L8d8:
	.incbin "overlays/rom_79aad8/orig.bin", 0x8d8, (0x920-0x8d8)
gOvl_02008920:
	.incbin "overlays/rom_79aad8/orig.bin", 0x920, (0x948-0x920)
.L948:
	.incbin "overlays/rom_79aad8/orig.bin", 0x948, (0x978-0x948)
.L978:
	.incbin "overlays/rom_79aad8/orig.bin", 0x978, (0x990-0x978)
.L990:
	.incbin "overlays/rom_79aad8/orig.bin", 0x990, (0x9f0-0x990)
.L9f0:
	.incbin "overlays/rom_79aad8/orig.bin", 0x9f0
