	.include "macros.inc"

@ Cutscene: roughly 113 instructions of straight-line script --
@ 0 turns, 2 animation changes, 1 dialogue line, 0 timed pauses.
@ Characterised structurally rather than beat by beat.
@ Message bases 0x1e84, 0x1ea2.
@ Reads save bits 0x300, 0x929, 0x92a, 0x92b.
@ Sets save bits 0x300, 0x918, 0x92d, 0x936.
.thumb_func_start OvlFunc_945_2008cc8
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ld22
	bl	OvlFunc_945_20092dc
	mov	r5, r0
	bl	OvlFunc_945_2009190
	ldr	r0, =0x1ea2
	bl	__MessageID
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r0, r5
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Ld10
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, r5
	bl	__MapActor_TravelTo
.Ld10:
	mov	r0, r5
	bl	__MapActor_WaitMovement
	mov	r0, r5
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.Lde0
.Ld22:
	ldr	r0, =0x1e84
	bl	__MessageID
	mov	r2, #0x3c
	mov	r0, #9
	mov	r1, #0
	bl	__Func_8093040
	mov	r0, #9
	mov	r1, #1
	bl	__Func_80925cc
	mov	r1, #0
	mov	r0, #9
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.Ldca
	mov	r0, #9
	bl	OvlFunc_945_200c86c
	mov	r0, #9
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Ld74
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #9
	bl	__MapActor_TravelTo
.Ld74:
	mov	r0, #9
	bl	__MapActor_WaitMovement
	mov	r0, #9
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__SetFlag
	ldr	r0, =0x92b
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ld9e
	ldr	r0, =0x991
	bl	__SetFlag
	b	.Lde0
.Ld9e:
	ldr	r0, =0x92a
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ldb0
	ldr	r0, =0x918
	bl	__SetFlag
	b	.Lde0
.Ldb0:
	ldr	r0, =0x929
	bl	__GetFlag
	cmp	r0, #0
	beq	.Ldc2
	ldr	r0, =0x936
	bl	__SetFlag
	b	.Lde0
.Ldc2:
	ldr	r0, =0x92d
	bl	__SetFlag
	b	.Lde0
.Ldca:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #9
	bl	OvlFunc_945_200c86c
.Lde0:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2008cc8
