	.include "macros.inc"

@ Cutscene: roughly 113 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 7 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Sets save bit 0x303.
.thumb_func_start OvlFunc_927_20095d0
	push	{r5, r6, lr}
	mov	r0, #0xc
	sub	sp, #0x10
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #0xc
	mov	r1, #1
	bl	OvlFunc_927_2008ea8
	mov	r1, #0xc4
	mov	r3, #0xe0
	lsl	r1, #1
	lsl	r3, #11
	mov	r2, #0x68
	mov	r0, #0xc
	bl	OvlFunc_927_2008d90
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r2, [r5, #0x10]
	mov	r3, #0x80
	lsl	r3, #11
	add	r2, r3
	mov	r3, #1
	mov	r4, #0
	ldr	r0, [r5, #8]
	ldr	r1, [r5, #0xc]
	str	r3, [sp, #8]
	mov	r3, #0
	str	r4, [sp]
	str	r4, [sp, #4]
	str	r4, [sp, #0xc]
	bl	OvlFunc_927_2008ae8
	mov	r0, #0xc
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xc
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xc
	mov	r1, #2
	bl	__Func_809259c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0xc
	mov	r5, #0xd4
	mov	r6, #0xc0
	bl	__MapActor_Surprise
	lsl	r5, #1
	lsl	r6, #10
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r3, r6
	mov	r1, r5
	mov	r0, #0xc
	mov	r2, #0x78
	bl	OvlFunc_927_2008d90
	mov	r1, #0xc
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r3, r6
	mov	r1, r5
	mov	r0, #0xc
	mov	r2, #0xa8
	bl	OvlFunc_927_2008d90
	mov	r1, #0xc
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r3, r6
	mov	r1, r5
	mov	r0, #0xc
	mov	r2, #0xd0
	bl	OvlFunc_927_2008d90
	mov	r1, #0xc
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r3, r6
	mov	r1, r5
	mov	r0, #0xc
	mov	r2, #0xe8
	bl	OvlFunc_927_2008d90
	mov	r1, #0xc
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xc
	bl	__MapActor_SetPos
	ldr	r0, =0x303
	bl	__SetFlag
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneEnd
	add	sp, #0x10
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_927_20095d0

@ Cutscene: roughly 117 instructions of straight-line script --
@ 0 turns, 0 animation changes, 0 dialogue lines, 7 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Sets save bit 0x304.
.thumb_func_start OvlFunc_927_20096f0
	push	{r5, lr}
	mov	r0, #0xd
	sub	sp, #0x10
	bl	__MapActor_GetActor
	mov	r5, r0
	bl	__CutsceneStart
	mov	r0, #0xd
	mov	r1, #1
	bl	OvlFunc_927_2008ea8
	mov	r1, #0xe4
	mov	r3, #0xe0
	lsl	r1, #1
	lsl	r3, #11
	mov	r2, #0x68
	mov	r0, #0xd
	bl	OvlFunc_927_2008d90
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r2, [r5, #0x10]
	mov	r3, #0x80
	lsl	r3, #11
	add	r2, r3
	mov	r3, #1
	mov	r4, #0
	ldr	r0, [r5, #8]
	ldr	r1, [r5, #0xc]
	str	r3, [sp, #8]
	mov	r3, #0
	str	r4, [sp]
	str	r4, [sp, #4]
	str	r4, [sp, #0xc]
	bl	OvlFunc_927_2008ae8
	mov	r0, #0xd
	mov	r1, #1
	bl	__SetCameraTarget
	mov	r2, #0
	mov	r1, #0
	mov	r0, #0xd
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0xd
	mov	r1, #2
	bl	__Func_809259c
	mov	r1, #0x81
	lsl	r1, #1
	mov	r0, #0xd
	bl	__MapActor_Surprise
	mov	r0, #0x3c
	bl	__CutsceneWait
	mov	r1, #0xec
	mov	r3, #0xc0
	lsl	r3, #10
	lsl	r1, #1
	mov	r0, #0xd
	mov	r2, #0x88
	bl	OvlFunc_927_2008d90
	mov	r1, #0xd
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r1, #0xfc
	ldr	r3, =0x33333
	lsl	r1, #1
	mov	r0, #0xd
	mov	r2, #0x88
	bl	OvlFunc_927_2008d90
	mov	r1, #0xd
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r5, #0xe0
	mov	r0, #6
	bl	__CutsceneWait
	lsl	r5, #10
	mov	r1, #0x8a
	mov	r3, r5
	lsl	r1, #2
	mov	r0, #0xd
	mov	r2, #0x88
	bl	OvlFunc_927_2008d90
	mov	r1, #0xd
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r1, #0x92
	mov	r3, r5
	lsl	r1, #2
	mov	r0, #0xd
	mov	r2, #0x88
	bl	OvlFunc_927_2008d90
	mov	r1, #0xd
	mov	r2, #0
	mov	r0, #0
	bl	__Func_809280c
	mov	r0, #6
	bl	__CutsceneWait
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0xd
	bl	__MapActor_SetPos
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__SetFlag
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneEnd
	add	sp, #0x10
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_927_20096f0
