	.include "macros.inc"

@ DATA HALF of the old ovl_314_c_c_c.s.  Its one function became
@ src/overlays/rom_7b6668/ovl_314_c_c_c_b.c in batch 299; datacheck confirmed the
@ function reads none of these labels, so no export had to be added.

	.section .data
	.global gScript_928__020095b0
	.global gScript_928__020096a0
	.global .L1714
	.global .L1778
	.global .L178e
	.global .L17a4
	.global .L17ba
	.global .L17d0
	.global .L1900
	.global .L1740
	.global gOvl_020097e8

gScript_928__020095b0:
	.incbin "overlays/rom_7b6668/orig.bin", 0x15b0, (0x16a0-0x15b0)
gScript_928__020096a0:
	.incbin "overlays/rom_7b6668/orig.bin", 0x16a0, (0x1714-0x16a0)
.L1714:
	.incbin "overlays/rom_7b6668/orig.bin", 0x1714, (0x1740-0x1714)
.L1740:
	.incbin "overlays/rom_7b6668/orig.bin", 0x1740, (0x1778-0x1740)
.L1778:
	.incbin "overlays/rom_7b6668/orig.bin", 0x1778, (0x178e-0x1778)
.L178e:
	.incbin "overlays/rom_7b6668/orig.bin", 0x178e, (0x17a4-0x178e)
.L17a4:
	.incbin "overlays/rom_7b6668/orig.bin", 0x17a4, (0x17ba-0x17a4)
.L17ba:
	.incbin "overlays/rom_7b6668/orig.bin", 0x17ba, (0x17d0-0x17ba)
.L17d0:
	.incbin "overlays/rom_7b6668/orig.bin", 0x17d0, (0x17e8-0x17d0)
gOvl_020097e8:
	.incbin "overlays/rom_7b6668/orig.bin", 0x17e8, (0x18d8-0x17e8)
	.global gOvl_020098d8
gOvl_020098d8:
	.incbin "overlays/rom_7b6668/orig.bin", 0x18d8, (0x1900-0x18d8)
.L1900:
	.incbin "overlays/rom_7b6668/orig.bin", 0x1900, (0x1ac8-0x1900)
	.global gOvl_02009ac8
gOvl_02009ac8:
	.incbin "overlays/rom_7b6668/orig.bin", 0x1ac8
