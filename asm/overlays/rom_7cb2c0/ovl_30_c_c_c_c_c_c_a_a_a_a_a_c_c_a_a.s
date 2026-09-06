	.include "macros.inc"

@ Cutscene: roughly 377 instructions of straight-line script --
@ 11 turns, 7 animation changes, 4 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1d56.
@ Reads save bit 0x911.
@ Sets save bit 0x922.
.thumb_func_start OvlFunc_945_2009b34
	push	{r5, r6, lr}
	mov	r6, r11
	mov	r5, r10
	push	{r5, r6}
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6}
	ldr	r0, =0x911
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1b4e
	b	.L1eea
.L1b4e:
	bl	__CutsceneStart
	bl	__Func_808e118
	ldr	r0, =0x26666
	ldr	r1, =0x4ccc
	bl	__Func_80933d4
	mov	r1, #1
	mov	r2, #0xe8
	lsl	r2, #17
	ldr	r3, =0x10000014
	ldr	r0, =0x5b70000
	neg	r1, r1
	bl	OvlFunc_945_200c8ac
	mov	r1, #1
	mov	r0, #0xd
	bl	__Func_80925cc
	ldr	r0, =0x1d56
	bl	__MessageID
	ldr	r3, =0x200d
	mov	r8, r3
	mov	r0, r8
	bl	OvlFunc_945_200c86c
	mov	r3, #0xd0
	lsl	r3, #8
	mov	r10, r3
	mov	r0, #0xc
	mov	r1, r10
	bl	OvlFunc_945_200c880
	mov	r1, #0x81
	ldr	r6, =0x800c
	mov	r2, #0x14
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_809259c
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r0, #0xe
	mov	r1, #1
	bl	__Func_80925cc
	mov	r2, #0x14
	ldr	r0, =0xa00e
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0xc
	mov	r1, #0
	bl	OvlFunc_945_200c880
	mov	r0, #0xc
	ldr	r1, =0x101
	mov	r2, #0x28
	ldr	r5, =0xa00e
	bl	__MapActor_Emote
	mov	r2, #0x28
	mov	r0, #0xe
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #3
	mov	r0, #0xe
	bl	__Func_809259c
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0xc
	bl	__MapActor_Surprise
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0xc
	bl	__Func_809259c
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, #1
	mov	r0, #0xe
	bl	__Func_80925cc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r9, r3
	mov	r1, r9
	mov	r0, #0xe
	bl	OvlFunc_945_200c880
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, r10
	bl	OvlFunc_945_200c880
	mov	r1, #0x80
	mov	r2, #0x1e
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #0xc
	bl	__Func_809259c
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, #4
	mov	r0, #0xd
	bl	__MapActor_DoAnim
	mov	r0, r8
	bl	OvlFunc_945_200c86c
	mov	r1, #2
	mov	r0, #0xd
	bl	__Func_809259c
	mov	r0, r8
	bl	OvlFunc_945_200c86c
	mov	r1, #4
	mov	r0, #0xc
	bl	__MapActor_SetAnim
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, #4
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r3, #0x80
	lsl	r3, #8
	mov	r11, r3
	mov	r0, #0xe
	mov	r1, r11
	bl	OvlFunc_945_200c880
	mov	r0, #0xe
	mov	r1, #2
	bl	__Func_809259c
	mov	r0, r5
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r0, #0xc
	lsl	r1, #1
	mov	r2, #0x50
	bl	__MapActor_Emote
	mov	r0, r6
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #0xe
	ldr	r1, =0x103
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0x3c
	mov	r0, #0xd
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xe
	bl	__Func_809259c
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xe
	mov	r1, r9
	bl	OvlFunc_945_200c880
	mov	r1, #1
	mov	r0, #0xe
	bl	__Func_80925cc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r3, #0xc0
	lsl	r3, #6
	mov	r8, r3
	mov	r0, #0xd
	mov	r1, r8
	bl	OvlFunc_945_200c880
	mov	r0, #0xd
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0x3c
	mov	r0, #0xc
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #0xd
	bl	__Func_80925cc
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r2, #0x28
	mov	r0, #0xe
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #0xe
	bl	__Func_80925cc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, r10
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #0xd
	lsl	r1, #7
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r2, #0
	mov	r0, #0xc
	mov	r1, #0
	bl	__Func_8092adc
	mov	r0, #0xd
	mov	r1, r8
	bl	OvlFunc_945_200c880
	mov	r0, #0xc
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, r6
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r1, #0x80
	mov	r2, #0x28
	lsl	r1, #7
	mov	r0, #0xe
	bl	__Func_8092adc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, #2
	bl	__Func_809259c
	mov	r1, #2
	mov	r0, #0xd
	bl	__Func_80925cc
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #1
	mov	r0, #0xd
	bl	__Func_80925cc
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #0xe
	bl	__MapActor_DoAnim
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	mov	r2, #0x28
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_80925cc
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #0xd
	bl	__MapActor_DoAnim
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r2, #0x28
	mov	r0, #0xe
	mov	r1, r9
	bl	__Func_8092adc
	mov	r0, #0xe
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r0, #0xd
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xe
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	ldr	r2, =0xcccc
	mov	r0, #0xd
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	ldr	r5, =gScript_945__0200e6a8
	mov	r0, #0xe
	mov	r1, r5
	bl	__MapActor_SetBehavior
	mov	r1, r5
	mov	r0, #0xd
	bl	__MapActor_SetBehavior
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #0xc
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	ldr	r1, =0x26666
	ldr	r2, =0x13333
	mov	r0, #0
	bl	__MapActor_SetSpeed
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #0xfe
	and	r3, r2
	mov	r2, #0x82
	strb	r3, [r0]
	mov	r1, #0xb8
	lsl	r2, #2
	mov	r0, #0
	bl	__Func_80921c4
	mov	r0, #1
	bl	__CutsceneWait
	mov	r0, #0
	bl	__MapActor_GetActor
	add	r0, #0x5a
	ldrb	r2, [r0]
	mov	r3, #1
	orr	r3, r2
	strb	r3, [r0]
	mov	r1, r11
	mov	r0, #0
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xc
	mov	r1, #4
	mov	r2, #0x14
	bl	__MapActor_Jump
	mov	r1, #0xa0
	mov	r2, #0x14
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_809259c
	mov	r0, #0xc
	bl	OvlFunc_945_200c86c
	ldr	r2, =0xcccc
	mov	r0, #0xc
	ldr	r1, =0x19999
	bl	__MapActor_SetSpeed
	mov	r1, r5
	mov	r0, #0xc
	bl	__MapActor_SetBehavior
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #0x80
	lsl	r1, #7
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0xc
	bl	__MapActor_WaitScript
	ldr	r0, =0x922
	bl	__SetFlag
	bl	__CutsceneEnd
