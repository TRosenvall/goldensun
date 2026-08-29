	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L29b4, .L299c
@ on save bit 0x84e and the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_909_200809c
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x21
	cmp	r2, r3
	bne	.Le4
	ldr	r5, =.L29b4
	mov	r0, r5
	bl	__Func_808b868
	ldr	r0, =0x84e
	bl	__GetFlag
	cmp	r0, #0
	beq	.Le0
	mov	r1, r5
	mov	r3, #2
	add	r1, #0xa6
	strb	r3, [r1]
	mov	r3, r5
	mov	r2, #0
	add	r3, #0xbe
	strb	r2, [r3]
	mov	r2, r5
	add	r2, #0xd6
	mov	r3, #3
	strb	r3, [r2]
	add	r2, #0x18
	mov	r3, #1
	strb	r3, [r2]
.Le0:
	mov	r0, r5
	b	.Le6
.Le4:
	ldr	r0, =.L299c
.Le6:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_909_200809c
