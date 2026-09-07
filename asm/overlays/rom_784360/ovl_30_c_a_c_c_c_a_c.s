	.include "macros.inc"

@ Cutscene: roughly 292 instructions of straight-line script --
@ 7 turns, 7 animation changes, 2 dialogue lines, 8 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Sets save bit 0x11a.
.thumb_func_start OvlFunc_884_2009274
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r1, #0xf
	mov	r0, #0x19
	bl	__Func_8092950
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r2, =0x14b0000
	mov	r1, #0
	mov	r0, #0x19
	bl	__MapActor_SetPos
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =0x1019
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0x17
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x18
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r0, #0x19
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0xa0
	mov	r0, #0x17
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0xa0
	lsl	r2, #7
	mov	r8, r2
	mov	r1, r8
	mov	r2, #0x28
	mov	r0, #0x18
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #9
	lsl	r1, #6
	bl	__Func_80933d4
	mov	r0, #0xb2
	mov	r1, #0xb0
	mov	r3, #1
	lsl	r1, #16
	ldr	r2, =0x1390000
	lsl	r0, #15
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0xe0
	mov	r0, #0x17
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r2, #0x28
	lsl	r1, #7
	mov	r0, #0x18
	bl	OvlFunc_884_200a2e0
	ldr	r0, =0xcccc
	ldr	r1, =0x1999
	bl	__Func_80933d4
	mov	r0, #0xc8
	mov	r1, #0x90
	mov	r3, #1
	lsl	r0, #15
	lsl	r1, #16
	ldr	r2, =0x14d0000
	bl	__Func_80933f8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x17
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x18
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0x69
	ldr	r2, =0x149
	mov	r0, #0x17
	bl	__Func_809218c
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r2, =0x149
	mov	r1, #0x7c
	mov	r0, #0x18
	bl	__Func_809218c
	mov	r0, #0x17
	bl	__MapActor_WaitMovement
	mov	r0, #0x17
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, r8
	mov	r2, #0
	mov	r0, #0x17
	bl	__Func_8092adc
	mov	r0, #0x18
	bl	__MapActor_WaitMovement
	mov	r0, #0x18
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r2, #0
	mov	r1, r8
	mov	r0, #0x18
	bl	__Func_8092adc
	mov	r1, #0
	mov	r0, #0x19
	bl	__Func_8092950
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x19
	mov	r1, #0
	ldr	r2, =0x14b0000
	bl	__MapActor_SetPos
	mov	r0, #0x19
	ldr	r1, =0x13333
	ldr	r2, =0x9999
	bl	__MapActor_SetSpeed
	ldr	r2, =0x153
	mov	r1, #0x25
	mov	r0, #0x19
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x17
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r5, #0xd0
	mov	r1, #0
	mov	r0, #0x17
	bl	__Func_8092c40
	lsl	r5, #8
	mov	r0, #0x19
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0xa
	mov	r1, r5
	mov	r0, #0
	bl	OvlFunc_884_200a2e0
	mov	r1, #0
	mov	r0, #0
	bl	__Func_8091c7c
	mov	r6, #0x80
	mov	r0, #0x28
	bl	__CutsceneWait
	lsl	r6, #8
	mov	r0, #0x17
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x14
	mov	r1, r6
	mov	r0, #0x18
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x19
	mov	r1, #2
	bl	__Func_80925cc
	ldr	r0, =0x1019
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	ldr	r2, =gScript_884__0200ac00
	mov	r0, #0
	ldr	r1, =0x10019
	bl	__Func_8092a1c
	mov	r0, #0x19
	mov	r1, #0x5d
	ldr	r2, =0x169
	bl	__Func_80921c4
	mov	r2, #0x28
	mov	r1, r5
	mov	r0, #0x19
	bl	OvlFunc_884_200a2e0
	mov	r1, #0x14
	mov	r0, #0x19
	bl	OvlFunc_884_200a2c8
	mov	r0, #0
	bl	__MapActor_SetIdle
	mov	r0, #0x17
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0xf
	mov	r1, r6
	mov	r0, #0x18
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x17
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x18
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, r8
	mov	r0, #0x17
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x1e
	mov	r1, r8
	mov	r0, #0x18
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x18
	mov	r1, #4
	bl	__MapActor_DoAnim
	ldr	r0, =0x2018
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, r5
	mov	r0, #0
	mov	r2, #0x1e
	bl	OvlFunc_884_200a2e0
	mov	r1, #0x80
	mov	r2, #0x28
	lsl	r1, #7
	mov	r0, #0
	bl	OvlFunc_884_200a2e0
	mov	r0, #0x17
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x17
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x17
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x19
	bl	__MapActor_Surprise
	mov	r0, #0x50
	bl	__CutsceneWait
	ldr	r1, =gScript_884__0200a8e8
	mov	r0, #0x18
	bl	__MapActor_SetBehavior
	mov	r0, #6
	bl	__CutsceneWait
	ldr	r1, =gScript_884__0200a940
	mov	r0, #0x17
	bl	__MapActor_SetBehavior
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r1, =gScript_884__0200a998
	mov	r0, #0
	bl	__MapActor_SetBehavior
	mov	r0, #6
	bl	__CutsceneWait
	ldr	r1, =gScript_884__0200a9f0
	mov	r0, #0x19
	bl	__MapActor_RunScript
	ldr	r3, =gState
	ldr	r2, =0x22b
	add	r3, r2
	mov	r2, #2
	strb	r2, [r3]
	ldr	r5, =5
	mov	r1, #0x13
	mov	r0, r5
	bl	__Func_8091f90
	mov	r0, r5
	mov	r1, #0x13
	bl	__Func_8091fa8
	mov	r0, #0xc
	mov	r1, #4
	bl	__Func_8091eb0
	mov	r0, #0x8d
	lsl	r0, #1
	bl	__SetFlag
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_884_2009274

