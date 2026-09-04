	.include "macros.inc"
	.include "gba.inc"

@ 49 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed x2, PlaySound, GetSlotEntityChecked
@   MoveSlotTo, WaitForSlotArrival, MoveSlotBy, PlaySound
@   MoveSlotBy, WaitForSlotArrival, MoveSlotTo, WaitForSlotArrival
@   EndCutscene, ClearSaveBit
@ clears 0x220.
.thumb_func_start OvlFunc_948_2009838
	push	{lr}
	bl	__CutsceneStart
	mov	r0, #0
	ldr	r1, =0x1b333
	ldr	r2, =0xd999
	bl	__MapActor_SetSpeed
	mov	r0, #0xc
	ldr	r1, =0x1b333
	ldr	r2, =0xd999
	bl	__MapActor_SetSpeed
	mov	r0, #0xbc
	bl	__PlaySound
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1870
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0xc
	bl	__MapActor_TravelTo
.L1870:
	mov	r0, #0xc
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r2, #0x18
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #0xbc
	bl	__PlaySound
	mov	r1, #0
	mov	r2, #0x10
	mov	r0, #0xc
	bl	__Func_809228c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #0x9c
	lsl	r1, #1
	mov	r2, #0xe8
	mov	r0, #0xc
	bl	__MapActor_TravelTo
	mov	r0, #0xc
	bl	__MapActor_WaitMovement
	bl	__CutsceneEnd
	mov	r0, #0x88
	lsl	r0, #2
	bl	__ClearFlag
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_2009838
