	.include "macros.inc"

@ Counter: shop type 0xf, 0x11 via UI_Sanctum, opened only from inside the facing arc.
@ Outside it the attendant speaks instead -- lines 0x1f48, 0x1f7b, 0x1f7d, 0x1f7f, 0x1f81.
@ Gated on save bits 0x8a0, 0x925, 0x928, 0x93e.
.thumb_func_start OvlFunc_945_2009894
	push	{lr}
	mov	r0, #0
	bl	__MapActor_GetActor
	ldr	r2, =0xffffe000
	ldrh	r3, [r0, #6]
	add	r3, r2
	mov	r2, #0xc0
	lsl	r3, #16
	lsl	r2, #24
	cmp	r3, r2
	bls	.L18d0
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18c8
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	bne	.L18c8
	mov	r0, #0x11
	bl	__UI_Sanctum
	b	.L194e
.L18c8:
	mov	r0, #0xf
	bl	__UI_Sanctum
	b	.L194e
.L18d0:
	bl	__CutsceneStart
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18e6
	ldr	r0, =0x1f81
	bl	__MessageID
	b	.L1924
.L18e6:
	mov	r0, #0x8a
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L18fa
	ldr	r0, =0x1f48
	bl	__MessageID
	b	.L1924
.L18fa:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L190c
	ldr	r0, =0x1f7f
	bl	__MessageID
	b	.L1924
.L190c:
	ldr	r0, =0x925
	bl	__GetFlag
	cmp	r0, #0
	beq	.L191e
	ldr	r0, =0x1f7d
	bl	__MessageID
	b	.L1924
.L191e:
	ldr	r0, =0x1f7b
	bl	__MessageID
.L1924:
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1942
	ldr	r0, =0x93e
	bl	__GetFlag
	cmp	r0, #0
	bne	.L1942
	mov	r0, #0x11
	mov	r1, #0
	bl	__ActorMessage
	b	.L194a
.L1942:
	mov	r0, #0xf
	mov	r1, #0
	bl	__ActorMessage
.L194a:
	bl	__CutsceneEnd
.L194e:
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2009894

@ 46 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, CloseMessageWindow, ClearSaveBit, TestSaveBit
@   SetSpawnPositionA, LoadMapByIdAndEntrance, TestSaveBit, SetSpawnPositionA
@   LoadMapByIdAndEntrance, TestSaveBit, SetSpawnPositionA, LoadMapByIdAndEntrance
@   EndCutscene
@ reads save bits 0x928, 0x929, 0x92a; clears 0x8f0.
.thumb_func_start OvlFunc_945_2009978
	push	{lr}
	bl	__CutsceneStart
	bl	__Func_808e118
	ldr	r3, =gState
	ldr	r2, =0x22b
	add	r3, r2
	mov	r2, #3
	strb	r2, [r3]
	mov	r0, #0x8f
	lsl	r0, #4
	bl	__ClearFlag
	ldr	r0, =0x928
	bl	__GetFlag
	cmp	r0, #0
	bne	.L19b0
	ldr	r0, =0x6f
	mov	r1, #0x10
	bl	__Func_8091f90
	mov	r0, #0x3e
	mov	r1, #0
	bl	__Func_8091eb0
	b	.L19e6
.L19b0:
	ldr	r0, =0x929
	bl	__GetFlag
	cmp	r0, #0
	bne	.L19cc
	ldr	r0, =0x6f
	mov	r1, #0x12
	bl	__Func_8091f90
	mov	r0, #0x3e
	mov	r1, #1
	bl	__Func_8091eb0
	b	.L19e6
.L19cc:
	ldr	r0, =0x92a
	bl	__GetFlag
	cmp	r0, #0
	bne	.L19e6
	ldr	r0, =0x6f
	mov	r1, #0x14
	bl	__Func_8091f90
	mov	r0, #0x3e
	mov	r1, #2
	bl	__Func_8091eb0
.L19e6:
	bl	__CutsceneEnd
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2009978
