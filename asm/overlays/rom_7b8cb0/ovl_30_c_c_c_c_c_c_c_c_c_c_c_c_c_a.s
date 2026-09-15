	.include "macros.inc"

	.section .data
	.global .L1e70
	.global .L13f4
	.global .L140c
	.global .L15bc
	.global .L1724
	.global .L19f4
	.global .L10f0
	.global .L1120
	.global .L1288

	.incbin "overlays/rom_7b8cb0/orig.bin", 0xfc8, (0x10f0-0xfc8)
.L10f0:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x10f0, (0x1120-0x10f0)
.L1120:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1120, (0x1288-0x1120)
.L1288:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1288, (0x1390-0x1288)
	.global gOvl_02009390
gOvl_02009390:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1390, (0x13f4-0x1390)
.L13f4:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x13f4, (0x140c-0x13f4)
.L140c:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x140c, (0x15bc-0x140c)
.L15bc:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x15bc, (0x1724-0x15bc)
.L1724:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1724, (0x1730-0x1724)
	.global gScript_930__02009730
gScript_930__02009730:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1730, (0x19f4-0x1730)
.L19f4:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x19f4, (0x1e70-0x19f4)
.L1e70:
	.incbin "overlays/rom_7b8cb0/orig.bin", 0x1e70
