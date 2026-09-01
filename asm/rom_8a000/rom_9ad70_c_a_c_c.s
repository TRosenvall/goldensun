	.include "macros.inc"

@ FindTargetInRangeB
@ r0=effect state. Same scan as Func_9b364 over a second candidate set -- the
@ two differ only in which table they walk, so an ability can look for either
@ kind of target.
.thumb_func_start Func_809b3d8  @ 0x0809b3d8
	push	{r5, r6, lr}
	ldr	r3, =gState
	mov	r5, #0xed
	lsl	r5, #1
	ldr	r1, [r0, #0x14]
	mov	r2, #0xa0
	lsl	r2, #12
	add	r3, r5
	add	r4, r1, r2
	mov	r5, #0
	ldrsh	r2, [r3, r5]
	ldr	r3, =1
	ldr	r6, [r0, #0x68]
	cmp	r2, r3
	bne	.L9b3fc
	mov	r2, #0x80
	lsl	r2, #11
	add	r4, r1, r2
.L9b3fc:
	ldr	r5, [r0, #0xc]
	cmp	r5, r4
	bgt	.L9b408
	bl	_DeleteActor
	b	.L9b43c
.L9b408:
	ldr	r3, [r0, #0x18]
	mov	r4, #0xc0
	lsl	r4, #4
	mov	r1, #0x80
	add	r2, r3, r4
	lsl	r1, #9
	cmp	r2, r1
	ble	.L9b41a
	mov	r2, r1
.L9b41a:
	neg	r3, r2
	str	r2, [r0, #0x18]
	str	r3, [r0, #0x1c]
	ldr	r4, =0xfffe0000
	ldr	r3, [r6, #8]
	str	r3, [r0, #8]
	add	r3, r5, r4
	str	r3, [r0, #0xc]
	sub	r3, r1, r2
	lsl	r2, r3, #2
	add	r2, r3
	ldr	r3, [r6, #0x10]
	mov	r5, #0x80
	sub	r3, r2
	lsl	r5, #13
	add	r3, r5
	str	r3, [r0, #0x10]
.L9b43c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_809b3d8

@ RunMoveAbility
@ Takes no arguments. The push/move field ability: locates the target, animates
@ the caster, and slides the target to its new tile. The ~130-instruction body
@ is characterised structurally.
.thumb_func_start Func_809b450  @ 0x0809b450
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f30
	ldr	r3, [r3]
	ldr	r1, [r3, #0x10]
	sub	sp, #0xc
	mov	r2, #4
	mov	r10, r3
	add	r2, sp
	mov	r3, #0x3f
	str	r1, [sp]
	mov	r6, r0
	mov	r7, #0
	mov	r9, r2
	mov	r11, r3
.L9b478:
	ldr	r2, [r6, #0xc]
	mov	r3, #0x80
	lsl	r3, #15
	ldr	r1, [r6, #8]
	add	r2, r3
	mov	r0, #0x1a
	ldr	r3, [r6, #0x10]
	bl	_CreateActor
	lsl	r3, r7, #2
	mov	r1, r9
	str	r0, [r3, r1]
	cmp	r0, #0
	beq	.L9b538
	ldr	r3, [r6, #0x14]
	str	r3, [r0, #0x14]
	mov	r3, r0
	add	r3, #0x55
	mov	r2, #0
	ldr	r5, [r0, #0x50]
	strb	r2, [r3]
	add	r3, #0xf
	strh	r2, [r3]
	ldr	r1, .L9b4e0	@ 0
	ldr	r3, =0x6666
	mov	r8, r1
	str	r6, [r0, #0x68]
	str	r3, [r0, #0x1c]
	str	r3, [r0, #0x18]
	cmp	r5, #0
	beq	.L9b538
	mov	r1, #0
	mov	r0, r5
	bl	_Sprite_SetAnim
	mov	r3, r5
	add	r3, #0x26
	mov	r2, r8
	strb	r2, [r3]
	ldrb	r0, [r5, #0x1c]
	bl	Func_8003f3c
	ldr	r3, =0x71a
	add	r3, r10
	ldrh	r3, [r3]
	strb	r3, [r5, #0x1c]
	ldrb	r3, [r5, #0x1d]
	mov	r2, #1
	orr	r3, r2
	strb	r3, [r5, #0x1d]
	b	.L9b4f0

	.align	2, 0
.L9b4e0:
	.word	0
	.pool

.L9b4f0:
	ldrb	r3, [r5, #0x1c]
	ldr	r2, =gSpriteSlots
	lsl	r3, #2
	add	r3, r2
	ldrh	r1, [r3, #2]
	ldr	r2, .L9b530	@ 0xfffffc00
	ldrh	r3, [r5, #8]
	lsl	r1, #17
	lsr	r1, #22
	and	r3, r2
	orr	r3, r1
	mov	r1, #0x21
	neg	r1, r1
	strh	r3, [r5, #8]
	ldrb	r3, [r5, #5]
	mov	r2, r1
	and	r3, r2
	mov	r2, r11
	and	r3, r2
	mov	r2, #0x40
	orr	r3, r2
	ldrb	r2, [r5, #7]
	strb	r3, [r5, #5]
	mov	r3, r11
	and	r3, r2
	mov	r2, #0x80
	orr	r3, r2
	strb	r3, [r5, #7]
	ldr	r3, [r5, #0x28]
	mov	r1, r8
	strb	r1, [r3, #0x16]
	b	.L9b538

	.align	2, 0
.L9b530:
	.word	0xfffffc00
	.pool

.L9b538:
	add	r7, #1
	cmp	r7, #1
	ble	.L9b478
	ldr	r2, [sp, #4]
	ldr	r3, =Func_809b3d8
	ldr	r0, [r2, #0x50]
	str	r3, [r2, #0x6c]
	mov	r2, #0xd
	ldrb	r1, [r0, #9]
	neg	r2, r2
	mov	r3, r2
	and	r3, r1
	strb	r3, [r0, #9]
	mov	r3, r9
	ldr	r1, [r3, #4]
	ldr	r3, =Func_809b364
	str	r3, [r1, #0x6c]
	ldr	r0, [r1, #0x50]
	ldr	r1, [sp]
	ldr	r3, [r1, #0x50]
	ldrb	r1, [r3, #9]
	mov	r3, #0xc
	and	r3, r1
	ldrb	r1, [r0, #9]
	and	r2, r1
	orr	r2, r3
	strb	r2, [r0, #9]
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_809b450
