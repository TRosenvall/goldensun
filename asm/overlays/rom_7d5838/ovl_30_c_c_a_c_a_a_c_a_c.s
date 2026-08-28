	.include "macros.inc"

@ Slot 0: map-load entry.
@
@ Reconstructs the town from save bits 0x8AB and 0x8BC: each selects a
@ Func_10704 attribute repaint and a MapActor_SetPos placement, so objects and
@ terrain match whatever the player has already done. Func_91dc8 puts the
@ screen overlay up, and .gcc2_compiled. leaves the affected actors facing the
@ right way.
.thumb_func_start OvlFunc_950_20083dc
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0x95
	add	r2, #0x49
	str	r2, [r3]
	lsl	r0, #4
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L4e2
	mov	r3, #0x33
	mov	r2, #0x2d
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x2f
	mov	r2, #3
	mov	r3, #1
	mov	r0, #0x33
	bl	__Func_8010704
	mov	r0, #0x1f
	bl	__MapActor_GetActor
	mov	r2, #0
	mov	r3, r0
	mov	r8, r2
	add	r3, #0x23
	mov	r2, r8
	strb	r2, [r3]
	ldr	r1, [r0, #0x50]
	mov	r5, #0xd
	ldrb	r2, [r1, #9]
	neg	r5, r5
	mov	r3, r5
	and	r3, r2
	mov	r6, #8
	orr	r3, r6
	strb	r3, [r1, #9]
	mov	r0, #0x20
	bl	__MapActor_GetActor
	mov	r3, r0
	add	r3, #0x23
	mov	r2, r8
	strb	r2, [r3]
	ldr	r2, [r0, #0x50]
	ldrb	r3, [r2, #9]
	and	r5, r3
	orr	r5, r6
	strb	r5, [r2, #9]
	ldr	r0, =0x8bc
	bl	__GetFlag
	cmp	r0, #0
	beq	.L472
	mov	r1, #0x8c
	mov	r2, #0xaa
	mov	r0, #0x19
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0x19
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
.L472:
	ldr	r5, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r5, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x13
	bne	.L49a
	ldr	r0, =0x8bc
	bl	__GetFlag
	cmp	r0, #0
	bne	.L49a
	ldr	r0, =0x8bc
	bl	__SetFlag
	bl	__MapTransitionIn
	bl	OvlFunc_950_200813c
.L49a:
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r5, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x10
	bne	.L4c4
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	bne	.L4c4
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__SetFlag
	bl	__MapTransitionIn
	bl	OvlFunc_950_2008328
.L4c4:
	ldr	r0, =0x8ab
	bl	__GetFlag
	cmp	r0, #0
	beq	.L4e2
	mov	r0, #0x23
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x24
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L4e2:
	mov	r0, #0
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_950_20083dc

@ Counter: shop 0x1F, speaker slot 0x0D. Lines 0x238D / 0x221B / 0x1FD5
@ across the three states; gated on save bit 0x962.
.thumb_func_start OvlFunc_950_2008500
	push	{r5, r6, lr}
	mov	r6, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldrh	r3, [r0, #6]
	lsl	r2, #6
	add	r3, r2
	ldr	r2, =0xffffc000
	and	r3, r2
	mov	r2, #0x80
	lsl	r3, #16
	lsl	r2, #24
	cmp	r3, r2
	bne	.L52a
	mov	r0, #0x1c
	mov	r1, r6
	bl	__Func_80b0278
	b	.L596
.L52a:
	mov	r0, #0x95
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L544
	ldr	r0, =0x238d
	b	.L550

	.pool_aligned

.L544:
	ldr	r0, =0x962
	bl	__GetFlag
	cmp	r0, #0
	beq	.L55e
	ldr	r0, =0x221b
.L550:
	bl	__MessageID
	mov	r0, r6
	mov	r1, #0
	bl	__ActorMessage
	b	.L596
.L55e:
	ldr	r5, =0x1fd5
	mov	r0, r5
	bl	__MessageID
	mov	r1, #0
	mov	r0, r6
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L588
	mov	r0, #0xa
	bl	__CutsceneWait
	add	r0, r5, #1
	bl	__MessageID
	b	.L58e
.L588:
	add	r0, r5, #2
	bl	__MessageID
.L58e:
	mov	r0, r6
	mov	r1, #0
	bl	__ActorMessage
.L596:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_950_2008500

@ Counter: shop 0x1F at a second position, lines 0x2389 / 0x2219 / 0x1FD2,
@ with an interaction effect before the dialogue.
.thumb_func_start OvlFunc_950_20085a8
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldrh	r3, [r0, #6]
	lsl	r2, #6
	add	r3, r2
	ldr	r2, =0xffffc000
	and	r3, r2
	mov	r2, #0xc0
	lsl	r3, #16
	lsl	r2, #24
	cmp	r3, r2
	bne	.L5d8
	mov	r0, #0x1a
	mov	r1, r5
	bl	__Func_80b0278
	b	.L654

	.pool_aligned

.L5d8:
	mov	r0, #0x95
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L612
	ldr	r6, =0x2389
	mov	r0, r6
	bl	__MessageID
	mov	r1, #0
	mov	r0, r5
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L60a
	mov	r0, #0xa
	bl	__CutsceneWait
	add	r0, r6, #1
	b	.L61e
.L60a:
	add	r0, r6, #2
	bl	__MessageID
	b	.L622
.L612:
	ldr	r0, =0x962
	bl	__GetFlag
	cmp	r0, #0
	beq	.L62c
	ldr	r0, =0x2219
.L61e:
	bl	__MessageID
.L622:
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	b	.L654
.L62c:
	ldr	r0, =0x1fd2
	bl	__MessageID
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x83
	lsl	r1, #1
	mov	r0, r5
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
.L654:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_950_20085a8
