	.include "macros.inc"

@ WaitForField
@ r0 = entity. Yields one frame at a time with WaitFrames(1) until the
@ watched field settles, giving up after 0x3C (one second) frames so a stuck entity cannot
@ hang the caller. The same shape recurs across several overlays.
.thumb_func_start OvlFunc_964_2009038
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r6, #0x3c
.L103e:
	cmp	r6, #0
	beq	.L1054
	mov	r0, #1
	bl	__WaitFrames
	ldr	r3, [r5, #0xc]
	ldr	r2, [r5, #0x14]
	sub	r6, #1
	cmp	r3, r2
	bgt	.L103e
	b	.L1056
.L1054:
	ldr	r2, [r5, #0x14]
.L1056:
	mov	r3, #0
	str	r3, [r5, #0x28]
	mov	r3, #0x80
	lsl	r3, #24
	str	r2, [r5, #0xc]
	str	r3, [r5, #0x3c]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_964_2009038
