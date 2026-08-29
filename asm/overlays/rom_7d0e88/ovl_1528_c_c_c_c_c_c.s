	.include "macros.inc"
	.include "gba.inc"

	.section .data
	.global .L2da8
	.global .L2dd2
	.global .L2dfc
	.global .L2e26
	.global .L2e50

.L2da8:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x2da8, (0x2dd2-0x2da8)
.L2dd2:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x2dd2, (0x2dfc-0x2dd2)
.L2dfc:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x2dfc, (0x2e26-0x2dfc)
.L2e26:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x2e26, (0x2e50-0x2e26)
.L2e50:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x2e50, (0x2e7c-0x2e50)

	.section .bss
	.global .L3720
	.global .L372c
	.global .L3738

	.space	4

	.global	bss_36d0
bss_36d0:
	.space	0x50
	.ssize	bss_36d0

.L3720:
	.space	0xc
.L372c:
	.space	0xc
.L3738:
	.space	4
