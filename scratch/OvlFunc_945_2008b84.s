	.include "macros.inc"

.thumb_func_start OvlFunc_945_2008b84
	push	{r5, lr}
	bl	__CutsceneStart
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lbde
	bl	OvlFunc_945_20092dc
	mov	r5, r0
	bl	OvlFunc_945_2009190
	ldr	r0, =0x1ea1
	bl	__MessageID
	mov	r0, #0xc
	bl	OvlFunc_945_200c86c
	mov	r0, r5
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lbcc
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, r5
	bl	__MapActor_TravelTo
.Lbcc:
	mov	r0, r5
	bl	__MapActor_WaitMovement
	mov	r0, r5
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	b	.Lc9a
.Lbde:
	mov	r1, #2
	mov	r0, #0xc
	bl	__Func_80925cc
	mov	r0, #0x14
	bl	__CutsceneWait
	ldr	r0, =0x1e81
	bl	__MessageID
	mov	r1, #0
	mov	r0, #0xc
	bl	__Func_8092c40
	mov	r0, #0
	mov	r1, #0
	bl	__Func_8091c7c
	cmp	r0, #0
	bne	.Lc84
	mov	r0, #0xc
	bl	OvlFunc_945_200c86c
	mov	r0, #0xc
	mov	r1, #2
	bl	__MapActor_SetAnim
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.Lc2c
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0xc
	bl	__MapActor_TravelTo
.Lc2c:
	mov	r0, #0xc
	bl	__MapActor_WaitMovement
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0xc0
	lsl	r0, #2
	bl	__SetFlag
	ldr	r0, =0x92b
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc56
	ldr	r0, =0x994
	bl	__SetFlag
	b	.Lc9a
.Lc56:
	ldr	r0, =0x92a
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc68
	ldr	r0, =0x91b
	bl	__SetFlag
	b	.Lc9a
.Lc68:
	ldr	r0, =0x929
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc7a
	ldr	r0, =0x939
	bl	__SetFlag
	b	.Lc9a
.Lc7a:
	mov	r0, #0x93
	lsl	r0, #4
	bl	__SetFlag
	b	.Lc9a
.Lc84:
	ldr	r3, =iwram_3001ebc
	ldr	r2, [r3]
	mov	r3, #0xec
	lsl	r3, #1
	add	r2, r3
	ldrh	r3, [r2]
	add	r3, #1
	strh	r3, [r2]
	mov	r0, #0xc
	bl	OvlFunc_945_200c86c
.Lc9a:
	bl	__CutsceneEnd
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_2008b84