.L1eea:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r3}
	mov	r11, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2009b34

@ Cutscene: roughly 807 instructions of straight-line script --
@ 40 turns, 20 animation changes, 6 dialogue lines, 6 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1d93.
@ Sets save bit 0x921.
.thumb_func_start OvlFunc_945_2009f3c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r0, #0x1c
	bl	__PlaySound
	ldr	r0, =0x26666
	ldr	r1, =0x4ccc
	bl	__Func_80933d4
	mov	r0, #0xe4
	mov	r2, #0xa2
	mov	r1, #1
	ldr	r3, =0x10000014
	lsl	r2, #18
	lsl	r0, #17
	neg	r1, r1
	bl	OvlFunc_945_200c8ac
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	ldr	r0, =0x1d93
	bl	__MessageID
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, #0xd0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r0, #0xa
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #0xc
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0xd
	lsl	r1, #8
	mov	r2, #0x28
	bl	__Func_8092adc
	mov	r2, #0x28
	mov	r0, #9
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #9
	bl	__Func_809259c
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r0, #0xb
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xd0
	mov	r2, #0x14
	mov	r0, #0xd
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	ldr	r3, =0x100b
	mov	r9, r3
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	mov	r2, #0x14
	mov	r0, #0xd
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xd
	bl	__Func_809259c
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	ldr	r1, =0x105
	mov	r2, #0x3c
	mov	r0, #9
	bl	__MapActor_Emote
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, #0x82
	mov	r2, #0x14
	lsl	r1, #1
	mov	r0, #0xc
	bl	__MapActor_Emote
	ldr	r0, =0x900c
	bl	OvlFunc_945_200c86c
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r7, #0xc0
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	lsl	r7, #6
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, r7
	mov	r0, #0xc
	bl	OvlFunc_945_200c880
	ldr	r0, =0x900c
	bl	OvlFunc_945_200c86c
	mov	r3, #0xb0
	lsl	r3, #8
	mov	r11, r3
	mov	r1, r11
	mov	r0, #0xb
	bl	OvlFunc_945_200c880
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #0xd
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #0xd
	bl	__MapActor_DoAnim
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, #0x80
	mov	r0, #0xd
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r2, #0
	mov	r0, #0xc
	lsl	r1, #7
	bl	__Func_8092adc
	mov	r3, #0xa0
	lsl	r3, #7
	mov	r10, r3
	mov	r1, r10
	mov	r0, #0xb
	bl	OvlFunc_945_200c880
	mov	r0, #0xd
	ldr	r1, =0x6666
	ldr	r2, =0x3333
	bl	__MapActor_SetSpeed
	mov	r0, #0xc
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r1, #0xde
	mov	r2, #0xa7
	mov	r0, #0xc
	lsl	r1, #1
	lsl	r2, #2
	bl	__Func_809218c
	mov	r1, #0xec
	mov	r2, #0xa7
	lsl	r2, #2
	lsl	r1, #1
	mov	r0, #0xd
	bl	__Func_80921c4
	mov	r0, #0xc
	bl	__MapActor_WaitMovement
	mov	r1, #1
	mov	r0, #0xc
	bl	__MapActor_SetAnim
	mov	r0, #0x50
	bl	__CutsceneWait
	mov	r3, #0xd0
	lsl	r3, #8
	mov	r8, r3
	mov	r1, r8
	mov	r0, #0xc
	bl	OvlFunc_945_200c880
	mov	r2, #0x3c
	mov	r0, #0xc
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x28
	ldr	r0, =0x400b
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_80925cc
	mov	r1, r8
	mov	r2, #0
	mov	r0, #0xb
	bl	__Func_8092adc
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r1, r8
	mov	r0, #0xc
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x3c
	mov	r0, #9
	ldr	r1, =0x101
	bl	__MapActor_Emote
	mov	r1, #4
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, r8
	mov	r2, #0
	mov	r0, #0xd
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r0, #0xd
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r2, #0x14
	mov	r1, #2
	mov	r0, #0xd
	bl	__MapActor_Jump
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_DoAnim
	mov	r0, #9
	ldr	r6, =0x100c
	bl	OvlFunc_945_200c86c
	mov	r1, r8
	mov	r0, #0xb
	bl	OvlFunc_945_200c880
	mov	r1, #1
	mov	r0, #0xc
	bl	__Func_80925cc
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r2, #0x28
	mov	r0, #8
	ldr	r1, =0x105
	bl	__MapActor_Emote
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	mov	r0, #0xd
	lsl	r1, #1
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r2, #0
	mov	r1, #4
	mov	r0, #0xd
	bl	__MapActor_Jump
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	mov	r2, #0x28
	mov	r0, #8
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r0, #8
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #0xb
	mov	r1, #2
	bl	__Func_809259c
	mov	r0, r9
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8093040
	mov	r1, #0x80
	mov	r0, #9
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r1, r10
	mov	r2, #0x14
	mov	r0, #9
	bl	__Func_8092adc
	mov	r0, #9
	mov	r1, #2
	bl	__Func_809259c
	mov	r2, #0x14
	mov	r0, #9
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #3
	mov	r0, #0xb
	bl	__MapActor_SetAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_809259c
	ldr	r5, =0x400b
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r2, #0x14
	mov	r1, r10
	mov	r0, #0xb
	bl	__Func_8092adc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_809259c
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r0, #0xb
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	mov	r2, #0x3c
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #1
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r0, #0xb
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r2, #0x14
	mov	r1, r8
	mov	r0, #0xb
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #9
	bl	__MapActor_SetAnim
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r2, #0x14
	mov	r0, #0xb
	ldr	r1, =0x103
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xb
	bl	__Func_809259c
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r1, #0x84
	mov	r2, #0x28
	lsl	r1, #1
	mov	r0, #9
	bl	__MapActor_Emote
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r0, #8
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_SetAnim
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r2, #0x28
	mov	r1, r8
	b	.L2384

	.pool_aligned

