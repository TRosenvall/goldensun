	.include "macros.inc"
	.include "gba.inc"

@ 118 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random x3, SetActorPartsPalette, Random, DestroyEntity
.thumb_func_start OvlFunc_932_200aa48
	push	{r5, r6, lr}
	ldr	r3, =.L523c
	ldr	r3, [r3]
	mov	r5, r0
	cmp	r3, #0
	beq	.L2a62
	mov	r1, #0x80
	lsl	r1, #8
	cmp	r3, r1
	beq	.L2a82
	mov	r6, r5
	add	r6, #0x64
	b	.L2aa2
.L2a62:
	bl	__Random
	mov	r6, r5
	lsl	r0, #1
	add	r6, #0x64
	lsr	r0, #16
	mov	r3, #0
	ldrsh	r2, [r6, r3]
	sub	r0, #1
	lsl	r0, #16
	ldr	r3, [r5, #8]
	lsl	r2, #12
	asr	r0, #1
	add	r2, r0
	add	r3, r2
	b	.L2aa0
.L2a82:
	bl	__Random
	mov	r6, r5
	lsl	r0, #1
	add	r6, #0x64
	lsr	r0, #16
	mov	r1, #0
	ldrsh	r2, [r6, r1]
	sub	r0, #1
	lsl	r0, #16
	ldr	r3, [r5, #8]
	lsl	r2, #12
	asr	r0, #1
	add	r2, r0
	sub	r3, r2
.L2aa0:
	str	r3, [r5, #8]
.L2aa2:
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	cmp	r3, #3
	bgt	.L2ade
	ldr	r3, =.L523c
	ldr	r3, [r3]
	cmp	r3, #0
	beq	.L2abc
	mov	r1, #0x80
	lsl	r1, #8
	cmp	r3, r1
	beq	.L2ac6
	b	.L2ace
.L2abc:
	ldr	r3, [r5, #8]
	mov	r2, #0x80
	lsl	r2, #8
	add	r3, r2
	b	.L2acc
.L2ac6:
	ldr	r3, [r5, #8]
	ldr	r1, =0xffff8000
	add	r3, r1
.L2acc:
	str	r3, [r5, #8]
.L2ace:
	ldr	r3, [r5, #0x18]
	ldr	r2, =0x1999
	add	r3, r2
	str	r3, [r5, #0x18]
	ldr	r1, =0xfffff334
	ldr	r3, [r5, #0x1c]
	add	r3, r1
	b	.L2af2
.L2ade:
	ldr	r3, [r5, #0x10]
	ldr	r2, =0x13333
	add	r3, r2
	str	r3, [r5, #0x10]
	ldr	r2, =0x7ae
	ldr	r3, [r5, #0x18]
	add	r3, r2
	str	r3, [r5, #0x18]
	ldr	r3, [r5, #0x1c]
	add	r3, r2
.L2af2:
	str	r3, [r5, #0x1c]
	bl	__Random
	mov	r1, #0
	ldrsh	r3, [r6, r1]
	mul	r3, r0
	lsr	r3, #16
	ldrh	r2, [r6]
	cmp	r3, #0
	bne	.L2b10
	mov	r0, r5
	mov	r1, #7
	bl	__Func_80929d8
	ldrh	r2, [r6]
.L2b10:
	lsl	r3, r2, #16
	cmp	r3, #0
	beq	.L2b1a
	sub	r3, r2, #2
	b	.L2b28
.L2b1a:
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsr	r3, #16
	lsl	r3, #1
	add	r3, #2
.L2b28:
	strh	r3, [r6]
	ldr	r3, [r5, #0x68]
	sub	r3, #1
	str	r3, [r5, #0x68]
	cmp	r3, #0
	bne	.L2b3a
	mov	r0, r5
	bl	__DeleteActor
.L2b3a:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200aa48

@ 34 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SpawnEntity, OvlFunc_2a10, SetEntityAnimation
.thumb_func_start OvlFunc_932_200ab58
	push	{r5, lr}
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #3
	and	r3, r2
	cmp	r3, #0
	bne	.L2b9c
	ldr	r3, =.L5240
	mov	r0, #0xde
	ldr	r1, [r3]
	ldr	r2, [r3, #4]
	ldr	r3, [r3, #8]
	bl	__CreateActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L2b9c
	mov	r2, r5
	add	r2, #0x64
	mov	r3, #0x1e
	strh	r3, [r2]
	add	r2, #2
	mov	r3, #1
	strh	r3, [r2]
	mov	r3, #0x14
	str	r3, [r5, #0x68]
	bl	OvlFunc_932_200aa10
	ldr	r3, =OvlFunc_932_200aa48
	mov	r0, r5
	str	r3, [r5, #0x6c]
	mov	r1, #1
	bl	__Actor_SetAnim
.L2b9c:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200ab58
