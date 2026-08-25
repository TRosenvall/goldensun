	.include "macros.inc"

@ Slot 0: map-load entry. Dispatches to OvlFunc_4b4, _4e8 or _538 for areas
@ 0x31, 0x30 and 0x2F respectively; any other area needs no setup.
.thumb_func_start OvlFunc_920_200846c
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x31
	cmp	r2, r3
	bne	.L486
	bl	OvlFunc_920_20084b4
	b	.L49c
.L486:
	ldr	r3, =0x30
	cmp	r2, r3
	bne	.L492
	bl	OvlFunc_920_20084e8
	b	.L49c
.L492:
	ldr	r3, =0x2f
	cmp	r2, r3
	bne	.L49c
	bl	OvlFunc_920_2008538
.L49c:
	mov	r0, #0
	pop	{r1}
	bx	r1
.func_end OvlFunc_920_200846c
