	.include "macros.inc"
	.include "gba.inc"

@ Cutscene: roughly 113 instructions of straight-line script --
@ 1 turn, 4 animation changes, 2 dialogue lines, 5 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x2691.
@ Reads save bit 0x986.
@ Sets save bit 0x986.
.thumb_func_start OvlFunc_968_20094f4
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #0xc
	bl	__MapActor_GetActor
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r3, #0x35
	beq	.L150a
	b	.L1612
.L150a:
	ldr	r0, =0x986
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1612
	ldr	r0, =0x986
	bl	__SetFlag
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L152e
	ldr	r1, [r0, #8]
	ldr	r2, [r0, #0x10]
	mov	r0, #1
	bl	__MapActor_SetPos
.L152e:
	mov	r0, #1
	ldr	r1, =0xcccc
	ldr	r2, =0x6666
	bl	__MapActor_SetSpeed
	mov	r1, #0xce
	mov	r0, #1
	lsl	r1, #2
	mov	r2, #0x58
	bl	__Func_80921c4
	mov	r1, #0xce
	mov	r0, #1
	lsl	r1, #2
	mov	r2, #0x68
	bl	__Func_80921c4
	mov	r2, #0
	mov	r1, #0
	mov	r0, #1
	bl	__Func_8092848
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #4
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x2691
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x14
	bl	__Func_8093040
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0xa
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #1
	mov	r2, #0x3c
	bl	__MapActor_Emote
	mov	r2, #0
	mov	r1, #0
	mov	r0, #1
	bl	__Func_809280c
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #1
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r2, #0x14
	mov	r0, #1
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #0
	mov	r1, #3
	bl	__MapActor_SetAnim
	mov	r1, #3
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #0xce
	mov	r0, #1
	lsl	r1, #2
	mov	r2, #0x58
	bl	__Func_80921c4
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L15fe
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.L15fe:
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	bl	__CutsceneEnd
.L1612:
	pop	{r0}
	bx	r0
.func_end OvlFunc_968_20094f4
