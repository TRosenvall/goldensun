	.include "macros.inc"
	.include "gba.inc"

@ 58 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   BeginCutscene, SetSlotEntitySpeed x2, PlaySound, GetSlotEntityChecked
@   MoveSlotTo, WaitForSlotArrival, MoveSlotBy, DialogueWait
@   PlaySound, MoveSlotBy, WaitForSlotArrival, MoveSlotTo
@   WaitForSlotArrival, DialogueWait
@   ... and 1 more
.thumb_func_start OvlFunc_948_20095f0
	push	{lr}
	ldr	r2, =0x24a
	ldr	r3, =gState
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0xa
	beq	.L1680
	bl	__CutsceneStart
	mov	r0, #0
	ldr	r1, =0x1b333
	ldr	r2, =0xd999
	bl	__MapActor_SetSpeed
	mov	r0, #0xa
	ldr	r1, =0x1b333
	ldr	r2, =0xd999
	bl	__MapActor_SetSpeed
	mov	r0, #0xbc
	bl	__PlaySound
	mov	r0, #0
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L1636
	mov	r3, #0xa
	ldrsh	r1, [r0, r3]
	mov	r3, #0x12
	ldrsh	r2, [r0, r3]
	mov	r0, #0xa
	bl	__MapActor_TravelTo
.L1636:
	mov	r0, #0xa
	bl	__MapActor_WaitMovement
	mov	r1, #0
	mov	r2, #0x18
	mov	r0, #0
	bl	__Func_809228c
	mov	r0, #4
	bl	__CutsceneWait
	mov	r0, #0xbc
	bl	__PlaySound
	mov	r1, #0
	mov	r2, #0x10
	mov	r0, #0xa
	bl	__Func_809228c
	mov	r0, #0
	bl	__MapActor_WaitMovement
	mov	r1, #0x84
	mov	r2, #0xb4
	lsl	r1, #1
	lsl	r2, #1
	mov	r0, #0xa
	bl	__MapActor_TravelTo
	mov	r0, #0xa
	bl	__MapActor_WaitMovement
	mov	r0, #0xa
	bl	__CutsceneWait
	bl	__CutsceneEnd
.L1680:
	pop	{r0}
	bx	r0
.func_end OvlFunc_948_20095f0
