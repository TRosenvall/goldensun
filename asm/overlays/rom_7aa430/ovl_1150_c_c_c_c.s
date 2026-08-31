	.include "macros.inc"
	.include "gba.inc"

	.section .data

	.global gScript_923__0200a820
gScript_923__0200a820:
	.incbin "overlays/rom_7aa430/orig.bin", 0x2820, (0x2874-0x2820)
	.global gScript_884__0200a874
	.global gScript_923__0200a874
gScript_884__0200a874:
gScript_923__0200a874:
	.incbin "overlays/rom_7aa430/orig.bin", 0x2874, (0x28c8-0x2874)
	.global gScript_923__0200a8c8
gScript_923__0200a8c8:
	.incbin "overlays/rom_7aa430/orig.bin", 0x28c8, (0x291c-0x28c8)
