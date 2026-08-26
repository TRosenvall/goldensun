	.include "macros.inc"
	.include "gba.inc"


@ 119 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound x2, RotateVector, GetTerrainHeight, RotateVector
@   GetTerrainHeight, SetEntityAnimation x3
.thumb_func_start OvlFunc_932_200b738
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r0
	mov	r0, #0xc0
	lsl	r0, #8
	mov	r9, r0
	ldrh	r3, [r7, #6]
	mov	r1, r9
	ldr	r0, [r7, #0xc]
	and	r1, r3
	sub	sp, #0xc
	mov	r9, r1
	cmp	r0, #0
	bge	.L375e
	ldr	r2, =0xffff
	add	r0, r2
.L375e:
	mov	r3, r7
	asr	r0, #16
	add	r3, #0x64
	mov	r10, r0
	mov	r0, #0
	ldrsh	r3, [r3, r0]
	mov	r1, r10
	sub	r3, r1
	ldr	r2, =.L51b4
	lsl	r3, #2
	add	r3, #0x40
	ldr	r2, [r2, r3]
	mov	r10, r2
	mov	r2, #0x66
	add	r2, r7
	mov	r8, r2
	mov	r1, r8
	mov	r0, #0
	ldrsh	r3, [r1, r0]
	ldrh	r2, [r2]
	cmp	r3, #0
	beq	.L37b0
	sub	r3, r2, #1
	mov	r0, #0xa0
	mov	r2, r8
	strh	r3, [r2]
	lsl	r0, #13
	lsl	r3, #16
	cmp	r3, r0
	bne	.L37a0
	mov	r0, #0xb8
	bl	__PlaySound
.L37a0:
	mov	r2, r8
	mov	r1, #0
	ldrsh	r3, [r2, r1]
	cmp	r3, #0
	bne	.L37b0
	mov	r0, #0xe9
	bl	__PlaySound
.L37b0:
	ldr	r3, [r7, #8]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r7, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r7, #0x10]
	mov	r1, #0xc0
	str	r3, [r5, #8]
	mov	r0, r10
	lsl	r1, #8
	ldr	r3, =Func_8000888
	.call_via r3
	mov	r1, r9
	mov	r2, r5
	bl	__vec3_translate
	ldr	r1, [r5]
	str	r1, [r7, #8]
	ldr	r2, [r5, #8]
	mov	r0, #2
	str	r2, [r7, #0x10]
	bl	__Func_8011f54
	mov	r1, #0xc0
	mov	r6, r0
	lsl	r1, #9
	mov	r0, r10
	ldr	r3, =Func_8000888
	.call_via r3
	neg	r0, r0
	mov	r1, r9
	mov	r2, r5
	bl	__vec3_translate
	ldr	r1, [r5]
	ldr	r2, [r5, #8]
	mov	r0, #2
	bl	__Func_8011f54
	mov	r2, r8
	mov	r1, #0
	ldrsh	r3, [r2, r1]
	cmp	r3, #0x14
	bgt	.L3832
	cmp	r6, r0
	bne	.L381c
	mov	r0, r7
	mov	r1, #2
	bl	__Actor_SetAnim
	b	.L3832
.L381c:
	cmp	r6, r0
	ble	.L382a
	mov	r0, r7
	mov	r1, #3
	bl	__Actor_SetAnim
	b	.L3832
.L382a:
	mov	r0, r7
	mov	r1, #4
	bl	__Actor_SetAnim
.L3832:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200b738

@ 157 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, GetSlotEntity, BeginCutscene, RotateVector
@   GetTerrainHeight, AddActorPart, AttachCameraToSlot, WaitForCameraArrival
@   SetCameraSpeed, RotateVector, SetEntityMoveTarget, WaitForEntityIdle
@   PlaySound, GetTileFlags
@   ... and 7 more
.thumb_func_start OvlFunc_932_200b850
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	sub	sp, #0xc
	mov	r8, r1
	mov	r9, r0
	bl	__MapActor_GetActor
	ldr	r3, =gState
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	mov	r6, r0
	ldr	r0, [r3]
	bl	__GetFieldActor
	mov	r10, r0
	bl	__CutsceneStart
	mov	r3, #1
	neg	r3, r3
	cmp	r8, r3
	bne	.L3886
	ldrh	r2, [r6, #6]
	mov	r8, r2
.L3886:
	mov	r7, #0
	mov	r5, sp
	b	.L3898

	.pool_aligned

.L3890:
	mov	r3, #0x80
	lsl	r3, #7
	add	r8, r3
	add	r7, #1
.L3898:
	cmp	r7, #3
	bgt	.L38c4
	ldr	r3, [r6, #8]
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, #0x80
	str	r3, [r5, #8]
	lsl	r0, #13
	mov	r1, r8
	mov	r2, r5
	bl	__vec3_translate
	ldr	r1, [r5]
	ldr	r2, [r5, #8]
	mov	r0, #2
	bl	__Func_8011f54
	ldr	r3, [r6, #0xc]
	cmp	r0, r3
	bne	.L3890
.L38c4:
	cmp	r7, #4
	beq	.L39b6
	mov	r2, r6
	mov	r3, #2
	add	r2, #0x22
	strb	r3, [r2]
	mov	r5, #0
	mov	r2, r10
	str	r5, [r2, #8]
	str	r5, [r2, #0x10]
	mov	r1, #0x10
	ldr	r0, [r6, #0x50]
	bl	__Sprite_AddLayer
	mov	r0, r9
	mov	r1, #1
	bl	__SetCameraTarget
	bl	__Func_8093530
	mov	r0, #0x80
	mov	r1, #0x80
	lsl	r0, #13
	lsl	r1, #10
	bl	__Func_80933d4
	mov	r3, r8
	strh	r3, [r6, #6]
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r6, #0x30]
	ldr	r3, =0xccc
	ldr	r2, .L391c	@ 0
	str	r3, [r6, #0x34]
	mov	r3, r6
	add	r3, #0x5b
	strb	r2, [r3]
	ldr	r2, [r6, #0xc]
	cmp	r2, #0
	bge	.L3928
	ldr	r3, =0xffff
	add	r2, r3
	b	.L3928

	.align	2, 0
.L391c:
	.word	0
	.pool

.L3928:
	mov	r3, r6
	asr	r2, #16
	add	r3, #0x64
	strh	r2, [r3]
	add	r3, #2
	strh	r5, [r3]
	ldr	r3, [r6, #8]
	mov	r5, sp
	str	r3, [r5]
	ldr	r3, [r6, #0xc]
	str	r3, [r5, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, #0xc0
	str	r3, [r5, #8]
	lsl	r0, #13
	mov	r1, r8
	mov	r2, r5
	bl	__vec3_translate
	ldr	r1, [r5]
	ldr	r2, [r6, #0xc]
	ldr	r3, [r5, #8]
	mov	r0, r6
	bl	__Actor_TravelTo
	mov	r0, r6
	bl	__Actor_WaitMovement
	mov	r0, #0xe9
	bl	__PlaySound
.L3966:
	ldr	r1, [r6, #8]
	ldr	r2, [r6, #0x10]
	mov	r0, #2
	bl	__Func_8012038
	cmp	r0, #0x62
	beq	.L3988
	cmp	r0, #0x62
	bgt	.L3982
	cmp	r0, #0x60
	beq	.L3998
	cmp	r0, #0x61
	beq	.L3990
	b	.L399e
.L3982:
	cmp	r0, #0x63
	beq	.L39ac
	b	.L399e
.L3988:
	mov	r0, r6
	bl	OvlFunc_932_200b5ac
	b	.L399e
.L3990:
	mov	r0, r6
	bl	OvlFunc_932_200b668
	b	.L399e
.L3998:
	mov	r0, r6
	bl	OvlFunc_932_200b724
.L399e:
	mov	r0, r6
	bl	OvlFunc_932_200b738
	mov	r0, #1
	bl	__WaitFrames
	b	.L3966
.L39ac:
	mov	r0, r6
	bl	OvlFunc_932_200b484
	bl	__CutsceneEnd
.L39b6:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200b850

@ Leaf helper, 43 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: REG_DMA3SAD, iwram_1ed0
.thumb_func_start OvlFunc_932_200b9c8
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ed0
	ldr	r4, =.L525c
	ldr	r6, [r3]
	mov	r2, #0
	ldrsh	r3, [r4, r2]
	cmp	r3, #0
	bgt	.L3a28
.L39d8:
	ldr	r1, =.L5260
	ldrh	r3, [r1]
	ldr	r5, =s8_ARRAY_932__0200bd28
	add	r2, r3, #1
	lsl	r3, #16
	asr	r3, #16
	ldrsb	r0, [r5, r3]
	mov	r3, #1
	neg	r3, r3
	strh	r2, [r1]
	cmp	r0, r3
	bne	.L3a0c
	ldr	r3, .L39f8	@ 0
	strh	r3, [r1]
	b	.L39d8

	.align	2, 0
.L39f8:
	.word	0
	.pool

.L3a0c:
	add	r3, r2, #1
	strh	r3, [r1]
	lsl	r3, r2, #16
	asr	r3, #16
	ldrsb	r3, [r5, r3]
	ldr	r4, =.L525c
	lsl	r0, #1
	strh	r3, [r4]
	add	r0, r6, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =0x5000006
	ldr	r2, =0x80000009
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.L3a28:
	ldrh	r3, [r4]
	sub	r3, #1
	strh	r3, [r4]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_200b9c8
