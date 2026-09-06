	.include "macros.inc"

@ 43 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes x2, SetPlayerObjectFields x2, PlaceSlotAt x2
.thumb_func_start OvlFunc_964_2009fdc
	push	{r5, lr}
	sub	sp, #8
	mov	r3, #8
	str	r3, [sp]
	mov	r5, #0x31
	mov	r0, #0x48
	mov	r1, #0x31
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x2b
	str	r3, [sp, #4]
	mov	r0, #0x71
	mov	r3, #1
	mov	r1, #0x2b
	mov	r2, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0x64
	mov	r1, #0
	mov	r2, #0
	bl	__Func_808edac
	mov	r0, #0x65
	mov	r1, #0
	mov	r2, #0
	bl	__Func_808edac
	mov	r1, #0x88
	mov	r2, #0xc6
	mov	r0, #0xf
	lsl	r1, #16
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r1, #0xc6
	mov	r2, #0xae
	mov	r0, #0x10
	lsl	r1, #18
	lsl	r2, #18
	bl	__MapActor_SetPos
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_964_2009fdc

@ 43 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes x2, SetPlayerObjectFields x2, PlaceSlotAt x2
.thumb_func_start OvlFunc_964_200a040
	push	{r5, lr}
	sub	sp, #8
	mov	r3, #8
	str	r3, [sp]
	mov	r5, #0x31
	mov	r0, #8
	mov	r1, #0x71
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x2b
	str	r3, [sp, #4]
	mov	r0, #0x31
	mov	r3, #1
	mov	r1, #0x6b
	mov	r2, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r1, #1
	mov	r2, #1
	mov	r0, #0x64
	neg	r1, r1
	neg	r2, r2
	bl	__Func_808edac
	mov	r1, #1
	mov	r2, #1
	mov	r0, #0x65
	neg	r1, r1
	neg	r2, r2
	bl	__Func_808edac
	mov	r0, #0xf
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r0, #0x10
	mov	r1, #0
	mov	r2, #0
	bl	__MapActor_SetPos
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_964_200a040
