	.include "macros.inc"

@ Leaf helper, 24 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Writes offsets +0x4c.
.thumb_func_start OvlFunc_945_20080fc
	push	{lr}
	ldr	r3, [r0, #0x4c]
	cmp	r3, #0
	beq	.L10a
	sub	r3, #1
	str	r3, [r0, #0x4c]
	b	.L10e
.L10a:
	mov	r0, #1
	b	.L128
.L10e:
	mov	r2, #0x80
	ldr	r3, [r0, #0x38]
	lsl	r2, #24
	cmp	r3, r2
	bne	.L126
	ldr	r2, [r0, #0x3c]
	cmp	r2, r3
	bne	.L126
	ldr	r3, [r0, #0x40]
	mov	r0, #1
	cmp	r3, r2
	beq	.L128
.L126:
	mov	r0, #0
.L128:
	pop	{r1}
	bx	r1
.func_end OvlFunc_945_20080fc

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, SetEntityAnimation, SetEntityMoveTarget, OvlFunc_fc
@   SetEntityAnimation x3, SetEntityMoveTarget, OvlFunc_fc, SetEntityAnimation
@   OvlFunc_d8
.thumb_func_start OvlFunc_945_200812c
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r0, #8
	bl	__MapActor_GetActor
	mov	r6, r5
	add	r6, #0x66
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	mov	r7, r0
	cmp	r3, #0xc
	bls	.L146
	b	.L27c
.L146:
	ldr	r2, =.L150
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L150:
	.word	.L184
	.word	.L270
	.word	.L18c
	.word	.L270
	.word	.L192
	.word	.L1b6
	.word	.L270
	.word	.L1e6
	.word	.L270
	.word	.L218
	.word	.L250
	.word	.L270
	.word	.L278
.L184:
	mov	r3, #0xb0
	lsl	r3, #8
	strh	r3, [r5, #6]
	b	.L200
.L18c:
	mov	r3, #0
	strh	r3, [r5, #6]
	b	.L200
.L192:
	mov	r0, r5
	mov	r1, #2
	bl	__Actor_SetAnim
	mov	r1, #0xea
	mov	r2, #0x80
	mov	r3, #0x9e
	lsl	r3, #18
	mov	r0, r5
	lsl	r1, #17
	lsl	r2, #14
	bl	__Actor_TravelTo
	mov	r3, #0x3c
	str	r3, [r5, #0x4c]
	ldrh	r3, [r6]
	add	r3, #1
	b	.L27a
.L1b6:
	mov	r0, r5
	bl	OvlFunc_945_20080fc
	cmp	r0, #0
	beq	.L27c
	mov	r0, r5
	mov	r1, #1
	bl	__Actor_SetAnim
	mov	r2, r5
	mov	r3, #0
	add	r2, #0x62
	strb	r3, [r2]
	mov	r3, r7
	add	r3, #0x5b
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L1e0
	add	r2, #1
	mov	r3, #1
	strb	r3, [r2]
.L1e0:
	ldrh	r3, [r6]
	add	r3, #1
	b	.L27a
.L1e6:
	mov	r3, r7
	add	r3, #0x5b
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L200
	mov	r0, r5
	mov	r1, #3
	bl	__Actor_SetAnim
	mov	r2, r5
	add	r2, #0x63
	mov	r3, #2
	strb	r3, [r2]
.L200:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldr	r2, .L210	@ 0
	mov	r3, r5
	add	r3, #0x62
	strb	r2, [r3]
	b	.L27c

	.align	2, 0
.L210:
	.word	0
	.pool

.L218:
	mov	r0, r5
	mov	r1, #2
	bl	__Actor_SetAnim
	mov	r1, #0xf0
	mov	r2, #0x80
	mov	r3, #0x96
	lsl	r3, #18
	mov	r0, r5
	lsl	r1, #17
	lsl	r2, #14
	bl	__Actor_TravelTo
	mov	r3, #0x3c
	str	r3, [r5, #0x4c]
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	mov	r3, r7
	add	r3, #0x5b
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L27c
	mov	r2, r5
	add	r2, #0x63
	mov	r3, #3
	strb	r3, [r2]
	b	.L27c
.L250:
	mov	r0, r5
	bl	OvlFunc_945_20080fc
	cmp	r0, #0
	beq	.L27c
	mov	r0, r5
	mov	r1, #1
	bl	__Actor_SetAnim
	mov	r2, r5
	mov	r3, #0
	add	r2, #0x62
	strb	r3, [r2]
	ldrh	r3, [r6]
	add	r3, #1
	b	.L27a
.L270:
	mov	r0, r5
	bl	OvlFunc_945_20080d8
	b	.L27c
.L278:
	mov	r3, #0
.L27a:
	strh	r3, [r6]
.L27c:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_945_200812c
