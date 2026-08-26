	.include "macros.inc"


@ Cutscene: roughly 129 instructions of straight-line script --
@ 1 turn, 4 animation changes, 5 dialogue lines, 3 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe74.
@ Reads save bit 0x837.
@ Sets save bit 0x837.
.thumb_func_start OvlFunc_882_2009828
	push	{r5, r6, lr}
	ldr	r0, =0x837
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1836
	b	.L196c
.L1836:
	bl	__CutsceneStart
	mov	r1, #0x80
	lsl	r1, #1
	mov	r0, #0x16
	bl	__MapActor_Surprise
	ldr	r5, =0xe74
	mov	r0, r5
	bl	__MessageID
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0
	lsl	r1, #7
	bl	__Func_8092adc
	ldr	r0, =0x6666
	ldr	r1, =0xccc
	bl	__Func_80933d4
	mov	r0, #0x80
	mov	r1, #1
	mov	r2, #0x93
	mov	r3, #1
	lsl	r0, #17
	neg	r1, r1
	lsl	r2, #18
	bl	__Func_80933f8
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	mov	r0, #0x16
	lsl	r1, #10
	bl	__MapActor_SetSpeed
	ldr	r1, =gScript_882__0200c934
	mov	r0, #0x16
	bl	__MapActor_RunScript
	mov	r2, #0
	mov	r1, #0x16
	mov	r0, #0
	bl	__Func_8092848
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldr	r1, =gScript_882__0200c984
	mov	r0, #0x16
	bl	__MapActor_SetBehavior
	mov	r1, #0
	mov	r0, #0x16
	bl	__ActorMessage
	mov	r0, #0x16
	bl	__MapActor_GetActor
	mov	r6, #0x80
	lsl	r6, #9
	mov	r1, #1
	str	r6, [r0, #0x1c]
	mov	r0, #0x16
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #0x16
	bl	__Func_8093054
	mov	r0, #0x28
	bl	__CutsceneWait
	add	r5, #5
	mov	r1, #1
	mov	r0, #0x16
	bl	__Func_80925cc
	mov	r0, r5
	bl	__MessageID
	mov	r2, #0x14
	mov	r0, #0x16
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x16
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r2, #0x80
	mov	r0, #0x16
	mov	r1, r6
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0x16
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1942
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x16
	bl	__MapActor_TravelTo
.L1942:
	mov	r0, #0x16
	bl	__MapActor_WaitMovement
	mov	r2, #0
	mov	r0, #0x16
	mov	r1, #0
	bl	__MapActor_SetPos
	mov	r0, #1
	mov	r1, #1
	bl	__Func_80917d0
	mov	r0, #0x15
	mov	r1, #3
	bl	__MapActor_SetAnim
	ldr	r0, =0x837
	bl	__SetFlag
	bl	__CutsceneEnd
.L196c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2009828

@ Cutscene: roughly 77 instructions of straight-line script --
@ 1 turn, 1 animation change, 2 dialogue lines, 3 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe7b.
.thumb_func_start OvlFunc_882_200998c
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L19a6
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x16
	bl	__MapActor_SetPos
.L19a6:
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x16
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0x16
	ldr	r1, =0x119
	ldr	r2, =0x1fb
	bl	__Func_80921c4
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x16
	bl	__Func_8092848
	mov	r0, #0x1e
	bl	__CutsceneWait
	ldr	r0, =0xe7b
	bl	__MessageID
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r2, #0
	mov	r1, #0x16
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #0
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x16
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x16
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1a2e
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x16
	bl	__MapActor_TravelTo
.L1a2e:
	mov	r0, #0x16
	bl	__MapActor_WaitMovement
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	ldr	r2, =0x205
	bl	__Func_80921c4
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_200998c

@ Cutscene: roughly 69 instructions of straight-line script --
@ 0 turns, 2 animation changes, 2 dialogue lines, 2 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe7d.
.thumb_func_start OvlFunc_882_2009a64
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r0, #0
	mov	r6, r1
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1a7e
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x16
	bl	__MapActor_SetPos
.L1a7e:
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x16
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r0, #0x16
	mov	r1, r5
	mov	r2, r6
	bl	__Func_80921c4
	mov	r2, #0
	mov	r1, #0x16
	mov	r0, #0
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0
	bl	__MapActor_Surprise
	mov	r0, #0x28
	bl	__CutsceneWait
	ldr	r0, =0xe7d
	bl	__MessageID
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0x16
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x16
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1afc
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x16
	bl	__MapActor_TravelTo
.L1afc:
	mov	r0, #0x16
	bl	__MapActor_WaitMovement
	mov	r0, #0x16
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2009a64

@ Cutscene: roughly 525 instructions of straight-line script --
@ 6 turns, 14 animation changes, 6 dialogue lines, 13 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe7f.
@ Reads save bit 0x838.
@ Sets save bit 0x838.
.thumb_func_start OvlFunc_882_2009b18
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r0, =0x838
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1b32
	b	.L207e
.L1b32:
	bl	__CutsceneStart
	ldr	r0, =.L54b0
	bl	__LoadFieldActors
	bl	OvlFunc_882_200c550
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x8d
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0xc0
	mov	r1, #0xc0
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1b92
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #0x16
	bl	__MapActor_SetPos
.L1b92:
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0
	lsl	r1, #10
	lsl	r2, #9
	bl	__MapActor_SetSpeed
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	mov	r0, #0x16
	lsl	r1, #10
	bl	__MapActor_SetSpeed
	ldr	r1, =gScript_882__0200ca00
	mov	r0, #0
	bl	__MapActor_SetBehavior
	ldr	r1, =gScript_882__0200ca3c
	mov	r0, #0x16
	bl	__MapActor_RunScript
	mov	r0, #0
	bl	__MapActor_WaitScript
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x16
	lsl	r1, #1
	mov	r2, #0x1e
	bl	__MapActor_Emote
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #9
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xa0
	mov	r1, #0xa0
	mov	r2, #0x80
	lsl	r2, #9
	lsl	r1, #11
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r0, #0
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x16
	bl	__MapActor_Surprise
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0x20
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0x21
	mov	r1, #5
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	mov	r1, #8
	bl	__MapActor_SetAnim
	mov	r1, #8
	mov	r0, #0x1d
	bl	__MapActor_SetAnim
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	ldr	r3, =0xffff0000
	mov	r1, #2
	str	r3, [r0, #0x18]
	mov	r0, #0x20
	bl	__Func_8092b08
	mov	r0, #0x21
	mov	r1, #2
	bl	__Func_8092b08
	mov	r0, #0x1e
	mov	r1, #3
	bl	__Func_8092b08
	mov	r1, #3
	mov	r0, #0x1d
	bl	__Func_8092b08
	ldr	r0, =0xe7f
	bl	__MessageID
	mov	r0, #0x1c
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x14
	mov	r0, #0x16
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #11
	lsl	r1, #8
	bl	__Func_80933d4
	mov	r0, #0xe0
	mov	r1, #1
	lsl	r0, #15
	neg	r1, r1
	ldr	r2, =0x14b0000
	mov	r3, #1
	bl	__Func_80933f8
	bl	__Func_8093530
	mov	r5, #0
.L1cc2:
	mov	r0, #0x20
	bl	__MapActor_GetActor
	bl	OvlFunc_882_200c2bc
	mov	r0, #0x21
	bl	__MapActor_GetActor
	bl	OvlFunc_882_200c2bc
	mov	r0, #0x1e
	bl	__MapActor_GetActor
	bl	OvlFunc_882_200c2bc
	mov	r0, #0x1d
	bl	__MapActor_GetActor
	bl	OvlFunc_882_200c2bc
	add	r5, #1
	mov	r0, #1
	bl	__WaitFrames
	cmp	r5, #0x27
	bls	.L1cc2
	ldr	r5, =.L57fc
	ldr	r2, =.L57f8
	ldr	r7, =OvlFunc_882_200c56c
	mov	r6, #0
	mov	r1, #0xc8
	str	r6, [r2]
	lsl	r1, #4
	str	r6, [r5]
	mov	r0, r7
	mov	r9, r2
	bl	__StartTask
	ldr	r3, =OvlFunc_882_200c5a8
	mov	r1, #0xc8
	mov	r11, r3
	lsl	r1, #4
	mov	r0, r11
	bl	__StartTask
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r2, #1
	str	r2, [r5]
	mov	r0, #0x1e
	mov	r10, r2
	bl	__CutsceneWait
	mov	r1, #0xe4
	mov	r2, #0x91
	lsl	r1, #15
	lsl	r2, #17
	mov	r0, #0x13
	bl	__MapActor_SetPos
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldr	r3, [r0, #0xc]
	lsl	r2, #15
	add	r3, r2
	str	r3, [r0, #0xc]
	str	r3, [r0, #0x3c]
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	mov	r0, #0x13
	bl	__MapActor_SetSpeed
	mov	r0, #0x91
	bl	__PlaySound
	ldr	r2, =0x14d
	mov	r0, #0x13
	mov	r1, #0x72
	bl	__Func_8092158
	mov	r0, #0x13
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, #0x13
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	str	r6, [r5]
	bl	__MapActor_SetSpeed
	mov	r2, #0x96
	lsl	r2, #1
	mov	r0, #0x13
	mov	r1, #0x72
	bl	__Func_8092158
	mov	r0, #0x13
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0xa0
	mov	r1, #0xa0
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #9
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r3, #2
	str	r3, [r5]
	mov	r0, #0x13
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	mov	r8, r3
	bl	__MapActor_SetSpeed
	ldr	r2, =0x14d
	mov	r0, #0x13
	mov	r1, #0x72
	bl	__Func_8092158
	mov	r0, #0x13
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #10
	lsl	r2, #9
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r0, #0x13
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	str	r6, [r5]
	bl	__MapActor_SetSpeed
	mov	r2, #0x96
	lsl	r2, #1
	mov	r0, #0x13
	mov	r1, #0x72
	bl	__Func_8092158
	mov	r0, #0x13
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r1, #11
	lsl	r2, #9
	lsl	r0, #11
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r2, r8
	str	r2, [r5]
	mov	r0, #0x13
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	ldr	r2, =0x14d
	mov	r0, #0x13
	mov	r1, #0x72
	bl	__Func_8092158
	mov	r0, #0x13
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	lsl	r1, #10
	lsl	r0, #10
	bl	__Func_8012330
	mov	r0, #0x91
	bl	__PlaySound
	mov	r3, r10
	str	r3, [r5]
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	mov	r0, #0x20
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r0, #0x20
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1f
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x21
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0x21
	mov	r1, #2
	bl	__Func_80925cc
	mov	r2, #0x28
	mov	r0, #0x1c
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #0x81
	mov	r0, #0x1e
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r0, #0x1e
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x1e
	mov	r1, #0
	bl	__ActorMessage
	mov	r3, r9
	mov	r2, r10
	str	r2, [r3]
	mov	r1, #1
	mov	r0, #0x1d
	bl	__MapActor_SetAnim
	mov	r0, #1
	bl	__WaitFrames
	mov	r0, #0x1d
	mov	r1, #0
	bl	__Func_8092950
	mov	r0, #0x1d
	ldr	r1, =0x105
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r1, #0x80
	mov	r0, #0x1d
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r0, #0x1d
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x1d
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0x1d
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0
	mov	r0, #0x1d
	b	.L1f58

	.pool_aligned

.L1f58:
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #0x1d
	mov	r1, #2
	bl	__Func_80925cc
	mov	r2, #0x28
	mov	r0, #0x1d
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r1, #9
	mov	r0, #0x1d
	bl	__MapActor_SetAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0
	mov	r2, #0x14
	mov	r0, #0x1d
	bl	__Func_8093040
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #1
	ldr	r2, =0xe666
	neg	r0, r0
	neg	r1, r1
	bl	__Func_8012330
	mov	r0, #0xc0
	mov	r1, #0xc0
	lsl	r0, #11
	lsl	r1, #8
	bl	__Func_80933d4
	mov	r0, #0xa8
	mov	r1, #1
	mov	r2, #0x8d
	mov	r3, #1
	lsl	r0, #15
	neg	r1, r1
	lsl	r2, #18
	bl	__Func_80933f8
	bl	__Func_8093530
	bl	__Func_809202c
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0x16
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0x16
	bl	__MapActor_Surprise
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r0, r7
	bl	__StopTask
	mov	r0, r11
	bl	__StopTask
	mov	r0, #0x16
	mov	r1, #0
	bl	__ActorMessage
	mov	r2, #0
	mov	r1, #0x16
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	bl	OvlFunc_882_200c560
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #0x16
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x16
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2040
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0x16
	bl	__MapActor_TravelTo
.L2040:
	mov	r0, #0x16
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0x16
	bl	__MapActor_SetPos
	mov	r0, #0x1f
	bl	__DeleteFieldActor
	mov	r0, #0x1c
	bl	__DeleteFieldActor
	mov	r0, #0x1e
	bl	__DeleteFieldActor
	mov	r0, #0x1d
	bl	__DeleteFieldActor
	mov	r0, #0x20
	bl	__DeleteFieldActor
	mov	r0, #0x21
	bl	__DeleteFieldActor
	ldr	r0, =0x838
	bl	__SetFlag
	bl	__CutsceneEnd
.L207e:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2009b18

@ Leaf helper, 44 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e40
@ Writes offsets +0x5.
.thumb_func_start OvlFunc_882_200a09c
	push	{lr}
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, r1
	mov	r1, #0xf
	and	r1, r3
	cmp	r1, #1
	bne	.L20f0
	ldr	r0, [r0, #0x50]
	sub	r4, r2, #1
	mov	r12, r0
	cmp	r2, #0
	bne	.L20c4
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	ldr	r2, =.L48bc
	lsr	r3, #1
	and	r3, r1
	ldrb	r4, [r2, r3]
.L20c4:
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L20e8
	mov	r0, r12
	add	r0, #0x28
	mov	r1, r3
.L20d4:
	ldmia	r0!, {r2}
	cmp	r2, #0
	beq	.L20e2
	ldr	r3, [r2, #0x10]
	cmp	r3, #0
	beq	.L20e2
	strb	r4, [r2, #5]
.L20e2:
	sub	r1, #1
	cmp	r1, #0
	bne	.L20d4
.L20e8:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.L20f0:
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_200a09c
