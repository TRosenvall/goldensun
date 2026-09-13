	.include "macros.inc"

@ Data blob split out of ovl_2dcc_c.s when its two functions were
@ elevated.  A .c and a data-only .s cannot share one stem, because
@ `asm/%.o: src/%.c` would build the .o from the .c and drop the .data.
@ .L5e70 is referenced from the elevated ovl_2dcc_c_a.c, so it must be
@ exported -- same shape as ovl_e20_c_c_c_c_c_c_c_c.s in this overlay.

	.section .data
	.global .L5e70

.L5e70:
	.incbin "overlays/rom_7ac2d8/orig.bin", 0x5e70, (0x5f20-0x5e70)
