	.include "macros.inc"

@ Cutscene: roughly 174 instructions of straight-line script --
@ 3 turns, 1 animation change, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bit 0x941.
.thumb_func_start OvlFunc_959_200cda0
	push	{r5, r6, r7, lr}
	ldr	r5, =.L5fa4
	bl	__Random
	lsl	r3, r0, #3
	sub	r3, r0
	lsr	r3, #16
	ldr	r7, =gState
	mov	r1, #0xe0
	str	r3, [r5]
	lsl	r1, #1
	add	r5, r7, r1
	mov	r3, #0
	ldrsh	r2, [r5, r3]
	ldr	r3, =0xa0
	ldrh	r1, [r5]
	cmp	r2, r3
	bne	.L4dd2
	mov	r0, #0xe0
	lsl	r0, #4
	bl	__Func_80108c4
	bl	OvlFunc_959_200cf60
	ldrh	r1, [r5]
.L4dd2:
	lsl	r3, r1, #16
	ldr	r2, =0xa1
	asr	r3, #16
	cmp	r3, r2
	bne	.L4de2
	bl	OvlFunc_959_200d0e4
	ldrh	r1, [r5]
.L4de2:
	lsl	r3, r1, #16
	ldr	r2, =0xa2
	asr	r3, #16
	cmp	r3, r2
	bne	.L4df2
	bl	OvlFunc_959_200d324
	ldrh	r1, [r5]
.L4df2:
	lsl	r3, r1, #16
	ldr	r2, =0xa3
	asr	r3, #16
	cmp	r3, r2
	beq	.L4dfe
	b	.L4f32
.L4dfe:
	ldr	r3, =iwram_3001ebc
	mov	r1, #0xe0
	ldr	r3, [r3]
	lsl	r1, #1
	mov	r2, #0x81
	add	r3, r1
	lsl	r2, #2
	str	r2, [r3]
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, #0
	mov	r0, #0xc
	mov	r1, #0
	bl	__Func_8092adc
	mov	r0, #0xc
	mov	r1, #0
	bl	__MapActor_SetAnim
	bl	OvlFunc_959_200d520
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L4e42
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L4e42:
	mov	r3, r5
	add	r3, #0x23
	mov	r6, #2
	strb	r6, [r3]
	mov	r0, #9
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L4e5c
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L4e5c:
	mov	r3, r5
	add	r3, #0x23
	strb	r6, [r3]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L4e74
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L4e74:
	mov	r3, r5
	add	r3, #0x23
	mov	r0, #0xe0
	strb	r6, [r3]
	lsl	r0, #4
	bl	__Func_80108c4
	mov	r2, #0xe1
	lsl	r2, #1
	add	r5, r7, r2
	mov	r1, #0
	ldrsh	r3, [r5, r1]
	ldrh	r2, [r5]
	cmp	r3, #4
	bne	.L4ea0
	mov	r0, #0xc0
	lsl	r0, #4
	bl	__Func_80108c4
	bl	OvlFunc_959_200b054
	ldrh	r2, [r5]
.L4ea0:
	lsl	r3, r2, #16
	mov	r2, #0xc0
	lsl	r2, #10
	cmp	r3, r2
	bne	.L4f14
	mov	r0, #0xc0
	lsl	r0, #4
	bl	__Func_80108c4
	ldr	r0, =0x941
	bl	__GetFlag
	cmp	r0, #0
	beq	.L4f14
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xd8
	mov	r2, #0xac
	mov	r0, #0x10
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xa0
	mov	r0, #0x10
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe4
	mov	r2, #0x90
	mov	r0, #0xd
	lsl	r1, #17
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xa0
	mov	r0, #0xd
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe4
	mov	r2, #0xa0
	lsl	r1, #17
	mov	r0, #0x11
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r0, #0x11
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L4f14:
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.L4f26
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.L4f26:
	mov	r2, r5
	mov	r3, #2
	add	r2, #0x23
	strb	r3, [r2]
	ldr	r3, =0xcccc
	str	r3, [r5, #0x18]
.L4f32:
	mov	r0, #0
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_200cda0

@ 120 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_5470, RegisterTask x5, WaitFrames, UpdateMapView
@   WaitFrames, CopyMapRectFull, SetMapLayerBits, RegisterTask
@   SetMapLayerBits, SetSlotPalette x5, WaitFrames
.thumb_func_start OvlFunc_959_200cf60
	push	{lr}
	sub	sp, #8
	bl	OvlFunc_959_200d470
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #2
	cmp	r3, #0x11
	bhi	.L5076
	ldr	r2, =.L4f84
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L4f84:
	.word	.L4fcc
	.word	.L4fcc
	.word	.L4fcc
	.word	.L4fcc
	.word	.L4fcc
	.word	.L4fcc
	.word	.L5076
	.word	.L5076
	.word	.L5076
	.word	.L5076
	.word	.L4ffa
	.word	.L505c
	.word	.L505c
	.word	.L505c
	.word	.L500c
	.word	.L500c
	.word	.L500c
	.word	.L4ffa
.L4fcc:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0xc8
	add	r2, #0x40
	str	r2, [r3]
	lsl	r1, #4
	ldr	r0, =OvlFunc_959_2009718
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_959_200975c
	bl	__StartTask
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_959_20097bc
	lsl	r1, #4
	bl	__StartTask
	b	.L5052
.L4ffa:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0xc0
	add	r2, #0x49
	str	r2, [r3]
	b	.L5054
.L500c:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0xc8
	add	r2, #0x40
	str	r2, [r3]
	lsl	r1, #4
	ldr	r0, =OvlFunc_959_2009528
	bl	__StartTask
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =OvlFunc_959_20099e8
	bl	__StartTask
	mov	r0, #1
	bl	__WaitFrames
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r3, #0x6e
	mov	r2, #9
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x65
	mov	r1, #9
	mov	r2, #0xa
	mov	r3, #8
	bl	__Func_80105d4
.L5052:
	mov	r0, #0xe0
.L5054:
	lsl	r0, #4
	bl	__Func_80108c4
	b	.L508c
.L505c:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0xc8
	add	r2, #0x40
	str	r2, [r3]
	ldr	r0, =OvlFunc_959_200969c
	lsl	r1, #4
	bl	__StartTask
	b	.L508c
.L5076:
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	mov	r0, #0xe0
	add	r2, #0x40
	str	r2, [r3]
	lsl	r0, #4
	bl	__Func_80108c4
.L508c:
	mov	r0, #0x12
	mov	r1, #1
	bl	__Func_8092950
	mov	r0, #0x11
	mov	r1, #1
	bl	__Func_8092950
	mov	r0, #0x15
	mov	r1, #1
	bl	__Func_8092950
	mov	r0, #0xc
	mov	r1, #1
	bl	__Func_8092950
	mov	r1, #1
	mov	r0, #0xd
	bl	__Func_8092950
	mov	r0, #1
	bl	__WaitFrames
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_200cf60