.L2384:
	mov	r0, #9
	bl	__Func_8092adc
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, #1
	mov	r0, #0xc
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, r10
	mov	r0, #0xb
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r10
	mov	r0, #9
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0xd
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x28
	mov	r1, r11
	mov	r0, #0xa
	bl	__Func_8092adc
	mov	r1, #1
	mov	r0, #0xb
	bl	__Func_80925cc
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r2, #0x14
	mov	r0, r6
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #2
	mov	r0, #9
	bl	__Func_80925cc
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r1, #0x84
	mov	r2, #0x28
	mov	r0, #0xc
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #3
	mov	r0, #0xc
	bl	__MapActor_SetAnim
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, #0x80
	mov	r2, #0x14
	lsl	r1, #8
	mov	r0, #8
	bl	__Func_8092adc
	mov	r0, #0x13
	bl	__PlaySound
	mov	r0, #8
	mov	r1, #2
	bl	__Func_809259c
	ldr	r5, =0x8008
	mov	r1, #0x80
	lsl	r1, #1
	mov	r2, #0x50
	mov	r0, #8
	bl	__MapActor_Emote
	mov	r0, r5
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0xb
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0xd
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0xa
	ldr	r1, =0x101
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r0, #0
	ldr	r1, =0x101
	mov	r2, #0x28
	bl	__MapActor_Emote
	mov	r1, r8
	mov	r0, #0xc
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r8
	mov	r0, #0xb
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r11
	mov	r0, #0xd
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r11
	mov	r0, #0xa
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r2, #0x28
	mov	r0, #0
	lsl	r1, #8
	bl	__Func_8092adc
	mov	r1, #0x81
	mov	r0, #8
	lsl	r1, #1
	bl	__MapActor_Surprise
	mov	r2, #0x28
	mov	r0, #8
	mov	r1, #4
	bl	__MapActor_Jump
	mov	r0, #8
	mov	r1, #2
	bl	__Func_809259c
	mov	r0, r5
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #8
	ldr	r1, =0x19999
	ldr	r2, =0xcccc
	bl	__MapActor_SetSpeed
	mov	r0, #8
	ldr	r1, =0x1db
	ldr	r2, =0x256
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r0, #8
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #9
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xe7
	ldr	r2, =0x26a
	mov	r0, #9
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r1, r11
	mov	r0, #9
	bl	OvlFunc_945_200c880
	mov	r1, #0x80
	mov	r2, #0x28
	mov	r0, #9
	lsl	r1, #1
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #9
	bl	__Func_809259c
	ldr	r0, =0x8009
	bl	OvlFunc_945_200c86c
	ldr	r1, =0x101
	mov	r2, #0x3c
	mov	r0, #0xb
	bl	__MapActor_Emote
	mov	r0, #0xb
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r2, #0x14
	mov	r0, #0xc
	bl	__MapActor_Emote
	mov	r0, r6
	bl	OvlFunc_945_200c86c
	mov	r0, #8
	ldr	r1, =0x103
	mov	r2, #0x14
	bl	__MapActor_Emote
	mov	r0, #8
	mov	r1, #4
	mov	r2, #0
	bl	__MapActor_Jump
	mov	r2, #0x14
	mov	r1, r10
	mov	r0, #8
	bl	__Func_8092adc
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r0, #0x1c
	bl	__PlaySound
	mov	r1, #3
	mov	r0, #8
	bl	__Func_809259c
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r2, #0x3c
	ldr	r1, =0x101
	mov	r0, #0xd
	bl	__MapActor_Emote
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, r7
	mov	r0, #8
	bl	OvlFunc_945_200c880
	mov	r1, #4
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, #0xde
	mov	r2, #0x9d
	lsl	r2, #2
	mov	r0, #0xc
	lsl	r1, #1
	bl	__Func_80921c4
	mov	r1, r8
	mov	r0, #0xc
	bl	OvlFunc_945_200c880
	ldr	r0, =0x900c
	bl	OvlFunc_945_200c86c
	mov	r2, #0x14
	mov	r1, r10
	mov	r0, #8
	bl	__Func_8092adc
	mov	r1, #3
	mov	r0, #8
	bl	__MapActor_DoAnim
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r2, #0x3c
	mov	r0, #0xb
	bl	__MapActor_Emote
	mov	r0, r9
	bl	OvlFunc_945_200c86c
	mov	r2, #0x28
	mov	r0, #0xd
	ldr	r1, =0x107
	bl	__MapActor_Emote
	mov	r1, #2
	mov	r0, #0xd
	bl	__Func_809259c
	mov	r0, #0xd
	bl	OvlFunc_945_200c86c
	mov	r1, r7
	mov	r0, #9
	bl	OvlFunc_945_200c880
	mov	r1, #4
	mov	r0, #9
	bl	__MapActor_DoAnim
	ldr	r0, =0x1009
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, #0
	bl	OvlFunc_945_200c880
	mov	r1, #1
	mov	r0, #8
	bl	__Func_80925cc
	mov	r0, #8
	bl	OvlFunc_945_200c86c
	mov	r0, #0xb
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r0, #0xc
	ldr	r1, =0x105
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0x3c
	mov	r0, #9
	ldr	r1, =0x105
	bl	__MapActor_Emote
	ldr	r0, =0x13333
	ldr	r1, =0x2666
	bl	__Func_80933d4
	mov	r0, #0xe8
	mov	r2, #0xaa
	mov	r3, #0x80
	mov	r1, #1
	lsl	r3, #21
	lsl	r2, #18
	lsl	r0, #17
	neg	r1, r1
	bl	OvlFunc_945_200c8ac
	mov	r0, #0xa
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #0xa
	bl	OvlFunc_945_200c880
	mov	r0, #0xa
	bl	OvlFunc_945_200c86c
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x81
	lsl	r1, #1
	mov	r2, #0x28
	mov	r0, #0xa
	bl	__MapActor_Emote
	mov	r0, #0xa
	bl	OvlFunc_945_200c86c
	mov	r1, #0x80
	mov	r0, #0xa
	lsl	r1, #8
	mov	r2, #0x14
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #0xa
	lsl	r1, #1
	mov	r2, #0
	bl	__MapActor_Emote
	mov	r2, #0x28
	mov	r1, #4
	mov	r0, #0xa
	bl	__MapActor_Jump
	mov	r0, #0xa
	bl	OvlFunc_945_200c86c
	mov	r1, #1
	mov	r0, #0xa
	bl	__Func_80925cc
	mov	r0, #0xa
	bl	OvlFunc_945_200c86c
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xd
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xdb
	mov	r0, #0xd
	lsl	r1, #1
	ldr	r2, =0x293
	bl	__Func_809218c
	mov	r1, #0x80
	mov	r0, #8
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r11
	mov	r0, #9
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r7
	mov	r0, #0xc
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, r11
	mov	r2, #0
	mov	r0, #0xb
	bl	__Func_8092adc
	mov	r0, #0x11
	bl	__PlaySound
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xa
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xf4
	mov	r0, #0xa
	lsl	r1, #1
	ldr	r2, =0x2ae
	bl	__Func_80921c4
	mov	r1, r11
	mov	r2, #0
	mov	r0, #0xa
	bl	__Func_8092adc
	mov	r0, #0xd
	bl	__MapActor_WaitMovement
	mov	r0, #0xd
	mov	r1, #1
	bl	__MapActor_SetAnim
	mov	r0, #0xd
	mov	r1, r8
	mov	r2, #0
	bl	__Func_8092adc
	bl	__PlayMapMusic
	ldr	r0, =0x921
	bl	__SetFlag
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2009f3c
