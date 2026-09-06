	.include "macros.inc"


@ The .data below is reached from src/overlays/rom_7ac2d8/ovl_1db4_b.c
@ (OvlFunc_924_200a1cc), split out of this file.  It stays here because
@ split_s.py keeps trailing data with the function it follows, which would
@ have carried it into the .c and dropped it.  All four blobs are already
@ .global, so the C side needs only plain extern declarations.

	.section .data

	.global gScript_924__0200df20
gScript_924__0200df20:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5f20, (0x5f60-0x5f20)
	.global gScript_924__0200df60
gScript_924__0200df60:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5f60, (0x5fa8-0x5f60)
	.global gScript_924__0200dfa8
gScript_924__0200dfa8:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5fa8, (0x5ff0-0x5fa8)
	.global gScript_924__0200dff0
gScript_924__0200dff0:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5ff0, (0x6004-0x5ff0)
