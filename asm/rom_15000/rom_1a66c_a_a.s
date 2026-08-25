	.include "macros.inc"


@ InitPartyScreen
@ Takes no arguments. Allocates the 0x3E4-byte party/status block under tag 0x12
@ -- which is what iwram_1e98 points at, since iwram_1e50 + 0x12*4 = iwram_1e98
@ -- clears its pointer fields at +0x348 onward, and reserves OBJ tiles with
@ UploadSpriteGFX / AllocSpriteSlot.
@ Every +0xNNN offset elsewhere in this file and rom_1aeec.s is inside this
@ block and bounded by 0x3E4.
.thumb_func_start Func_801a66c  @ 0x0801a66c
	push	{r5, r6, r7, lr}
	mov	r1, #0xf9
	lsl	r1, #2
	mov	r0, #0x12
	bl	galloc_ewram
	mov	r2, #0xd2
	mov	r7, r0
	lsl	r2, #2
	mov	r5, #0
	add	r3, r7, r2
	add	r2, #4
	str	r5, [r3]
	add	r3, r7, r2
	add	r2, #4
	str	r5, [r3]
	add	r3, r7, r2
	add	r2, #0x4a
	str	r5, [r3]
	add	r3, r7, r2
	add	r2, #2
	strh	r5, [r3]
	add	r3, r7, r2
	strh	r5, [r3]
	ldr	r3, =0x39e
	add	r2, r7, r3
	mov	r3, #0x80
	strh	r3, [r2]
	mov	r3, #0xe8
	lsl	r3, #2
	add	r2, r7, r3
	mov	r3, #0x20
	strh	r3, [r2]
	mov	r2, #0xe5
	lsl	r2, #2
	add	r3, r7, r2
	strh	r5, [r3]
	mov	r3, #0xee
	lsl	r3, #2
	add	r2, r7, r3
	add	r3, #0x2f
	strh	r3, [r2]
	mov	r3, #0xef
	lsl	r3, #1
	add	r2, r7, r3
	mov	r3, r7
	mov	r1, #0
	mov	r0, #0
	add	r3, #0x72
.L1a6ce:
	add	r1, #1
	strh	r0, [r3]
	strh	r0, [r2]
	add	r3, #0x34
	add	r2, #0x34
	cmp	r1, #5
	bne	.L1a6ce
	mov	r2, #0xba
	lsl	r2, #1
	mov	r5, #0
	add	r3, r7, r2
	add	r2, #0x34
	strh	r5, [r3, #2]
	add	r3, r7, r2
	strh	r5, [r3, #2]
	mov	r3, r7
	add	r3, #0x46
	strh	r5, [r7, #0xa]
	strh	r5, [r7, #0x3e]
	strh	r5, [r7, #0x12]
	strh	r5, [r3]
	ldr	r6, =Data_346f8
	bl	AllocSpriteSlot
	mov	r2, #0xb9
	lsl	r2, #2
	add	r3, r7, r2
	strh	r0, [r3]
	mov	r1, #0x80
	mov	r2, r6
	ldrh	r0, [r3]
	lsl	r1, #1
	bl	UploadSpriteGFX
	ldr	r2, =0x2e6
	add	r3, r7, r2
	sub	r2, #4
	strh	r0, [r3]
	add	r3, r7, r2
	add	r2, #0x18
	strh	r5, [r3]
	add	r3, r7, r2
	add	r2, #0x1c
	strh	r5, [r3]
	add	r3, r7, r2
	strh	r5, [r3]
	mov	r3, #0xc0
	lsl	r3, #2
	add	r5, r7, r3
	mov	r0, #0xd
	ldrb	r3, [r5, #5]
	neg	r0, r0
	mov	r2, r0
	and	r2, r3
	mov	r3, #0x11
	neg	r3, r3
	and	r2, r3
	mov	r3, #0x20
	orr	r2, r3
	mov	r3, #4
	ldrb	r1, [r5, #7]
	neg	r3, r3
	and	r2, r3
	sub	r3, #0x3b
	mov	r4, #0x3f
	and	r3, r1
	and	r3, r4
	mov	r1, #0x40
	orr	r3, r1
	strb	r3, [r5, #7]
	ldrb	r3, [r5, #9]
	and	r2, r4
	and	r0, r3
	strb	r2, [r5, #5]
	strb	r0, [r5, #9]
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_801a66c

@ ResetPartyScreenState
@ Takes no arguments. Clears the working pointer at [iwram_1e98]+0x348 and the
@ halfword at +0x39A, then rewinds the entry counter at +0x39E.
.thumb_func_start Func_801a778  @ 0x0801a778
	push	{lr}
	ldr	r3, =iwram_3001e98
	mov	r2, #0xd2
	ldr	r1, [r3]
	lsl	r2, #2
	add	r3, r1, r2
	mov	r0, #0
	add	r2, #0x52
	str	r0, [r3]
	add	r3, r1, r2
	strh	r0, [r3]
	ldr	r3, =0x39e
	add	r4, r1, r3
	ldrh	r2, [r4]
	mov	r3, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.L1a7a6
	mov	r2, #0xe7
	lsl	r2, #2
	add	r3, r1, r2
	strh	r0, [r3]
	strh	r0, [r4]
.L1a7a6:
	mov	r2, #0xe8
	lsl	r2, #2
	add	r3, r1, r2
	sub	r2, #0xc
	strh	r0, [r3]
	add	r3, r1, r2
	strh	r0, [r3]
	pop	{r0}
	bx	r0
.func_end Func_801a778
