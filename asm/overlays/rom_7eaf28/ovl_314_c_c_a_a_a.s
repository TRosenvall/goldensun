	.include "macros.inc"
	.include "gba.inc"

@ 83 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   ClearSaveBit, SetSaveBit x2, WriteSaveByte, LoadMapByIdAndEntrance x3
@   SetSlotWalkBehaviour, PlaceSlotAt, GetSlotEntityChecked
@ clears 0x20f.
.thumb_func_start OvlFunc_960_2008b24
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r6, r1
	ldr	r3, [r3]
	mov	r1, #0xc1
	lsl	r1, #1
	add	r2, r3, r1
	mov	r1, #0
	ldrsh	r3, [r2, r1]
	cmp	r3, #0x63
	bne	.Lb3e
	mov	r3, #0
	strh	r3, [r2]
.Lb3e:
	ldr	r0, =0x20f
	bl	__ClearFlag
	ldr	r3, =gState
	mov	r2, #0xe0
	lsl	r2, #1
	add	r3, r2
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa4
	cmp	r2, r3
	bne	.Lb60
	ldr	r2, =0x2f9
	add	r0, r6, r2
	bl	__SetFlag
	b	.Lb6e
.Lb60:
	ldr	r3, =0xa5
	cmp	r2, r3
	bne	.Lb6e
	ldr	r3, =0x309
	add	r0, r6, r3
	bl	__SetFlag
.Lb6e:
	mov	r0, #0x84
	lsl	r0, #2
	mov	r1, #0
	bl	__SetFlagByte
	mov	r0, #0x62
	mov	r1, #5
	bl	__Func_8091eb0
	ldr	r1, =gState
	ldr	r3, =0x22b
	add	r2, r1, r3
	mov	r3, #3
	strb	r3, [r2]
	mov	r5, r1
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r5, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa5
	cmp	r2, r3
	bne	.Lbc6
	cmp	r6, #0xb
	bne	.Lbaa
	mov	r0, #0x62
	mov	r1, #7
	bl	__Func_8091eb0
	b	.Lbc6
.Lbaa:
	cmp	r6, #0xc
	bne	.Lbc6
	mov	r1, #6
	mov	r0, #0x62
	bl	__Func_8091eb0
	mov	r0, #0xc
	bl	__MapActor_SetIdle
	mov	r0, #0xc
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
.Lbc6:
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r5, r2
	ldr	r0, [r3]
	bl	__MapActor_GetActor
	mov	r3, #3
	add	r0, #0x55
	strb	r3, [r0]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2008b24
