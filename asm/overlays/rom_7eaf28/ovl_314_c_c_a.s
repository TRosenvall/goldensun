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

@ 91 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   ReadSaveByte, GetSlotEntityChecked x2, BeginCutscene, MoveCameraTo
@   PlaySound, SetEntityActorOptions, WaitFrames, HideScreenOverlay
@   WaitSceneDelay, EndCutscene, SetSaveBit, ReadSaveByte
@   SetSceneTargetA x2
@ sets 0x122.
.thumb_func_start OvlFunc_960_2008c00
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0x86
	lsl	r0, #2
	bl	__GetFlagByte
	ldr	r5, =gState
	mov	r1, #0xfa
	lsl	r1, #1
	add	r5, r1
	mov	r6, r0
	ldr	r0, [r5]
	bl	__MapActor_GetActor
	mov	r7, r0
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r8, r0
	bl	__CutsceneStart
	mov	r0, #1
	mov	r1, #1
	mov	r2, #1
	neg	r2, r2
	mov	r3, #0
	neg	r1, r1
	neg	r0, r0
	bl	__Func_80933f8
	mov	r0, #0xdb
	bl	__PlaySound
	ldr	r0, [r5]
	mov	r1, #0
	bl	__Actor_SetSpriteFlags
	mov	r2, r8
	mov	r3, #0
	add	r2, #0x55
	strb	r3, [r2]
	mov	r2, r7
	add	r2, #0x55
	strb	r3, [r2]
	str	r3, [r7, #0x28]
	add	r2, #0xc
	mov	r3, #1
	strb	r3, [r2]
	mov	r2, r8
	add	r2, #0x61
	strb	r3, [r2]
	ldr	r6, =0x3333
	mov	r5, #0x3b
.Lc6c:
	ldr	r3, [r7, #0x28]
	add	r3, r6
	str	r3, [r7, #0x28]
	mov	r2, r8
	ldr	r3, [r2, #0x28]
	add	r3, r6
	str	r3, [r2, #0x28]
	mov	r0, #1
	sub	r5, #1
	bl	__WaitFrames
	cmp	r5, #0
	bge	.Lc6c
	bl	__MapTransitionOut
	bl	__WaitMapTransition
	bl	__CutsceneEnd
	mov	r0, #0x91
	lsl	r0, #1
	bl	__SetFlag
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa5
	cmp	r2, r3
	bne	.Lcc2
	mov	r0, #0x86
	lsl	r0, #2
	bl	__GetFlagByte
	cmp	r0, #0xb
	bne	.Lcc2
	ldr	r0, =2
	mov	r1, #0x4d
	bl	__SetDestMap
	b	.Lcca
.Lcc2:
	ldr	r0, =2
	mov	r1, #0x1b
	bl	__SetDestMap
.Lcca:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2008c00

@ Leaf helper, 25 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1e40
.thumb_func_start OvlFunc_960_2008ce4
	push	{lr}
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #0x3f
	and	r3, r2
	lsl	r3, #16
	lsr	r2, r3, #16
	cmp	r2, #0x1f
	bls	.Lcfc
	ldr	r3, .Ld14	@ 0x40
	sub	r3, r2
	lsl	r3, #16
.Lcfc:
	lsr	r3, #17
	add	r3, #7
	lsl	r1, r3, #5
	lsl	r2, r3, #10
	orr	r2, r1
	orr	r3, r2
	lsl	r3, #16
	ldr	r2, =0x500019e
	lsr	r3, #16
	strh	r3, [r2]
	b	.Ld20

	.align	2, 0
.Ld14:
	.word	0x40
	.pool

.Ld20:
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2008ce4

@ 64 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, PlaceSlotAt, CopyMapRectAttributes, SetPlayerObjectFields
@   CopyMapRectAttributes x2, UnregisterTask
.thumb_func_start OvlFunc_960_2008d24
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa5
	sub	sp, #8
	cmp	r2, r3
	bne	.Ldae
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, #2
	add	r0, #0x23
	strb	r3, [r0]
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, #3
	add	r0, #0x55
	strb	r3, [r0]
	mov	r1, #0
	mov	r0, #0xe
	mov	r2, #0
	bl	__MapActor_SetPos
	mov	r3, #0xf
	mov	r2, #0x2c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #1
	mov	r0, #0x10
	mov	r1, #0x2c
	mov	r2, #1
	bl	__Func_8010704
	mov	r0, #0x64
	mov	r1, #0
	mov	r2, #0
	bl	__Func_808edac
	mov	r3, #0x7f
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0xc
	mov	r1, #0x47
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r3, #0xc
	mov	r2, #0x47
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x47
	mov	r2, #1
	mov	r3, #1
	mov	r0, #0xb
	bl	__Func_8010704
	ldr	r0, =OvlFunc_960_2008ce4
	bl	__StopTask
	ldr	r3, =.L1a00
	ldrh	r2, [r3]
	ldr	r3, =0x500019e
	strh	r2, [r3]
.Ldae:
	add	sp, #8
	pop	{r1}
	bx	r1
.func_end OvlFunc_960_2008d24

@ 60 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, PlaceSlotAt, CopyMapRectAttributes, SetPlayerObjectFields
@   UpdateObjectAnimation, CopyMapRectAttributes, RegisterTask
.thumb_func_start OvlFunc_960_2008dc8
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa5
	sub	sp, #8
	cmp	r2, r3
	bne	.Le48
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r3, #2
	add	r0, #0x23
	strb	r3, [r0]
	mov	r0, #0xe
	bl	__MapActor_GetActor
	mov	r5, #0
	add	r0, #0x55
	mov	r1, #0xf8
	mov	r2, #0xb2
	strb	r5, [r0]
	lsl	r1, #16
	mov	r0, #0xe
	lsl	r2, #18
	bl	__MapActor_SetPos
	mov	r3, #0xf
	mov	r2, #0x2c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r3, #1
	mov	r0, #0x1f
	mov	r1, #0x5f
	mov	r2, #1
	bl	__Func_8010704
	mov	r1, #1
	mov	r2, #1
	mov	r0, #0x64
	neg	r1, r1
	neg	r2, r2
	bl	__Func_808edac
	bl	__Func_808ee0c
	mov	r3, #0xc
	mov	r2, #0x47
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x7f
	mov	r1, #0x7f
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	mov	r1, #0xc8
	ldr	r0, =OvlFunc_960_2008ce4
	lsl	r1, #4
	bl	__StartTask
.Le48:
	add	sp, #8
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_960_2008dc8
