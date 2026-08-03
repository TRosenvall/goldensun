	.include "macros.inc"

@ RevealSlotF
@ Takes no arguments. Teleports slot 8 to the origin, sets save bit 0x883, then
@ after a forty-frame beat brings slot 0x0F into view: animation 2, its flag
@ byte +0x55 cleared, bit 1 set in +0x23, draw priority 2, and a 1x1 attribute
@ cell repainted from (0, 0) to (0x0E, 0x12).
.thumb_func_start OvlFunc_920_2008214
	push	{lr}
	sub	sp, #8
	bl	__CutsceneStart
	mov	r2, #0
	mov	r1, #0
	mov	r0, #8
	bl	__MapActor_SetPos
	ldr	r0, =0x883
	bl	__SetFlag
	mov	r0, #0x28
	bl	__CutsceneWait
	mov	r1, #2
	mov	r0, #0xf
	bl	__MapActor_DoAnim
	mov	r0, #0xf
	bl	__MapActor_GetActor
	mov	r3, #0
	add	r0, #0x55
	strb	r3, [r0]
	mov	r0, #0xf
	bl	__MapActor_GetActor
	add	r0, #0x23
	ldrb	r2, [r0]
	mov	r3, #2
	orr	r3, r2
	strb	r3, [r0]
	mov	r1, #2
	mov	r0, #0xf
	bl	__Func_8092b08
	mov	r3, #0x12
	mov	r2, #0xe
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0
	mov	r1, #0
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	bl	__CutsceneEnd
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_920_2008214
