	.include "macros.inc"

@ ResetRecordArray
@ r0 = an array of fifteen 0x18-byte records. Rewrites every one: byte +0x16 to
@ 2, word +0x04 to 1, and the halfword at +0x00 to sprite 0x69 -- except records
@ 4 and 7, which get 0x6E instead. Called only from OvlFunc_70.
.thumb_func_start OvlFunc_912_2008030
	push	{r5, lr}
	mov	r3, #0
	mov	r5, #2
	mov	r4, #1
	mov	r1, #0x69
	mov	r2, #0x6e
.L3c:
	strb	r5, [r0, #0x16]
	str	r4, [r0, #4]
	strh	r1, [r0]
	cmp	r3, #4
	beq	.L4a
	cmp	r3, #7
	bne	.L4c
.L4a:
	strh	r2, [r0]
.L4c:
	add	r3, #1
	add	r0, #0x18
	cmp	r3, #0xe
	bls	.L3c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_912_2008030

