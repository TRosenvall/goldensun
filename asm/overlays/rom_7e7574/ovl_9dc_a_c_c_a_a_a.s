	.include "macros.inc"

@ 23 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   SetSlotEntitySpeed, MoveSlotTo, WaitForSlotArrival, PlaySound
@   DialogueWait, OvlFunc_b4c, SetSaveBit
@ sets 0x943.
.thumb_func_start OvlFunc_959_2008bac
	push	{lr}
	mov	r1, #0x80
	mov	r2, #0x80
	mov	r0, #0xc
	lsl	r1, #9
	lsl	r2, #8
	bl	__MapActor_SetSpeed
	mov	r2, #0xbc
	mov	r1, #0xf8
	lsl	r2, #1
	mov	r0, #0xc
	bl	__MapActor_TravelTo
	mov	r0, #0xc
	bl	__MapActor_WaitMovement
	mov	r0, #0xd7
	bl	__PlaySound
	mov	r0, #0x3c
	bl	__CutsceneWait
	bl	OvlFunc_959_2008b4c
	ldr	r0, =0x943
	bl	__SetFlag
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2008bac

@ 57 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetMapTransition x2, PlaySound, CopyMapRectAttributes x2
@   GetSlotEntityChecked, SetEntityActorOptions, SetSaveBit
@ sets 0x943.
.thumb_func_start OvlFunc_959_2008bec
	push	{r5, lr}
	mov	r0, #0xc
	sub	sp, #8
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	asr	r3, #20
	cmp	r3, #0x16
	ble	.Lc68
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #11
	lsl	r1, #11
	lsl	r2, #9
	bl	__Func_8012330
	mov	r0, #1
	mov	r1, #1
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0x90
	bl	__PlaySound
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r5, #0xf
	mov	r0, #0xf
	mov	r1, #0x14
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x17
	str	r3, [sp, #4]
	mov	r0, #0x11
	mov	r1, #0x17
	mov	r2, #1
	mov	r3, #3
	str	r5, [sp]
	bl	__Func_8010704
	mov	r0, #0xc
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Lc62
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, r5
	add	r2, #0x23
	mov	r3, #2
	strb	r3, [r2]
.Lc62:
	ldr	r0, =0x943
	bl	__SetFlag
.Lc68:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2008bec

@ 10 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaceSlotAt, OvlFunc_b4c
.thumb_func_start OvlFunc_959_2008c78
	push	{lr}
	mov	r1, #0xf8
	mov	r2, #0xbc
	mov	r0, #0xc
	lsl	r1, #16
	lsl	r2, #17
	bl	__MapActor_SetPos
	bl	OvlFunc_959_2008b4c
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2008c78

@ 35 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectFull x3
.thumb_func_start OvlFunc_959_2008c90
	push	{r5, r6, lr}
	ldr	r3, =.L7714
	lsl	r0, #3
	ldr	r6, [r3, r0]
	add	r0, #4
	ldr	r5, [r3, r0]
	sub	sp, #8
	mov	r0, #0
	mov	r1, #0x4d
	mov	r2, #1
	mov	r3, #3
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_80105d4
	add	r3, r6, #1
	str	r3, [sp]
	mov	r0, #1
	mov	r1, #0x4d
	mov	r2, #1
	mov	r3, #1
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r1, r5
	sub	r1, #0x30
	sub	r5, #0x2e
	mov	r0, r6
	mov	r2, #1
	mov	r3, #1
	str	r6, [sp]
	str	r5, [sp, #4]
	bl	__Func_80105d4
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_959_2008c90
