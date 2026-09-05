	.include "macros.inc"
	.include "gba.inc"

	.section .data
	.global .L40c0

.L40c0:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x40c0, (0x40c4-0x40c0)

	.section .data1
	.global .L4834
	.global .L4838
	.global gOvl_0200c474
	.global gOvl_0200c48c
	.global gOvl_0200c414

gOvl_0200c414:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4414, (0x4474-0x4414)
gOvl_0200c474:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4474, (0x448c-0x4474)
gOvl_0200c48c:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x448c, (0x4834-0x448c)
.L4834:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4834, (0x4838-0x4834)
.L4838:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x4838, (0x483c-0x4838)
	.global gOvl_0200c83c
gOvl_0200c83c:
	.incbin "overlays/rom_7ddb88/orig.bin", 0x483c, (0x4a1c-0x483c)
