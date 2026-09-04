	.include "macros.inc"
	.include "gba.inc"

@ 25 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random
.thumb_func_start OvlFunc_932_20086a0
	push	{r5, r6, lr}
	mov	r6, #0x80
	lsl	r6, #19
	ldrh	r2, [r6]
	ldr	r3, =0xfdff
	and	r3, r2
	lsl	r3, #16
	asr	r5, r3, #16
	bl	__Random
	mov	r3, #0x64
	mul	r3, r0
	ldr	r2, =.L5238
	ldrh	r2, [r2]
	lsr	r3, #16
	cmp	r3, r2
	bcc	.L6c8
	mov	r3, #0x80
	lsl	r3, #2
	orr	r5, r3
.L6c8:
	lsl	r3, r5, #16
	lsr	r3, #16
	strh	r3, [r6]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_20086a0

@ 96 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   PlaySound, SetMapTransition, DialogueWait, WaitFrames
@   GetSlotEntityChecked x2, PlaceSlotAt, SetSlotScriptWithTurn, SetInterruptSource
@   WaitFrames x2, SetInterruptSource, PlaySound, SetMapTransition
@   DialogueWait, CopyMapRectAttributes
@   ... and 1 more
@ sets 0x8fd.
.thumb_func_start OvlFunc_932_20086dc
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e70
	mov	r0, #0xe6
	ldr	r5, [r3]
	sub	sp, #8
	bl	__PlaySound
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r2, #9
	lsl	r0, #10
	lsl	r1, #10
	bl	__Func_8012330
	mov	r0, #0xa
	bl	__CutsceneWait
	mov	r2, #0xb2
	lsl	r2, #1
	ldr	r7, =0x1999
	add	r6, r5, r2
	mov	r5, #0
.L70a:
	ldr	r3, [r6, #0xc]
	ldr	r2, =0xffff0000
	add	r3, r2
	str	r3, [r6, #0xc]
	mov	r0, #4
	bl	__WaitFrames
	cmp	r5, #8
	bne	.L742
	mov	r0, #8
	bl	__MapActor_GetActor
	str	r7, [r0, #0x18]
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r1, #0x98
	mov	r2, #0xd8
	str	r7, [r0, #0x1c]
	lsl	r1, #16
	mov	r0, #8
	lsl	r2, #16
	bl	__MapActor_SetPos
	mov	r0, #8
	ldr	r1, =gScript_932__0200bd48
	bl	__MapActor_SetBehavior
.L742:
	add	r5, #1
	cmp	r5, #0x17
	ble	.L70a
	ldr	r2, =OvlFunc_932_20086a0
	mov	r0, #1
	mov	r1, #0
	bl	__SetIntrHandler
	ldr	r2, =.L5238
	ldr	r3, .L78c	@ 0
	strh	r3, [r2]
	mov	r5, r2
.L75a:
	mov	r0, #1
	bl	__WaitFrames
	ldrh	r3, [r5]
	mov	r2, #0xc8
	add	r3, #1
	strh	r3, [r5]
	lsl	r2, #15
	lsl	r3, #16
	cmp	r3, r2
	bls	.L75a
	mov	r0, #1
	bl	__WaitFrames
	mov	r1, #0
	mov	r2, #0
	mov	r0, #1
	bl	__SetIntrHandler
	ldr	r0, =0x121
	bl	__PlaySound
	mov	r0, #1
	mov	r1, #1
	b	.L7ac

	.align	2, 0
.L78c:
	.word	0
	.pool

.L7ac:
	neg	r1, r1
	ldr	r2, =0xe666
	neg	r0, r0
	bl	__Func_8012330
	mov	r0, #0x1e
	bl	__CutsceneWait
	mov	r3, #3
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0
	mov	r2, #1
	mov	r3, #2
	mov	r0, #0
	bl	__Func_8010704
	ldr	r0, =0x8fd
	bl	__SetFlag
	add	sp, #8
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_932_20086dc
