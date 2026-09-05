	.include "macros.inc"

@ Cutscene: roughly 163 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1e44.
.thumb_func_start OvlFunc_943_2009db0
	push	{r5, lr}
	bl	__CutsceneStart
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =.L5208
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0xf8
	mov	r2, #0xb6
	lsl	r2, #18
	mov	r0, #0x15
	lsl	r1, #16
	bl	__MapActor_SetPos
	mov	r1, #0xf
	mov	r0, #0
	bl	__Func_8092950
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x42
	str	r2, [r3]
	bl	__MapTransitionIn
	mov	r0, #0x15
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r2, #0xad
	mov	r0, #0x15
	mov	r1, #0xf2
	lsl	r2, #2
	bl	__Func_80921c4
	mov	r0, #0x15
	mov	r1, #0xc4
	ldr	r2, =0x2a6
	bl	__Func_80921c4
	ldr	r2, =0x28e
	mov	r0, #0x15
	mov	r1, #0xb6
	bl	__Func_80921c4
	mov	r1, #2
	mov	r0, #0x15
	bl	__Func_80925cc
	ldr	r0, =0x1e44
	bl	__MessageID
	ldr	r0, =0xa015
	bl	OvlFunc_943_200b9ec
	mov	r0, #0
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	mov	r1, #0x9a
	ldr	r2, =0x261
	mov	r0, #0
	bl	__MapActor_TravelTo
	mov	r0, #0x92
	bl	__PlaySound
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x64
	strh	r5, [r0]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	add	r0, #0x64
	strh	r5, [r0]
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	mov	r1, #0x80
	add	r0, #0x64
	mov	r2, #0xf2
	strh	r5, [r0]
	lsl	r1, #14
	mov	r0, #0x18
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0xa8
	mov	r2, #0xf8
	mov	r0, #0x19
	lsl	r1, #15
	lsl	r2, #17
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r2, #0x95
	mov	r0, #0x1a
	lsl	r1, #13
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r0, #0x18
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	mov	r0, #0x19
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	bl	__MapActor_SetSpeed
	ldr	r2, =0x13333
	mov	r0, #0x1a
	ldr	r1, =0x26666
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_943__0200c4ec
	mov	r0, #0x18
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x19
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x1a
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0x18
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x19
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x1a
	mov	r1, #3
	bl	__Func_8092950
.L1ef4:
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x18
	bl	__MapActor_GetActor
	add	r0, #0x64
	mov	r2, #0
	ldrsh	r3, [r0, r2]
	cmp	r3, #0
	beq	.L1ef4
	bl	OvlFunc_943_2008bb8
	mov	r0, #0x15
	mov	r2, #0x99
	mov	r1, #0xc4
	lsl	r2, #2
	bl	__Func_809218c
	mov	r0, #0x18
	bl	__MapActor_WaitScript
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__Func_800c5b4
	mov	r0, #0x15
	bl	__Func_8093304
	ldr	r0, =0x1e45
	mov	r1, #1
	mov	r2, #0
	bl	__Func_8019aa0
	bl	__Func_800c5fc
	mov	r0, #0xc
	bl	__Func_8091e9c
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009db0

