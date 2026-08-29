	.include "macros.inc"

@ Cutscene: roughly 111 instructions of straight-line script --
@ 1 turn, 2 animation changes, 4 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x183a, 0x18ac, 0x18ae.
.thumb_func_start OvlFunc_926_200902c
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r1, =0xcccc
	mov	r0, #0xf
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r0, #0x3c
	bl	__CutsceneWait
	ldr	r5, =0x183a
	mov	r0, r5
	bl	__MessageID
	cmp	r6, #0
	bne	.L10a4
	sub	r0, r5, #1
	bl	__MessageID
	mov	r0, #0xf
	ldr	r1, =0x101
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r2, #0x14
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_80925cc
	ldr	r0, =0x18ae
	bl	__MessageID
	mov	r2, #0x14
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #4
	mov	r0, #0xf
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #0xf
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
.L10a4:
	cmp	r6, #2
	bne	.L10bc
	ldr	r0, =0x18ac
	bl	__MessageID
	mov	r0, #0xf
	mov	r1, #2
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
.L10bc:
	mov	r2, #0x14
	mov	r0, #0xf
	mov	r1, #0
	bl	__Func_8093040
	bl	OvlFunc_926_2008f80
	mov	r0, #0xf
	mov	r1, #3
	bl	__Func_80925cc
	mov	r1, #0xe8
	mov	r2, #0xa8
	mov	r0, #0x13
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xe8
	mov	r2, #0xa8
	lsl	r1, #16
	lsl	r2, #16
	mov	r0, #0x14
	bl	__MapActor_SetPos
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #12
	str	r3, [r0, #0xc]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r0, #0x3c]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	ldr	r3, =0xcccc
	str	r3, [r0, #0x18]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x80
	ldr	r2, [r0, #0x50]
	lsl	r3, #8
	strh	r3, [r2, #0x1e]
	mov	r0, #0x7c
	bl	__PlaySound
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xf
	mov	r1, #0xd8
	mov	r2, #0x98
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r0, #0xf
	lsl	r1, #6
	mov	r2, #0x1e
	bl	__Func_8092adc
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_926_200902c

@ Cutscene: roughly 141 instructions of straight-line script --
@ 1 turn, 3 animation changes, 5 dialogue lines, 7 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x187a.
@ Sets save bit 0x301.
.thumb_func_start OvlFunc_926_2009160
	push	{lr}
	mov	r0, #0xd
	mov	r1, #0x13
	mov	r2, #0
	bl	__Func_809280c
	mov	r0, #0xe
	mov	r1, #0x13
	mov	r2, #0
	bl	__Func_809280c
	mov	r0, #0xf
	mov	r1, #0x13
	mov	r2, #0
	bl	__Func_809280c
	mov	r0, #0x10
	mov	r1, #0x13
	mov	r2, #0
	bl	__Func_809280c
	mov	r2, #0
	mov	r1, #0x13
	mov	r0, #0x12
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x187a
	bl	__MessageID
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #0x12
	ldr	r1, =0x105
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r0, #0x10
	ldr	r1, =0x101
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r2, #0x14
	mov	r0, #0x10
	mov	r1, #0
	bl	__Func_8093040
	mov	r1, #4
	mov	r0, #0x12
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0x12
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r1, #0x81
	mov	r0, #0x10
	lsl	r1, #1
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r2, #0
	mov	r1, #0x12
	mov	r0, #0xf
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0x12
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0xf
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r2, =0x6666
	mov	r0, #0xf
	ldr	r1, =0xcccc
	bl	__MapActor_SetSpeed
	bl	OvlFunc_926_2008f80
	mov	r0, #0xf
	mov	r1, #3
	bl	__Func_80925cc
	mov	r1, #0xe8
	mov	r2, #0xa8
	mov	r0, #0x13
	lsl	r1, #16
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r1, #0xe8
	mov	r2, #0xa8
	lsl	r1, #16
	lsl	r2, #16
	mov	r0, #0x14
	bl	__MapActor_SetPos
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0xc0
	lsl	r3, #12
	str	r3, [r0, #0xc]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x80
	lsl	r3, #24
	str	r3, [r0, #0x3c]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	ldr	r3, =0xcccc
	str	r3, [r0, #0x18]
	mov	r0, #0x13
	bl	__MapActor_GetActor
	mov	r3, #0x80
	ldr	r2, [r0, #0x50]
	lsl	r3, #8
	strh	r3, [r2, #0x1e]
	mov	r0, #0x7c
	bl	__PlaySound
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r0, #0xf
	mov	r1, #0xd8
	mov	r2, #0x98
	bl	__Func_80921c4
	mov	r1, #0x80
	mov	r0, #0xf
	lsl	r1, #7
	mov	r2, #0x1e
	bl	__Func_8092adc
	ldr	r0, =0x301
	bl	__SetFlag
	pop	{r0}
	bx	r0
.func_end OvlFunc_926_2009160
