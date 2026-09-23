	.include "macros.inc"

@ Data rehomed out of ovl_2580_c_c.s in batch 283 so OvlFunc_947_200a74c could be
@ converted to C.  816 bytes across six .incbin blocks -- behaviour-script and table
@ data, well past docs/elevation.md's "a handful of readable words" allowance, so
@ rehomed rather than emitted from C.  All six labels were ALREADY .global in the
@ original, so no export had to be added.  Byte-identical content; only the object it
@ lives in changed, and overlay.ld's .data line was repointed to match.
	.section .data
	.global .L339c
	.global .L33a8
	.global .L3438
	.global .L3498
	.global .L351c
	.global .L3618

.L339c:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x339c, (0x33a8-0x339c)
.L33a8:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x33a8, (0x3438-0x33a8)
.L3438:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x3438, (0x3498-0x3438)
.L3498:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x3498, (0x351c-0x3498)
.L351c:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x351c, (0x3618-0x351c)
.L3618:
	.incbin "overlays/rom_7d0e88/orig.bin", 0x3618
