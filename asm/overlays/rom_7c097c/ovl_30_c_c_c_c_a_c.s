	.include "macros.inc"

@ 94 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x3, TestSaveBit x2, StartLoopingSound, PlaySound
@   SetSaveBit, OvlFunc_3864, TestSaveBit x2, StartLoopingSound
@   PlaySound, SetSaveBit, OvlFunc_3864
@ reads save bits 0x202, 0x203, 0x302, 0x303; sets 0x202, 0x203.
.thumb_func_start OvlFunc_936_200b768
	push	{r5, r6, lr}
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r2, #0xa
	ldrsh	r3, [r5, r2]
	ldr	r2, =0xfffffe83
	add	r3, r2
	mov	r6, r0
	cmp	r3, #0xc
	bhi	.L37ae
	mov	r2, #0x12
	ldrsh	r3, [r5, r2]
	ldr	r2, =0x309
	cmp	r3, r2
	ble	.L37ae
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x50]
	ldr	r4, [r5, #0x50]
	ldrb	r3, [r3, #9]
	mov	r2, #0xc
	and	r2, r3
	ldrb	r1, [r4, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r1
	orr	r3, r2
	strb	r3, [r4, #9]
	b	.L37f4
.L37ae:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	bne	.L37f4
	mov	r2, #0xa
	ldrsh	r3, [r5, r2]
	cmp	r3, #0xf5
	bgt	.L37f4
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L37f4
	ldr	r0, =0x202
	bl	__GetFlag
	cmp	r0, #0
	bne	.L37ea
	mov	r0, #1
	neg	r0, r0
	bl	__Func_8091ff0
	mov	r0, #0xe6
	bl	__PlaySound
	ldr	r0, =0x202
	bl	__SetFlag
.L37ea:
	ldr	r0, [r5, #8]
	ldr	r1, [r5, #0xc]
	ldr	r2, [r5, #0x10]
	bl	OvlFunc_936_200b864
.L37f4:
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	bne	.L383c
	mov	r2, #0xa
	ldrsh	r3, [r6, r2]
	ldr	r2, =0x2c5
	cmp	r3, r2
	bgt	.L383c
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L383c
	ldr	r0, =0x203
	bl	__GetFlag
	cmp	r0, #0
	bne	.L3832
	mov	r0, #1
	neg	r0, r0
	bl	__Func_8091ff0
	mov	r0, #0xe6
	bl	__PlaySound
	ldr	r0, =0x203
	bl	__SetFlag
.L3832:
	ldr	r0, [r6, #8]
	ldr	r1, [r6, #0xc]
	ldr	r2, [r6, #0x10]
	bl	OvlFunc_936_200b864
.L383c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_936_200b768

@ 72 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, SpawnEntity, SetActorPartsPalette, SetEntityActorOptions
@   Random x2, SetEntityAnimation, SetEntityScript
.thumb_func_start OvlFunc_936_200b864
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r6, r1
	mov	r8, r2
	mov	r5, r0
	bl	__Random
	mov	r2, r0
	ldr	r3, =0xfff80000
	lsl	r2, #3
	lsr	r2, #16
	add	r5, r3
	lsl	r2, #16
	mov	r3, #0x80
	lsl	r3, #13
	add	r2, r6
	add	r2, r3
	mov	r1, r5
	mov	r0, #0xde
	mov	r3, r8
	bl	__CreateActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L38fa
	mov	r2, r5
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	ldr	r1, [r5, #0x50]
	ldrb	r2, [r1, #9]
	sub	r3, #0xd
	and	r3, r2
	mov	r2, #8
	orr	r3, r2
	strb	r3, [r1, #9]
	mov	r1, #9
	bl	__Func_80929d8
	mov	r1, #0
	mov	r0, r5
	bl	__Actor_SetSpriteFlags
	bl	__Random
	lsl	r0, #1
	lsr	r0, #16
	sub	r0, #1
	lsl	r0, #16
	str	r0, [r5, #0x24]
	bl	__Random
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #1
	lsr	r3, #16
	sub	r3, #3
	lsl	r3, #16
	mov	r2, r5
	str	r3, [r5, #0x28]
	add	r2, #0x64
	mov	r3, #0x14
	strh	r3, [r2]
	sub	r2, #3
	mov	r3, #1
	mov	r0, r5
	mov	r1, #1
	strb	r3, [r2]
	bl	__Actor_SetAnim
	ldr	r1, =gScript_936__0200d120
	mov	r0, r5
	bl	__Actor_SetScript
.L38fa:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_936_200b864
