	.include "macros.inc"

@ Cutscene: roughly 496 instructions of straight-line script --
@ 17 turns, 13 animation changes, 10 dialogue lines, 30 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message base 0x1ffb.
@ Reads save bit 0x96a.
@ Sets save bit 0x96a.
.thumb_func_start OvlFunc_952_2008af8
	push	{r5, r6, lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldrh	r3, [r0, #6]
	lsl	r2, #6
	add	r3, r2
	ldr	r2, =0xffffc000
	and	r3, r2
	lsl	r3, #16
	asr	r5, r3, #16
	bl	__CutsceneStart
	bl	__Func_808e118
	ldr	r0, =0x1ffb
	bl	__MessageID
	mov	r0, #1
	mov	r1, #0
	neg	r0, r0
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	beq	.Lb36
	b	.Lfde
.Lb36:
	ldr	r0, =0x96a
	bl	__GetFlag
	b	.Lb4c

	.pool_aligned

.Lb4c:
	cmp	r0, #0
	beq	.Lb5e
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	bl	__InnHeal
	b	.Lfe2
.Lb5e:
	mov	r0, #0x14
	bl	__CutsceneWait
	lsl	r3, r5, #16
	lsr	r5, r3, #16
	cmp	r5, #0
	bne	.Lb76
	mov	r0, #0
	mov	r1, #0x80
	mov	r2, #0x78
	bl	__Func_80921c4
.Lb76:
	mov	r6, #0x80
	lsl	r6, #8
	cmp	r5, r6
	bne	.Lb88
	mov	r0, #0
	mov	r1, #0xf0
	mov	r2, #0x78
	bl	__Func_80921c4
.Lb88:
	mov	r0, #0
	mov	r1, #0xb8
	mov	r2, #0x78
	bl	__Func_80921c4
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r3, r6
	mov	r2, #0
	mov	r1, #0x10
	mov	r0, #1
	bl	__Func_809233c
	mov	r0, #1
	bl	__MapActor_WaitScript
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #1
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #9
	mov	r2, r6
	bl	__MapActor_SetSpeed
	mov	r0, #1
	mov	r1, #0x28
	mov	r2, #0
	bl	__Func_8092304
	mov	r2, #0x20
	mov	r0, #1
	mov	r1, #0
	neg	r2, r2
	bl	__Func_8092304
	mov	r1, #0xc0
	lsl	r1, #7
	mov	r2, #0
	mov	r0, #1
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, r6
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x10
	mov	r3, #0
	mov	r2, #0
	neg	r1, r1
	mov	r0, #2
	bl	__Func_809233c
	mov	r0, #2
	bl	__MapActor_WaitScript
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #2
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #2
	lsl	r1, #9
	mov	r2, r6
	bl	__MapActor_SetSpeed
	mov	r1, #0x28
	mov	r0, #2
	neg	r1, r1
	mov	r2, #0
	bl	__Func_8092304
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8092304
	mov	r1, #0xe0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #2
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r2, #0
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r3, r6
	mov	r2, #0
	mov	r1, #0x10
	mov	r0, #3
	bl	__Func_809233c
	mov	r0, #3
	bl	__MapActor_WaitScript
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #3
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0x80
	mov	r0, #3
	lsl	r1, #9
	mov	r2, r6
	bl	__MapActor_SetSpeed
	mov	r0, #3
	mov	r1, #0x28
	mov	r2, #0
	bl	__Func_8092304
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0x28
	bl	__Func_8092304
	mov	r1, #0xa0
	lsl	r1, #8
	mov	r2, #0
	mov	r0, #3
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	mov	r3, #0
	neg	r0, r0
	neg	r1, r1
	neg	r2, r2
	bl	__Func_80933f8
	mov	r1, #0x38
	mov	r0, #0
	neg	r1, r1
	mov	r2, #0
	bl	__Func_8092304
	mov	r2, #0x20
	mov	r0, #0
	mov	r1, #0
	neg	r2, r2
	bl	__Func_8092304
	mov	r1, #0x80
	mov	r2, #0
	lsl	r1, #6
	mov	r0, #0
	bl	__Func_8092adc
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #3
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #1
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x96a
	bl	__GetFlag
	cmp	r0, #0
	bne	.Ld6e
	mov	r0, #1
	mov	r1, #0
	bl	__ActorMessage
	b	.Ld7e
.Ld6e:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Ld7e:
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x96a
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lda0
	mov	r0, #2
	mov	r1, #0
	bl	__ActorMessage
	b	.Ldb0
.Lda0:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Ldb0:
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #1
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0x80
	mov	r0, #3
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0
	mov	r2, #0
	mov	r0, #2
	bl	__Func_8092adc
	mov	r0, #0
	bl	__InnHeal
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #6
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xc0
	mov	r0, #1
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xa0
	mov	r0, #3
	lsl	r1, #8
	mov	r2, #0
	bl	__Func_8092adc
	mov	r1, #0xe0
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #2
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #1
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r0, #1
	mov	r1, #0
	bl	__ActorMessage
	mov	r0, #2
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x96a
	bl	__GetFlag
	cmp	r0, #0
	bne	.Le56
	mov	r0, #2
	mov	r1, #0
	bl	__ActorMessage
	b	.Le66
.Le56:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Le66:
	mov	r0, #3
	mov	r1, #3
	bl	__MapActor_DoAnim
	mov	r0, #0xa
	bl	__CutsceneWait
	ldr	r0, =0x96a
	bl	__GetFlag
	cmp	r0, #0
	bne	.Le88
	mov	r0, #3
	mov	r1, #0
	bl	__ActorMessage
	b	.Le98
.Le88:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
.Le98:
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0x20
	bl	__Func_8092304
	mov	r1, #0x70
	mov	r0, #1
	neg	r1, r1
	mov	r2, #0
	bl	__Func_80922c4
	mov	r2, #0x28
	mov	r0, #3
	mov	r1, #0
	neg	r2, r2
	bl	__Func_8092304
	mov	r1, #0x70
	neg	r1, r1
	mov	r2, #0
	mov	r0, #3
	bl	__Func_80922c4
	mov	r0, #0x32
	bl	__CutsceneWait
	mov	r2, #0x18
	mov	r1, #0
	neg	r2, r2
	mov	r0, #2
	bl	__Func_8092304
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r1, #0x80
	mov	r0, #0
	lsl	r1, #7
	mov	r2, #0
	bl	__Func_8092adc
	mov	r2, #0x10
	mov	r1, #0
	neg	r2, r2
	mov	r0, #1
	bl	__Func_8092304
	mov	r0, #3
	bl	__MapActor_WaitMovement
	mov	r1, #0xc0
	mov	r2, #0
	lsl	r1, #8
	mov	r0, #3
	bl	__Func_8092adc
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r1, #0
	mov	r0, #1
	bl	__ActorMessage
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r1, #3
	mov	r0, #0
	bl	__MapActor_DoAnim
	mov	r0, #0x14
	bl	__CutsceneWait
	mov	r0, #1
	ldr	r1, =0x13333
	ldr	r2, =0x9999
	bl	__MapActor_SetSpeed
	mov	r0, #2
	ldr	r1, =0x13333
	ldr	r2, =0x9999
	bl	__MapActor_SetSpeed
	mov	r0, #3
	ldr	r1, =0x13333
	ldr	r2, =0x9999
	bl	__MapActor_SetSpeed
	mov	r0, #1
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lf68
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #1
	bl	__MapActor_TravelTo
.Lf68:
	mov	r0, #1
	bl	__MapActor_WaitMovement
	mov	r0, #1
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #3
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lf98
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #3
	bl	__MapActor_TravelTo
.Lf98:
	mov	r0, #3
	bl	__MapActor_WaitMovement
	mov	r0, #3
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #2
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lfc8
	mov	r2, #0xa
	ldrsh	r1, [r0, r2]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #2
	bl	__MapActor_TravelTo
.Lfc8:
	mov	r0, #2
	bl	__MapActor_WaitMovement
	mov	r0, #2
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	ldr	r0, =0x96a
	bl	__SetFlag
.Lfde:
	bl	__CutsceneEnd
.Lfe2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_952_2008af8
