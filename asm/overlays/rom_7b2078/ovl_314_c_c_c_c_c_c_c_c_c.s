	.include "macros.inc"

	.section .data
	.global gScript_926__0200c638
	.global .L477a
	.global .L4790
	.global .L48f0
	.global .L4998
	.global .L4ae8
	.global .L4b90
	.global .L4d40
	.global .L5184
	.global gScript_943__0200c7a8
	.global .L4838

gScript_926__0200c638:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4638, (0x4764-0x4638)
	.global gScript_943__0200c764
gScript_943__0200c764:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4764, (0x477a-0x4764)
.L477a:
	.incbin "overlays/rom_7b2078/orig.bin", 0x477a, (0x4790-0x477a)
.L4790:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4790, (0x47a8-0x4790)
gScript_943__0200c7a8:
	.incbin "overlays/rom_7b2078/orig.bin", 0x47a8, (0x4838-0x47a8)
.L4838:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4838, (0x48c8-0x4838)
	.global gOvl_0200c8c8
gOvl_0200c8c8:
	.incbin "overlays/rom_7b2078/orig.bin", 0x48c8, (0x48f0-0x48c8)
.L48f0:
	.incbin "overlays/rom_7b2078/orig.bin", 0x48f0, (0x4998-0x48f0)
.L4998:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4998, (0x4ae8-0x4998)
.L4ae8:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4ae8, (0x4b90-0x4ae8)
.L4b90:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4b90, (0x4d40-0x4b90)
.L4d40:
	.incbin "overlays/rom_7b2078/orig.bin", 0x4d40, (0x5184-0x4d40)
.L5184:
	.incbin "overlays/rom_7b2078/orig.bin", 0x5184, (0x51d8-0x5184)
	.global .L51d8
.L51d8:
	.incbin "overlays/rom_7b2078/orig.bin", 0x51d8