@ Cutscene: roughly 192 instructions of straight-line script --
@ 6 turns, 4 animation changes, 0 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_884_20095b4
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r0, #0x11
	bl	__PlaySound
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	neg	r2, r2
	mov	r3, #0
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x80
	mov	r1, #0x90
	mov	r2, #0xaf
	mov	r3, #0
	lsl	r1, #16
	lsl	r2, #17
	lsl	r0, #15
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0xc0
	mov	r2, #0xad
	mov	r0, #0
	lsl	r1, #14
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0x9c
	mov	r2, #0xb3
	mov	r0, #0x19
	lsl	r1, #15
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xce
	mov	r2, #0xab
	mov	r0, #0x17
	lsl	r1, #15
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xe0
	mov	r2, #0xb4
	mov	r0, #0x18
	lsl	r1, #15
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0x17
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x18
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0x10
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r3, =0xffff0000
	str	r3, [r0, #0x18]
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #7
	mov	r0, #0x19
	bl	__MapActor_SetAnim
	mov	r0, #0x19
	bl	__MapActor_GetActor
	ldr	r3, =0x1555
	ldr	r2, [r0, #0x50]
	mov	r0, #0x19
	strh	r3, [r2, #0x1e]
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r2, =iwram_3001ebc
	mov	r8, r2
	ldr	r3, [r2]
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r2
	sub	r2, #0xc0
	str	r2, [r3]
	bl	__MapTransitionIn
	bl	__Func_8095268
	bl	__WaitMapTransition
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r1, #0xc0
	mov	r0, #0x17
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0x18
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #0x17
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x18
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #0x80
	mov	r0, #0x17
	lsl	r1, #8
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0xa
	mov	r0, #0x18
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #3
	bl	__Func_8092b08
	mov	r0, #0x19
	mov	r1, #3
	bl	__Func_8092b08
	ldr	r2, =0x13333
	ldr	r1, =0x26666
	mov	r0, #0x17
	bl	__MapActor_SetSpeed
	mov	r0, #0x17
	bl	__MapActor_GetActor
	ldr	r6, =0x28f
	mov	r5, #0x80
	lsl	r5, #8
	str	r6, [r0, #0x44]
	str	r5, [r0, #0x48]
	ldr	r1, =gScript_884__0200aa48
	mov	r0, #0x17
	bl	__MapActor_SetBehavior
	mov	r0, #0x18
	bl	__CutsceneWait
	ldr	r2, =0x13333
	ldr	r1, =0x26666
	mov	r0, #0x18
	bl	__MapActor_SetSpeed
	mov	r0, #0x18
	bl	__MapActor_GetActor
	ldr	r1, =gScript_884__0200ab2c
	str	r6, [r0, #0x44]
	str	r5, [r0, #0x48]
	mov	r0, #0x18
	bl	__MapActor_RunScript
	mov	r0, #0x28
	bl	__CutsceneWait
	bl	__Func_8095214
	bl	__Func_8095240
	mov	r0, #0x14
	bl	__WaitFrames
	bl	__Func_8095240
	mov	r0, #0x3c
	bl	__WaitFrames
	bl	__Func_8095240
	mov	r0, #0x14
	bl	__WaitFrames
	bl	__Func_8095214
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, r8
	ldr	r3, [r2]
	mov	r2, #0xe4
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0x78
	str	r2, [r3]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	ldr	r0, =0x834
	bl	__ClearFlag
	mov	r0, #9
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_884_20095b4

@ Cutscene: roughly 1048 instructions of straight-line script --
@ 11 turns, 46 animation changes, 4 dialogue lines, 30 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x11fa.
.thumb_func_start OvlFunc_884_20097c8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r0, #1
	sub	sp, #8
	bl	__Func_807808c
	bl	__CutsceneStart
	mov	r3, #3
	mov	r2, #1
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2a
	mov	r1, #0x35
	mov	r2, #0x2a
	mov	r3, #0x36
	bl	__CopyMapTiles
	mov	r0, #0xb4
	mov	r1, #0x80
	mov	r3, #0
	ldr	r2, =0x26a0000
	lsl	r1, #13
	lsl	r0, #16
	bl	__Func_80933f8
	bl	__Func_800fe9c
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x17
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1d
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r0, #0
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #1
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x11
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0x10
	mov	r1, #1
	bl	__Func_8092b08
	mov	r0, #0xf
	mov	r1, #1
	bl	__Func_8092b08
	mov	r1, #0xd0
	lsl	r1, #16
	ldr	r2, =0x32e0000
	mov	r0, #0
	bl	__MapActor_SetPos
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #0xc
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r3, #0xe0
	lsl	r3, #7
	mov	r8, r3
	mov	r2, #0x14
	mov	r1, r8
	mov	r0, #0xc
	bl	OvlFunc_884_200a2e0
	ldr	r0, =0x11fa
	bl	__MessageID
	mov	r0, #0xc
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x81
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r3, #0x80
	lsl	r3, #5
	mov	r11, r3
	mov	r2, #0xa
	mov	r0, #0xb
	mov	r1, r11
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #3
	mov	r0, #0xc
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0xb
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x80
	mov	r0, #0xc
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r7, #0xc0
	mov	r0, #0xc
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	lsl	r7, #6
	bl	__MapActor_SetSpeed
	mov	r0, #0xc
	mov	r1, #0xb8
	ldr	r2, =0x26a
	bl	__Func_80921c4
	mov	r2, #0x3c
	mov	r0, #0xc
	mov	r1, r7
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xb
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0xb
	mov	r1, #0xa8
	ldr	r2, =0x26a
	bl	__Func_80921c4
	mov	r1, #0xf0
	mov	r2, #0xa
	mov	r0, #0xb
	lsl	r1, #8
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r0, #0xb
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r2, #0xa
	mov	r0, #0xc
	mov	r1, r8
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #0xc
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x1e
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	mov	r1, #0xdc
	mov	r2, #0xba
	lsl	r2, #18
	lsl	r1, #15
	mov	r0, #0x1e
	bl	__MapActor_SetPos
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0x1e
	mov	r1, #3
	bl	__MapActor_SetAnim
	ldr	r1, =gScript_884__0200ac14
	mov	r0, #0x1e
	bl	__MapActor_SetBehavior
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r5, =gScript_884__0200ac00
	mov	r0, #0xb
	ldr	r1, =0x1001e
	mov	r2, r5
	bl	__Func_8092a1c
	ldr	r1, =0x1001e
	mov	r2, r5
	mov	r0, #0xc
	bl	__Func_8092a1c
	mov	r0, #0x1e
	bl	__MapActor_WaitScript
	mov	r0, #0xb
	bl	__MapActor_SetIdle
	mov	r0, #0xc
	bl	__MapActor_SetIdle
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0xb
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0xc
	ldr	r1, =0x105
	mov	r2, #0x78
	bl	__MapActor_Emote
	mov	r0, #0xb
	mov	r1, r11
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xc
	mov	r1, r8
	mov	r2, #0x50
	bl	OvlFunc_884_200a2e0
	mov	r3, #0xa0
	lsl	r3, #7
	mov	r9, r3
	mov	r0, #0xb
	mov	r1, r9
	mov	r2, #0x28
	bl	OvlFunc_884_200a2e0
	mov	r2, #0x14
	mov	r0, #0xb
	mov	r1, r11
	bl	OvlFunc_884_200a2e0
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xc
	mov	r1, r9
	mov	r2, #0x3c
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, r7
	mov	r2, #0x28
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, r9
	mov	r2, #0x3c
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	ldr	r1, =0x101
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r0, #0xb
	mov	r1, r7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xb8
	ldr	r2, =0x276
	mov	r0, #0xc
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xc
	mov	r1, r7
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, r9
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	mov	r1, r7
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r0, #0xb
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, #0xa8
	ldr	r2, =0x276
	mov	r0, #0xb
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, r7
	mov	r2, #0x28
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, r9
	mov	r2, #0x28
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, r7
	mov	r2, #0x28
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r2, #0xa
	mov	r0, #0xb
	mov	r1, r11
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r0, #0xc
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xc
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x80
	mov	r0, #0xb
	lsl	r1, #1
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r0, #0xb
	mov	r1, r9
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, r7
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, r9
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r2, #0x3c
	mov	r0, #0xb
	mov	r1, r9
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1b66
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x1f
	bl	__MapActor_SetPos
.L1b66:
	mov	r0, #2
	bl	__WaitFrames
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r5, #0xfe
	mov	r3, r5
	and	r3, r2
	strb	r3, [r0]
	mov	r0, #0x1f
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	and	r5, r3
	strb	r5, [r0]
	mov	r1, #2
	mov	r0, #0x1e
	bl	__Func_8092b08
	mov	r0, #0x1f
	mov	r1, #2
	bl	__Func_8092b08
	ldr	r2, =0x1cccc
	mov	r0, #0x1f
	ldr	r1, =0x39999
	bl	__MapActor_SetSpeed
	mov	r0, #0x1f
	mov	r1, #2
	bl	__MapActor_SetAnim
	ldr	r5, =gScript_884__0200ac90
	mov	r0, #0x1f
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x1e
	mov	r1, #3
	bl	__MapActor_SetAnim
	ldr	r2, =0x26666
	mov	r0, #0x1e
	ldr	r1, =0x4cccc
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r0, #0x1e
	bl	__MapActor_RunScript
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0xc
	mov	r1, #2
	bl	__Func_80925cc
	mov	r2, #0xa
	mov	r1, r8
	mov	r0, #0xc
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xc
	b	.L1c3c

	.pool_aligned

.L1c3c:
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0xa
	mov	r1, r11
	mov	r0, #0xb
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xb
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	mov	r1, #3
	mov	r0, #0xc
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xb
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	ldr	r2, =0x13333
	mov	r0, #0xc
	ldr	r1, =0x26666
	bl	__MapActor_SetSpeed
	ldr	r1, =gScript_884__0200acf8
	mov	r0, #0xb
	bl	__MapActor_SetBehavior
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r1, =0x4ccc
	ldr	r0, =0x26666
	bl	__Func_80933d4
	bl	__Func_8093554
	mov	r6, #0
	add	r0, #0x55
	strb	r6, [r0]
	mov	r1, #0x80
	mov	r0, #0xd7
	mov	r3, #1
	ldr	r2, =0x3210000
	lsl	r1, #13
	lsl	r0, #16
	bl	__Func_80933f8
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r1, =gScript_884__0200ad74
	mov	r0, #0xc
	bl	__MapActor_SetBehavior
	mov	r0, #0xc
	bl	__MapActor_WaitScript
	mov	r2, #0x78
	mov	r1, r7
	mov	r0, #0xc
	bl	OvlFunc_884_200a2e0
	mov	r1, #2
	mov	r0, #0xd
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r6, #0x90
	mov	r0, #0xd
	mov	r1, #0x14
	bl	OvlFunc_884_200a2c8
	lsl	r6, #8
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r6
	mov	r0, #1
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r3, #0xc0
	lsl	r3, #8
	mov	r10, r3
	mov	r1, r10
	mov	r0, #0
	mov	r2, #0xa
	bl	OvlFunc_884_200a2e0
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r8, r3
	mov	r2, #0xa
	mov	r1, r8
	mov	r0, #1
	bl	OvlFunc_884_200a2e0
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #0x10
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x10
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x10
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r2, #0xc8
	mov	r0, #0x10
	mov	r1, #0xd8
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x10
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r1, #0
	mov	r0, #0xb4
	bl	__Func_8091a58
	mov	r1, #0x84
	mov	r2, #0xc8
	mov	r0, #0x10
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r0, #0x10
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, #0xf0
	mov	r2, #0xa
	mov	r0, #1
	lsl	r1, #8
	bl	OvlFunc_884_200a2e0
	mov	r0, #1
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0x11
	mov	r1, #4
	bl	__MapActor_DoAnim
	mov	r0, #0x11
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, r11
	mov	r0, #1
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #1
	ldr	r1, =0x103
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r2, #0x3c
	mov	r0, #1
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r5, #0xd0
	mov	r1, #2
	mov	r0, #0xe
	bl	__Func_80925cc
	lsl	r5, #8
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, r5
	mov	r2, #0xa
	mov	r0, #0xe
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xe
	mov	r1, #0x3c
	bl	OvlFunc_884_200a2c8
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #0x11
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #0x12
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #0x13
	lsl	r1, #1
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x11
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0x11
	mov	r1, #0x3c
	bl	OvlFunc_884_200a2c8
	mov	r0, #0
	mov	r1, #0x11
	bl	__MapActor_SetExtra
	mov	r0, #1
	mov	r1, #0x11
	bl	__MapActor_SetExtra
	mov	r2, #0xc8
	mov	r0, #0x11
	mov	r1, #0xd8
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x11
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r0, #0x11
	mov	r1, #0x3c
	bl	OvlFunc_884_200a2c8
	mov	r1, #0
	mov	r0, #0xcf
	bl	__Func_8091a58
	mov	r0, #0
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__MapActor_SetIdle
	mov	r1, #0x88
	mov	r2, #0xcc
	mov	r0, #0x11
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x11
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #1
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, r6
	mov	r2, #0xa
	mov	r0, #1
	bl	OvlFunc_884_200a2e0
	mov	r0, #1
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, r7
	mov	r0, #0xe
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0xa
	bl	OvlFunc_884_200a2e0
	mov	r2, #0x3c
	mov	r0, #0
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #1
	bl	__Func_80925cc
	mov	r0, #0x10
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, #0xf0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0xa
	bl	OvlFunc_884_200a2e0
	mov	r2, #0x14
	mov	r0, #1
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #4
	bl	__MapActor_DoAnim
	mov	r0, #0x10
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r0, #0x12
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x12
	mov	r1, #0
	bl	__ActorMessage
	mov	r2, #0
	mov	r1, r5
	mov	r0, #1
	bl	__Func_8092adc
	mov	r0, #0x12
	mov	r1, #4
	bl	__MapActor_SetAnim
	mov	r0, #0x12
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x12
	mov	r1, #3
	bl	__Func_809259c
	mov	r0, #0x12
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x13
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x11
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x18
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x12
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x1b
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0x1c
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0x19
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x15
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xf
	mov	r1, #2
	mov	r2, #0xa
	bl	__MapActor_Jump
	mov	r2, #0x28
	mov	r0, #0xf
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r0, #0xf
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, r8
	mov	r0, #1
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r10
	mov	r0, #0
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r1, r5
	mov	r2, #0xa
	mov	r0, #0xf
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xf
	mov	r1, #0xa
	bl	OvlFunc_884_200a2c8
	mov	r1, r6
	mov	r0, #0xf
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	mov	r2, #0xa
	mov	r1, r9
	mov	r0, #0xf
	bl	OvlFunc_884_200a2e0
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0xe
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x11
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x17
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x1a
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x1d
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xc
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0xf
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x12
	b	.L20a4

	.pool_aligned

.L20a4:
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x15
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x18
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x1b
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xd
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x10
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x13
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x16
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0x19
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x1c
	bl	__MapActor_DoAnim
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #0xb
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0xe
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x11
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x14
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x17
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x1a
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x1d
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0xc
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0xf
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x12
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x15
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x18
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x1b
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0xd
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x10
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x13
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x16
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r0, #0x19
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r2, #0
	mov	r0, #0x1c
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r1, #1
	ldr	r0, =0x1214
	bl	__Func_801776c
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	mov	r5, #1
	orr	r3, r5
	strb	r3, [r0]
	mov	r0, #1
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r3, [r0]
	mov	r1, #0x81
	orr	r5, r3
	strb	r5, [r0]
	lsl	r1, #1
	mov	r0, #0
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r1, #0x80
	lsl	r1, #7
	mov	r0, #0
	mov	r2, #0xa
	bl	OvlFunc_884_200a2e0
	mov	r1, r9
	mov	r0, #1
	mov	r2, #0x14
	bl	OvlFunc_884_200a2e0
	ldr	r2, =0x6666
	mov	r0, #0
	ldr	r1, =0xcccc
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_884__0200adf0
	mov	r0, #0
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0xd8
	mov	r1, #0x80
	mov	r3, #1
	lsl	r1, #13
	ldr	r2, =0x3890000
	lsl	r0, #16
	bl	__Func_80933f8
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r2, =0x6666
	mov	r0, #1
	ldr	r1, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r0, #1
	bl	__MapActor_SetBehavior
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r3, =iwram_3001ebc
	ldr	r1, [r3]
	mov	r3, #0xe0
	lsl	r3, #1
	add	r2, r1, r3
	sub	r3, #0xc0
	str	r3, [r2]
	add	r3, #0xc8
	add	r2, r1, r3
	mov	r3, #0x3c
	str	r3, [r2]
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__MapActor_SetIdle
	mov	r0, #0xa
	bl	__Func_8091e9c
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_884_20097c8
