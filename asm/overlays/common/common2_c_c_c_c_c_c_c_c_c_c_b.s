	.include "macros.inc"

@ Data blob split out of common2_c_c_c_c_c_c_c_c_c_c.s when OvlFunc_common2_618
@ was elevated.  A .c and a data-only .s cannot share one stem, because
@ `asm/%.o: src/%.c` builds the .o from the .c and drops the .data.
@ .L1 is LIVE -- asm/overlays/common/common2_b.s:16 has `.word .L1` -- so the
@ .global must stay.  The common_orig_deps wildcard is common2*, so this stem
@ keeps its orig.bin dependency automatically.

	.section .data
	.global .L1

.L1:
	.incbin "overlays/rom_7bf5a8/orig.bin", 0x1888, (0x189c-0x1888)