@ Cutscene: roughly 289 instructions of straight-line script --
@ 0 turns, 1 animation change, 0 dialogue lines, 8 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_943_2009f90
	push	{r5, r6, lr}
	bl	__CutsceneStart
	mov	r1, #0xf
	mov	r0, #0
	bl	__Func_8092950
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =.L5268
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0
	mov	r0, #0x1f
	bl	__MapActor_SetAnim
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1b
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1c
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0x1d
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r1, #0x80
	mov	r2, #0xa0
	lsl	r2, #18
	mov	r0, #0x16
	lsl	r1, #17
	bl	__MapActor_SetPos
	ldr	r5, =gScript_943__0200c80c
	mov	r0, #0x16
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, #0x86
	mov	r2, #0xad
	lsl	r2, #18
	mov	r0, #0x15
	lsl	r1, #17
	bl	__MapActor_SetPos
	mov	r1, r5
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r1, #0xf2
	mov	r2, #0x97
	mov	r0, #0x18
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x84
	mov	r2, #0x96
	mov	r0, #0x19
	lsl	r1, #17
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xfe
	mov	r2, #0xa7
	mov	r0, #0x1a
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x8d
	ldr	r2, =0x2920000
	lsl	r1, #17
	mov	r0, #0x1b
	bl	__MapActor_SetPos
	mov	r0, #0x18
	bl	__MapActor_GetActor
	mov	r6, #0
	add	r0, #0x63
	strb	r6, [r0]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r5, #1
	add	r0, #0x63
	strb	r5, [r0]
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	add	r0, #0x63
	strb	r6, [r0]
	mov	r0, #0x1b
	bl	__MapActor_GetActor
	add	r0, #0x63
	strb	r5, [r0]
	ldr	r5, =gScript_943__0200c7a8
	mov	r0, #0x18
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x19
	bl	__MapActor_SetBehavior
	ldr	r5, =gScript_943__0200c764
	mov	r0, #0x1a
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x1b
	bl	__MapActor_SetBehavior
	mov	r1, #0
	mov	r0, #0x14
	mov	r2, #0
	bl	__MapActor_SetPos
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x42
	str	r2, [r3]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0xc8
	lsl	r0, #1
	bl	__CutsceneWait
	mov	r1, #0xfe
	mov	r2, #0xb9
	mov	r0, #0x1c
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xc0
	mov	r0, #0x1d
	lsl	r1, #13
	ldr	r2, =0x24a0000
	bl	__MapActor_SetPos
	mov	r0, #0x1c
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r0, #0x1d
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r2, #0xa1
	mov	r0, #0x1d
	mov	r1, #0xac
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r2, #0xa5
	mov	r0, #0x1c
	mov	r1, #0xc8
	lsl	r2, #2
	bl	__Func_8092158
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #11
	lsl	r2, #10
	bl	__MapActor_SetSpeed
	mov	r2, #0x9b
	mov	r0, #0
	mov	r1, #0xae
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r2, #0x91
	lsl	r2, #2
	mov	r1, #0xb4
	mov	r0, #0x1c
	bl	__Func_8092158
	mov	r0, #0x92
	bl	__PlaySound
	ldr	r5, =gScript_943__0200c7ec
	mov	r0, #0x1c
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x1d
	bl	__MapActor_SetBehavior
	mov	r0, #0xf0
	bl	__PlaySound
	mov	r1, #0x86
	ldr	r2, =0x2520000
	mov	r0, #0x1f
	lsl	r1, #16
	bl	__MapActor_SetPos
	ldr	r1, =gScript_943__0200c814
	mov	r0, #0x1f
	bl	__MapActor_SetBehavior
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x86
	mov	r2, #0x92
	mov	r0, #0x1e
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #10
	mov	r0, #0x1e
	bl	__MapActor_SetSpeed
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #12
	mov	r2, #0x99
	str	r3, [r0, #0x28]
	lsl	r2, #2
	mov	r1, #0xba
	mov	r0, #0x1e
	bl	__Func_8092158
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x1e
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r2, #0x96
	lsl	r2, #2
	mov	r0, #0x1e
	mov	r1, #0xd8
	bl	__Func_8092158
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r0, #0x1e
	bl	OvlFunc_943_200ba00
	bl	OvlFunc_943_2008bb8
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r5, =gScript_943__0200c888
	mov	r0, #0x1e
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, r5
	mov	r0, #0x1c
	bl	__MapActor_SetBehavior
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, r5
	mov	r0, #0x1d
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x93
	bl	__PlaySound
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x18
	bl	__MapActor_SetIdle
	mov	r0, #0x19
	bl	__MapActor_SetIdle
	mov	r0, #0x1a
	bl	__MapActor_SetIdle
	mov	r0, #0x1b
	bl	__MapActor_SetIdle
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__Func_800c5b4
	mov	r0, #0x15
	bl	__Func_8093304
	ldr	r0, =0x1e45
	mov	r1, #1
	mov	r2, #0
	bl	__Func_8019aa0
	bl	__Func_800c5fc
	mov	r0, #0xd
	bl	__Func_8091e9c
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_2009f90

@ Cutscene: roughly 300 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_943_200a2c0
	push	{r5, r6, r7, lr}
	bl	__CutsceneStart
	mov	r1, #0xf
	mov	r0, #0
	bl	__Func_8092950
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	ldr	r0, =.L5160
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	ldr	r0, =.L5340
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0xb0
	mov	r2, #0xae
	lsl	r1, #16
	lsl	r2, #18
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r3, #0xd0
	lsl	r3, #8
	mov	r1, #0x84
	strh	r3, [r0, #6]
	lsl	r1, #17
	ldr	r2, =0x2960000
	mov	r0, #0x15
	bl	__MapActor_SetPos
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r1, #0xb8
	mov	r2, #0xa8
	strh	r3, [r0, #6]
	lsl	r1, #16
	mov	r0, #0x18
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xca
	mov	r2, #0xad
	mov	r0, #0x19
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xfc
	mov	r0, #0x1a
	lsl	r1, #16
	ldr	r2, =0x2860000
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0x1b
	lsl	r1, #17
	ldr	r2, =0x2ae0000
	bl	__MapActor_SetPos
	mov	r1, #0xac
	mov	r2, #0x9e
	mov	r0, #0x1c
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0x80
	lsl	r1, #17
	ldr	r2, =0x26e0000
	mov	r0, #0x1d
	bl	__MapActor_SetPos
	mov	r0, #0x18
	bl	__MapActor_GetActor
	ldr	r5, .L23b8	@ 0
	add	r0, #0x63
	strb	r5, [r0]
	mov	r0, #0x19
	bl	__MapActor_GetActor
	mov	r3, #1
	add	r0, #0x63
	strb	r3, [r0]
	mov	r0, #0x1a
	bl	__MapActor_GetActor
	add	r0, #0x63
	strb	r5, [r0]
	mov	r0, #0x1b
	bl	__MapActor_GetActor
	mov	r3, #2
	add	r0, #0x63
	strb	r3, [r0]
	mov	r2, #0
	mov	r0, #0x14
	mov	r1, #0
	bl	__MapActor_SetPos
	ldr	r5, =gScript_943__0200c8c4
	mov	r0, #0x18
	mov	r1, r5
	bl	__MapActor_SetBehavior
	b	.L23d8

	.align	2, 0
.L23b8:
	.word	0
	.pool

.L23d8:
	mov	r1, r5
	mov	r0, #0x19
	bl	__MapActor_SetBehavior
	ldr	r5, =gScript_943__0200c8b0
	mov	r0, #0x1a
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x1b
	bl	__MapActor_SetBehavior
	ldr	r5, =gScript_943__0200c8d8
	mov	r0, #0x1c
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0x1d
	bl	__MapActor_SetBehavior
	mov	r0, #0x18
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x19
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x1a
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x1b
	mov	r1, #3
	bl	__Func_8092950
	mov	r0, #0x1c
	mov	r1, #3
	bl	__Func_8092950
	mov	r1, #3
	mov	r0, #0x1d
	bl	__Func_8092950
	ldr	r3, =iwram_3001ebc
	mov	r2, #0xe0
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	add	r2, #0x42
	str	r2, [r3]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #0x93
	bl	__PlaySound
	mov	r0, #0x1f
	bl	__MapActor_GetActor
	ldr	r3, =0x1999
	mov	r6, r0
	str	r3, [r6, #0x18]
	str	r3, [r6, #0x1c]
	mov	r3, #0xc2
	lsl	r3, #16
	str	r3, [r6, #8]
	ldr	r3, =0x2820000
	ldr	r7, =0xf5c
	str	r3, [r6, #0x10]
	mov	r5, #0
.L2472:
	ldr	r3, [r6, #0x18]
	add	r3, r7
	str	r3, [r6, #0x18]
	ldr	r3, [r6, #0x1c]
	add	r3, r7
	str	r3, [r6, #0x1c]
	mov	r0, #1
	add	r5, #1
	bl	__WaitFrames
	cmp	r5, #0xf
	bls	.L2472
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	ldr	r3, =0x11999
	mov	r6, r0
	str	r3, [r6, #0x18]
	str	r3, [r6, #0x1c]
	mov	r3, #0xc2
	lsl	r3, #16
	str	r3, [r6, #8]
	mov	r3, #0xa0
	lsl	r3, #15
	str	r3, [r6, #0xc]
	ldr	r3, =0x2820000
	str	r3, [r6, #0x10]
	mov	r3, #0xa0
	lsl	r3, #7
	strh	r3, [r6, #6]
	ldr	r3, =0x6666
	str	r3, [r6, #0x44]
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r6, #0x48]
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r0, #0x93
	bl	__PlaySound
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x1f
	bl	__MapActor_SetPos
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	mov	r1, #1
	bl	__Actor_SetSpriteFlags
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	mov	r0, #0
	bl	__MapActor_SetSpeed
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r6, r0
	ldr	r5, .L252c	@ 0
	mov	r3, r6
	add	r3, #0x55
	mov	r2, #0x99
	strb	r5, [r3]
	mov	r0, #0
	mov	r1, #0xd8
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r0, #0x1e
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r2, #0x96
	mov	r0, #0x1e
	mov	r1, #0xc4
	lsl	r2, #2
	bl	__Func_8092158
	mov	r2, #0x96
	mov	r1, #0xd8
	lsl	r2, #2
	mov	r0, #0x1e
	bl	__Func_8092158
	mov	r0, #0x1c
	bl	__MapActor_SetIdle
	b	.L2558

	.align	2, 0
.L252c:
	.word	0
	.pool

.L2558:
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, =0xcccc
	mov	r0, #0x1c
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_943__0200c888
	mov	r0, #0x1c
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, #0xd0
	lsl	r1, #8
	mov	r0, #0x1e
	bl	OvlFunc_943_200ba00
	bl	OvlFunc_943_2008bb8
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, r5
	mov	r0, #0x1e
	bl	__MapActor_SetBehavior
	mov	r0, #0x1d
	bl	__MapActor_SetIdle
	mov	r0, #1
	bl	__WaitFrames
	ldr	r2, =0xcccc
	mov	r0, #0x1d
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r0, #0x1d
	bl	__MapActor_RunScript
	mov	r0, #0x14
	bl	__CutsceneWait
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	mov	r0, #0x18
	bl	__MapActor_SetIdle
	mov	r0, #0x19
	bl	__MapActor_SetIdle
	mov	r0, #0x1a
	bl	__MapActor_SetIdle
	mov	r0, #0x1b
	bl	__MapActor_SetIdle
	mov	r0, #0x1c
	bl	__MapActor_SetIdle
	mov	r0, #0x1d
	bl	__MapActor_SetIdle
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__Func_800c5b4
	mov	r0, #0x15
	bl	__Func_8093304
	ldr	r0, =0x1e45
	mov	r1, #1
	mov	r2, #0
	bl	__Func_8019aa0
	bl	__Func_800c5fc
	mov	r0, #0xe
	bl	__Func_8091e9c
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200a2c0

@ Cutscene: roughly 354 instructions of straight-line script --
@ 5 turns, 5 animation changes, 0 dialogue lines, 9 timed pauses.
@ Characterised structurally rather than beat by beat.
.thumb_func_start OvlFunc_943_200a618
	push	{r5, lr}
	bl	__CutsceneStart
	ldr	r0, =.L5418
	bl	__LoadFieldActors
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #1
	mov	r0, #0x19
	bl	__SetCameraTarget
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #5
	mov	r0, #0x15
	bl	__MapActor_SetAnim
	mov	r0, #0x15
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0
	bl	__MapActor_SetPos
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #7
	strh	r3, [r0, #6]
	ldr	r3, =iwram_3001ebc
	mov	r0, #0xe0
	ldr	r3, [r3]
	ldr	r2, =0x202
	lsl	r0, #1
	add	r3, r0
	str	r2, [r3]
	bl	__MapTransitionIn
	bl	__WaitMapTransition
	mov	r0, #0x14
	bl	__CutsceneWait
	bl	OvlFunc_943_2008bb8
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0xd8
	mov	r0, #0
	lsl	r1, #16
	ldr	r2, =0x24a0000
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	ldr	r2, =0x256
	mov	r1, #0xd8
	mov	r0, #0
	bl	__Func_80921c4
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0xc0
	lsl	r1, #7
	mov	r0, #0
	bl	OvlFunc_943_200ba00
	mov	r0, #0
	mov	r1, #2
	mov	r2, #0xa
	bl	__MapActor_Jump
	mov	r0, #0
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r2, #0x9c
	mov	r1, #0xc2
	lsl	r2, #2
	mov	r0, #0
	bl	__Func_80921c4
	mov	r0, #0xb5
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #9
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #1
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0xc0
	lsl	r1, #8
	mov	r2, #0x14
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x19
	bl	__MapActor_GetActor
	ldr	r5, .L275c	@ 0
	add	r0, #0x55
	mov	r1, #0x80
	mov	r2, #0x80
	strb	r5, [r0]
	lsl	r1, #10
	mov	r0, #0x19
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r2, #0x99
	lsl	r2, #2
	mov	r1, #0xd8
	mov	r0, #0x19
	bl	__MapActor_TravelTo
	mov	r0, #0x95
	bl	__PlaySound
	mov	r0, #0x16
	mov	r1, #2
	bl	__Func_8092b08
	mov	r1, #5
	mov	r0, #0x16
	bl	__MapActor_SetAnim
	mov	r0, #0x16
	bl	__MapActor_GetActor
	b	.L2780

	.align	2, 0
.L275c:
	.word	0
	.pool

.L2780:
	mov	r3, #0x80
	lsl	r3, #12
	str	r3, [r0, #0x28]
	ldr	r3, =0xb333
	str	r3, [r0, #0x48]
	mov	r3, #0xd0
	lsl	r3, #9
	str	r3, [r0, #0x18]
	str	r3, [r0, #0x1c]
	mov	r5, #0x80
	ldr	r3, =OvlFunc_943_20088c0
	lsl	r5, #8
	mov	r1, #0xc0
	mov	r2, #0xc0
	str	r3, [r0, #0x6c]
	str	r5, [r0, #0x44]
	lsl	r1, #11
	mov	r0, #0x16
	lsl	r2, #10
	bl	__MapActor_SetSpeed
	ldr	r2, =0x26a
	mov	r1, #0xb6
	mov	r0, #0x16
	bl	__Func_8092158
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r1, #0xa0
	lsl	r1, #8
	mov	r0, #0
	bl	OvlFunc_943_200ba00
	mov	r0, #0
	mov	r1, #6
	mov	r2, #0x50
	bl	__MapActor_Jump
	mov	r2, #0x8d
	mov	r0, #0x19
	mov	r1, #0xe8
	lsl	r2, #2
	bl	__MapActor_TravelTo
	mov	r0, #0
	mov	r1, #0xcc
	ldr	r2, =0x262
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #0xd0
	ldr	r2, =0x256
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #0xf8
	ldr	r2, =0x256
	bl	__Func_80921c4
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2812
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__MapActor_SetPos
.L2812:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2826
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #2
	bl	__MapActor_SetPos
.L2826:
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L283a
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #3
	bl	__MapActor_SetPos
.L283a:
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, #1
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, #2
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, #3
	lsl	r1, #9
	bl	__MapActor_SetSpeed
	mov	r2, #0x92
	mov	r0, #0
	mov	r1, #0xfa
	lsl	r2, #2
	bl	__Func_809218c
	mov	r2, #0x96
	mov	r0, #1
	mov	r1, #0xf0
	lsl	r2, #2
	bl	__Func_809218c
	mov	r2, #0x96
	mov	r0, #2
	mov	r1, #0xfe
	lsl	r2, #2
	bl	__Func_809218c
	mov	r2, #0x9a
	lsl	r2, #2
	mov	r0, #3
	mov	r1, #0xf8
	bl	__Func_80921c4
	mov	r0, #0
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #1
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #2
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #2
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x14
	lsl	r1, #8
	mov	r0, #3
	bl	__Func_8092adc
	mov	r0, #0x95
	bl	__PlaySound
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0x81
	mov	r0, #1
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0x81
	mov	r0, #2
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #3
	bl	__MapActor_Surprise
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r0, #0
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #1
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #2
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #3
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r2, #0x8d
	mov	r0, #0
	mov	r1, #0xf8
	lsl	r2, #2
	bl	__Func_809218c
	mov	r2, #0x8d
	mov	r1, #0xf8
	lsl	r2, #2
	mov	r0, #1
	bl	__Func_809218c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x8d
	mov	r0, #2
	mov	r1, #0xf8
	lsl	r2, #2
	bl	__Func_809218c
	mov	r2, #0x8d
	mov	r1, #0xf8
	lsl	r2, #2
	mov	r0, #3
	bl	__Func_809218c
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r2, =gState
	mov	r0, #0xe2
	ldr	r1, =0x6f
	lsl	r0, #1
	add	r3, r2, r0
	strh	r1, [r3]
	mov	r3, #0xe3
	lsl	r3, #1
	add	r1, r2, r3
	mov	r3, #0x1e
	strh	r3, [r1]
	add	r0, #0x67
	add	r2, r0
	mov	r3, #3
	strb	r3, [r2]
	ldr	r0, =0x6d
	mov	r1, #0x10
	bl	__Func_8091f90
	mov	r0, #0x3e
	mov	r1, #3
	bl	__Func_8091eb0
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_943_200a618
