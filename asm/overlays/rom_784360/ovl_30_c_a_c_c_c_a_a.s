	.include "macros.inc"

@ Cutscene: roughly 240 instructions of straight-line script --
@ 0 turns, 1 animation change, 0 dialogue lines, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Reads save bits 0x815, 0x834, 0x840, 0x842.
@ Sets save bit 0x20.
.thumb_func_start OvlFunc_884_2008940
	push	{r5, lr}
	ldr	r0, =0x90b
	sub	sp, #8
	bl	__GetFlag
	cmp	r0, #0
	beq	.L958
	mov	r0, #8
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L958:
	ldr	r0, =0x90c
	bl	__GetFlag
	cmp	r0, #0
	beq	.L96c
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L96c:
	ldr	r0, =0x90d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L980
	mov	r0, #0xa
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.L980:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x62
	beq	.L9a2
	cmp	r3, #0x62
	bgt	.L99c
	cmp	r3, #0x61
	bne	.L99a
	b	.Lb4a
.L99a:
	b	.L9b6
.L99c:
	cmp	r3, #0x63
	beq	.L9b0
	b	.L9b6
.L9a2:
	mov	r0, #0x20
	bl	__SetFlag
	mov	r0, #0x32
	bl	__Func_8091e9c
	b	.Lb80
.L9b0:
	bl	OvlFunc_884_200a5b0
	b	.Lb80
