	.include "macros.inc"

@ ============================================================================
@ rom_185000 -- one accessor over a large record table.
@
@ The whole module is this function plus the data it indexes. It is exported
@ (Exports_185000 is entries [17] and [18] of Data_320000) and rom_b5000's
@ Func_b7b6c reaches it as _Func_185008.
@ ============================================================================

@ GetRecord185
@ r0 = index, masked to 0x0FFF. Returns .L185024 + index * 0x14.
@ The record size is 0x14 bytes: the address arithmetic is `(i*4 + i) * 4`,
@ which is `i * 20`, not a shift.
@ NO BOUNDS CHECK beyond the mask, so the caller can address 4096 records. The
@ blob behind the label runs 0x185024..0x31EFD4, far larger than 4096 * 0x14 =
@ 0x14000, so this table is only the first part of that region and the rest is
@ other data reached another way.
.thumb_func_start Func_185008
	ldr	r2, =0xfff
	mov	r3, r0
	and	r3, r2
	lsl	r0, r3, #2
	add	r0, r3
	ldr	r3, =.L185024
	lsl	r0, #2
	add	r0, r3
	bx	lr
.func_end Func_185008

	.section .rodata

.L185024:
	.incrom 0x185024, 0x31efd4
