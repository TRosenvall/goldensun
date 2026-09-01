	.include "macros.inc"

	.section .data
	.global ActorCmd_ARRAY_886__020092fc
	.global .L1568
	.global .L1590
	.global .L15b8
	.global .L1738
	.global .L18b8
	.global gOvl_02009ac8
	.global ActorCmd_ARRAY_918__02009c00
	.global .L1da4
	.global .L1ffc
	.global gOvl_02009478

ActorCmd_ARRAY_886__020092fc:
	.incbin "overlays/rom_786f0c/orig.bin", 0x12fc, (0x1310-0x12fc)
	.global gScript_886__02009310
gScript_886__02009310:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1310, (0x1400-0x1310)
	.global gScript_886__02009400
gScript_886__02009400:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1400, (0x1440-0x1400)
	.global gScript_886__02009440
gScript_886__02009440:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1440, (0x1478-0x1440)
gOvl_02009478:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1478, (0x1568-0x1478)
.L1568:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1568, (0x1590-0x1568)
.L1590:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1590, (0x15b8-0x1590)
.L15b8:
	.incbin "overlays/rom_786f0c/orig.bin", 0x15b8, (0x1738-0x15b8)
.L1738:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1738, (0x18b8-0x1738)
.L18b8:
	.incbin "overlays/rom_786f0c/orig.bin", 0x18b8, (0x1ac8-0x18b8)
gOvl_02009ac8:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1ac8, (0x1c00-0x1ac8)
ActorCmd_ARRAY_918__02009c00:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1c00, (0x1da4-0x1c00)
.L1da4:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1da4, (0x1ffc-0x1da4)
.L1ffc:
	.incbin "overlays/rom_786f0c/orig.bin", 0x1ffc
