	.include "macros.inc"

@ Leaf helper, 36 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1ebc
.thumb_func_start OvlFunc_945_2009144
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xc
	mov	r4, #0xc
	ldr	r3, [r3]
	neg	r2, r2
	neg	r4, r4
	add	r4, r1
	add	r2, r0
	mov	r6, r0
	mov	r5, #8
	mov	r14, r2
	add	r6, #0xc
	mov	r12, r4
	add	r1, #0xc
	add	r3, #0x34
.L1164:
	ldmia	r3!, {r0}
	mov	r7, #0xa
	ldrsh	r2, [r0, r7]
	mov	r7, #0x12
	ldrsh	r4, [r0, r7]
	cmp	r14, r2
	bge	.L117e
	cmp	r6, r2
	ble	.L117e
	cmp	r12, r4
	bge	.L117e
	cmp	r1, r4
	bgt	.L1186
.L117e:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L1164
	mov	r0, #0
.L1186:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_2009144