.L9b6:
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r5, #0xc0
	lsl	r5, #9
	str	r5, [r0, #0x1c]
	mov	r0, #9
	bl	__MapActor_GetActor
	str	r5, [r0, #0x1c]
	mov	r0, #0xa
	bl	__MapActor_GetActor
	str	r5, [r0, #0x1c]
	ldr	r0, =0x87a
	bl	__GetFlag
	cmp	r0, #0
	beq	.La0e
	mov	r3, #2
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x61
	mov	r1, #2
	mov	r2, #0x50
	mov	r3, #5
	bl	__CopyMapTiles
	mov	r3, #3
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2a
	mov	r1, #0x35
	mov	r2, #0x2a
	mov	r3, #0x36
	bl	__CopyMapTiles
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	b	.Lb80
.La0e:
	ldr	r0, =0x834
	bl	__GetFlag
	cmp	r0, #0
	beq	.La62
	bl	__StartRain
	bl	__StartThunder
	mov	r3, #0x12
	mov	r2, #0x29
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x15
	mov	r1, #0x26
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r0, #0x84
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.Laaa
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0x80
	ldr	r2, =gScript_884__0200ac00
	mov	r0, #0x13
	lsl	r1, #9
	bl	__Func_8092a1c
	b	.Laaa
.La62:
	ldr	r0, =0x815
	bl	__GetFlag
	cmp	r0, #0
	beq	.Laaa
	mov	r1, #0xb4
	mov	r2, #0x8e
	mov	r0, #0x10
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r3, #2
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0x5c
	mov	r1, #2
	mov	r2, #0x50
	mov	r3, #5
	bl	__CopyMapTiles
	mov	r3, #3
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2a
	mov	r1, #0x35
	mov	r2, #0x2a
	mov	r3, #0x36
	bl	__CopyMapTiles
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
.Laaa:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xc
	bne	.Lac0
	bl	OvlFunc_884_20097c8
	b	.Lb80
.Lac0:
	ldr	r0, =0x834
	bl	__GetFlag
	cmp	r0, #0
	beq	.Laf8
	mov	r0, #0x14
	bl	__MapActor_GetActor
	ldr	r3, =0x4ccc
	str	r3, [r0, #0x18]
	str	r3, [r0, #0x1c]
	mov	r0, #0x14
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x15
	bl	__MapActor_GetActor
	ldr	r3, =0x9999
	mov	r1, #5
	str	r3, [r0, #0x18]
	str	r3, [r0, #0x1c]
	mov	r0, #0xd
	bl	__MapActor_SetAnim
	b	.Lb1a
.Laf8:
	ldr	r0, =0x815
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb1a
	mov	r2, #0xf9
	ldr	r1, =0x14b0000
	mov	r0, #0x15
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
.Lb1a:
	mov	r0, #0x84
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb3a
	mov	r0, #0x1a
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.Lb3a:
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x13
	bne	.Lb50
.Lb4a:
	bl	OvlFunc_884_20095b4
	b	.Lb80
.Lb50:
	ldr	r0, =0x834
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb6a
	ldr	r0, =0x842
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb6a
	bl	OvlFunc_884_2009084
	b	.Lb80
.Lb6a:
	ldr	r0, =0x834
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb80
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	bl	__Func_8095268
.Lb80:
	mov	r0, #0
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_884_2008940

@ Cutscene: roughly 455 instructions of straight-line script --
@ 13 turns, 17 animation changes, 7 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xeb6.
@ Reads save bits 0x834, 0x840.
@ Sets save bit 0x840.
.thumb_func_start OvlFunc_884_2008bbc
	push	{r5, r6, lr}
	ldr	r0, =0x834
	mov	r6, #0
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lbcc
	b	.L1054
.Lbcc:
	mov	r0, #0x84
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lbda
	b	.L1054
.Lbda:
	bl	__CutsceneStart
	ldr	r0, =0x19999
	ldr	r1, =0x3333
	bl	__Func_80933d4
	mov	r0, #0xc5
	mov	r1, #1
	mov	r2, #0xc0
	mov	r3, #1
	lsl	r2, #18
	neg	r1, r1
	lsl	r0, #16
	bl	__Func_80933f8
	bl	__Func_8093530
	ldr	r0, =0xeb6
	bl	__MessageID
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_80925cc
	ldr	r0, =0x4013
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_8093040
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x19
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0
	mov	r1, #0xb3
	ldr	r2, =0x315
	bl	__Func_80921c4
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lc4e
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x19
	bl	__MapActor_SetPos
.Lc4e:
	mov	r2, #0xc9
	mov	r0, #0x19
	mov	r1, #0xb3
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #0x19
	mov	r2, #0x28
	bl	__Func_8092848
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0
	mov	r0, #0x19
	mov	r1, #0
	bl	__Func_8092adc
	mov	r0, #0x11
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x12
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r2, #0
	mov	r1, #0x12
	mov	r0, #0x11
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x11
	mov	r1, #1
	bl	__Func_809259c
	mov	r2, #0xa
	ldr	r0, =0x4011
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0x12
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_8093040
	mov	r0, #0x11
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xf0
	mov	r2, #0xa
	mov	r0, #0x12
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0x13
	mov	r1, #3
	bl	__MapActor_DoAnim
	ldr	r0, =0x4013
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_8093040
	mov	r0, #0x11
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	ldr	r2, =0xcccc
	mov	r0, #0x12
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_884__0200aef0
	mov	r0, #0x11
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x12
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x3c
	mov	r0, #0x19
	lsl	r1, #8
	bl	__Func_8092adc
	ldr	r1, =gScript_884__0200af50
	mov	r0, #0
	bl	__MapActor_SetBehavior
	ldr	r1, =gScript_884__0200af78
	mov	r0, #0x19
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0xa
	mov	r0, #0x19
	mov	r1, #0
	bl	__Func_8092adc
	mov	r0, #0x19
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0x13
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x14
	lsl	r1, #7
	mov	r0, #0x1a
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x1a
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1a
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x19
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, #0
	ldr	r0, =0x4013
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #1
	bne	.Ldec
	mov	r0, #0x13
	mov	r1, #4
	mov	r6, #1
	bl	__MapActor_SetAnim
	b	.Le04

	.pool_aligned

.Ldec:
	mov	r0, #0x13
	mov	r1, #3
	bl	__MapActor_SetAnim
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Le04:
	ldr	r0, =0x4013
	mov	r1, #0
	bl	__ActorMessage
	cmp	r6, #0
	beq	.Le20
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Le20:
	mov	r5, #0x80
	lsl	r5, #7
	mov	r1, r5
	mov	r2, #0x1e
	mov	r0, #0x16
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0x13
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x1a
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x19
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, #0xa0
	mov	r0, #0x13
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0x1a
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r6, #0xe0
	mov	r1, #0xe0
	lsl	r6, #8
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r6
	mov	r2, #0xa
	mov	r0, #0x19
	bl	OvlFunc_884_200a2e0
	ldr	r0, =0x13333
	ldr	r1, =0x2666
	bl	__Func_80933d4
	mov	r0, #0xd7
	mov	r1, #1
	ldr	r2, =0x2f60000
	mov	r3, #1
	lsl	r0, #16
	neg	r1, r1
	bl	__Func_80933f8
	bl	__Func_8093530
	ldr	r0, =0xcccc
	ldr	r1, =0x1999
	bl	__Func_80933d4
	mov	r0, #0xcd
	mov	r1, #1
	mov	r3, #1
	ldr	r2, =0x30a0000
	lsl	r0, #16
	neg	r1, r1
	bl	__Func_80933f8
	ldr	r1, =gScript_884__0200a874
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r0, #0x16
	bl	__MapActor_WaitScript
	mov	r1, #0x80
	mov	r2, #0x3c
	lsl	r1, #6
	mov	r0, #0x16
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x13
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0x16
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x16
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r1, #3
	mov	r0, #0x13
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, r5
	mov	r2, #0x1e
	mov	r0, #0x13
	add	r5, #0x13
	bl	OvlFunc_884_200a2e0
	mov	r0, r5
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, r6
	mov	r2, #0x1e
	mov	r0, #0x1a
	mov	r6, #0x80
	bl	OvlFunc_884_200a2e0
	lsl	r6, #8
	mov	r0, #0x1a
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r2, #0x1e
	mov	r0, #0x13
	mov	r1, r6
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x13
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r5
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0
	mov	r1, #0x19
	mov	r2, #0x28
	bl	__Func_8092848
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r2, #0x1e
	mov	r0, #0x1a
	mov	r1, r6
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x1a
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x1a
	mov	r1, #0x1e
	bl	OvlFunc_884_200a2c8
	mov	r1, #0xc0
	mov	r2, #0x1e
	lsl	r1, #8
	mov	r0, #0x1a
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x1a
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x16
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x19
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lfcc
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x19
	bl	__MapActor_TravelTo
.Lfcc:
	mov	r0, #0x19
	bl	__MapActor_WaitMovement
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x1a
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lffc
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x1a
	bl	__MapActor_TravelTo
.Lffc:
	mov	r0, #0x1a
	bl	__MapActor_WaitMovement
	mov	r0, #0x1a
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x16
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L102c
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x16
	bl	__MapActor_TravelTo
.L102c:
	mov	r0, #0x16
	bl	__MapActor_WaitMovement
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0x80
	ldr	r2, =gScript_884__0200ac00
	mov	r0, #0x13
	lsl	r1, #9
	bl	__Func_8092a1c
	mov	r0, #0x84
	lsl	r0, #4
	bl	__SetFlag
	bl	__CutsceneEnd
.L1054:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_884_2008bbc
