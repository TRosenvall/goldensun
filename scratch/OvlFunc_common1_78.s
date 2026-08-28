	.include "macros.inc"
	.include "gba.inc"

.thumb_func_start OvlFunc_common1_78
	push	{r5, lr}
	mov	r5, r0
	mov	r0, #0
	bl	__Func_8079664
	mov	r0, #1
	bl	__Func_8079664
	mov	r0, #2
	bl	__Func_8079664
	mov	r0, #3
	bl	__Func_8079664
	mov	r0, #5
	bl	__Func_8079664
	mov	r0, r5
	bl	__AddPartyMember
	ldr	r3, =gState
	mov	r1, #0xfa
	lsl	r1, #1
	add	r3, r1
	str	r5, [r3]
	mov	r0, r5
	mov	r1, #0
	bl	__SetCameraTarget
	mov	r0, r5
	bl	__GetUnit
	mov	r5, r0
	ldrh	r3, [r5, #0x34]
	ldr	r1, =0x131
	strh	r3, [r5, #0x38]
	ldrh	r3, [r5, #0x36]
	ldr	r2, .Lec	@ 0
	strh	r3, [r5, #0x3a]
	add	r3, r5, r1
	strb	r2, [r3]
	mov	r2, #0x38
	ldrsh	r0, [r5, r2]
	mov	r3, #0x34
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	_divsi3_RAM
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.Lf8
	mov	r3, #0
	cmp	r0, #0
	blt	.Lf8
	mov	r3, r0
	b	.Lf8

	.align	2, 0
.Lec:
	.word	0
	.pool

.Lf8:
	strh	r3, [r5, #0x14]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L10c
	mov	r1, #0x38
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L10c
	mov	r3, #1
	strh	r3, [r5, #0x14]
.L10c:
	mov	r2, #0x3a
	ldrsh	r0, [r5, r2]
	mov	r3, #0x36
	ldrsh	r1, [r5, r3]
	lsl	r0, #14
	bl	_divsi3_RAM
	mov	r3, #0x80
	lsl	r3, #7
	cmp	r0, r3
	bgt	.L12a
	mov	r3, #0
	cmp	r0, #0
	blt	.L12a
	mov	r3, r0
.L12a:
	strh	r3, [r5, #0x16]
	lsl	r3, #16
	cmp	r3, #0
	bne	.L13e
	mov	r1, #0x3a
	ldrsh	r3, [r5, r1]
	cmp	r3, #0
	beq	.L13e
	mov	r3, #1
	strh	r3, [r5, #0x16]
.L13e:
	bl	__Func_8091858
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_common1_78
