	.include "macros.inc"

@ Data rehomed out of ovl_22c4_c_c_c_c_c.s in batch 281 so that
@ OvlFunc_924_200a8b0 could be converted to C.  Byte-identical content; only the
@ object it lives in changed, and overlay.ld's .data line was repointed to match.
@ Rehomed rather than emitted from C because docs/elevation.md's rule is to rehome
@ an .incbin of real data and to emit from C only a handful of readable words.
	.section .data

@ .global added with the rehome: .L60b8 is now referenced from another
@ object, so it must be exported.  Zero bytes -- an export declaration only.
	.global .L60b8
.L60b8:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x60b8, (0x60ec-0x60b8)
