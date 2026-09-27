	.include "macros.inc"
	.include "gba.inc"

	.section .data
	.global gOvl_020095c0
	.global .L1a00
	.global .L15f8
	.global .L1610
	.global gScript_930__020096b8
	.global .L1790
	.global .L19c4
	.global .L17b4
	.global gScript_960__020097a8
	.global .L1458
	.global gOvl_02009488
	.global .L14d0
	.global .L1548

	.incbin "overlays/rom_7eaf28/orig.bin", 0x1430, (0x1458-0x1430)
.L1458:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1458, (0x1488-0x1458)
gOvl_02009488:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1488, (0x14d0-0x1488)
.L14d0:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x14d0, (0x1548-0x14d0)
.L1548:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1548, (0x15c0-0x1548)
gOvl_020095c0:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x15c0, (0x15f8-0x15c0)
.L15f8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x15f8, (0x1610-0x15f8)
.L1610:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1610, (0x16b8-0x1610)
gScript_930__020096b8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x16b8, (0x1790-0x16b8)
.L1790:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1790, (0x17a8-0x1790)
gScript_960__020097a8:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x17a8, (0x17b4-0x17a8)
.L17b4:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x17b4, (0x19c4-0x17b4)
.L19c4:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x19c4, (0x1a00-0x19c4)
.L1a00:
	.incbin "overlays/rom_7eaf28/orig.bin", 0x1a00
