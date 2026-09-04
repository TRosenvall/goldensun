	.include "macros.inc"

@ Cutscene: roughly 73 instructions of straight-line script --
@ 3 turns, 0 animation changes, 2 dialogue lines, 1 timed pause.
@ Characterised structurally rather than beat by beat.
@ Message base 0xe3d.
@ Sets save bit 0x8c1.
.thumb_func_start OvlFunc_949_2008894
	push	{lr}
	bl	__CutsceneStart
	mov	r1, #0x98
	mov	r2, #0x9c
	mov	r0, #0
	lsl	r1, #1
	lsl	r2, #1
	bl	__Func_80921c4
	mov	r1, #0xc0
	mov	r0, #0
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	lsl	r1, #7
	mov	r2, #0
	mov	r0, #0x1c
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0xe3d
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0x1c
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.L936
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #0x1c
	mov	r1, #0
	bl	__ActorMessage
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0x1c
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r1, #0xa0
	mov	r2, #0x98
	mov	r0, #0x1c
	lsl	r1, #1
	lsl	r2, #1
	bl	__Func_80921c4
	mov	r1, #0x9e
	mov	r2, #0xa4
	mov	r0, #0x1c
	lsl	r1, #1
	lsl	r2, #1
	bl	__Func_80921c4
	mov	r1, #0xa0
	mov	r0, #0x1c
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	ldr	r0, =0x8c1
	bl	__SetFlag
	b	.L93e
.L936:
	mov	r0, #0x1c
	mov	r1, #0
	bl	__ActorMessage
.L93e:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_949_2008894
