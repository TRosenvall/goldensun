	.include "macros.inc"
	.include "gba.inc"

@ 213 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectFull x5, CopyMapRectAttributes, CopyMapRectFull x2, CopyMapRectAttributes
@   SetEntityAnimation, CopyMapRectFull x2, SetEntityAnimation, CopyMapRectFull
@   CopyMapRectAttributes, OvlFunc_b3c, CopyMapRectAttributes, OvlFunc_b3c
@   OvlFunc_150
.thumb_func_start OvlFunc_916_2008194
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =.L12c0
	ldr	r6, [r3]
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	sub	sp, #8
	cmp	r3, #0
	beq	.L1d6
	mov	r3, #0x4f
	mov	r2, #0x1d
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x41
	mov	r1, #0x35
	mov	r2, #2
	mov	r3, #1
	bl	__Func_80105d4
	mov	r3, #0xf
	mov	r2, #0x1c
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x41
	mov	r1, #0x28
	mov	r2, #2
	mov	r3, #4
	bl	__Func_80105d4
	b	.L1ea
.L1d6:
	mov	r3, #0x4f
	mov	r2, #0x19
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x41
	mov	r1, #0x32
	mov	r2, #2
	mov	r3, #5
	bl	__Func_80105d4
.L1ea:
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r2, #0
	ldrsh	r5, [r3, r2]
	cmp	r5, #0
	beq	.L22c
	mov	r5, #0
	mov	r3, #0x20
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x20
	str	r3, [sp]
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x40
	str	r3, [sp]
	mov	r0, #0x20
	mov	r1, #0x20
	mov	r2, #0x20
	mov	r3, #0x20
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0
	mov	r1, #0x20
	mov	r2, #0x20
	mov	r3, #0x20
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
	b	.L25e
.L22c:
	mov	r3, #0x20
	mov	r0, #0
	mov	r1, #0x40
	mov	r2, #0x20
	str	r3, [sp]
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r3, #0x40
	str	r3, [sp]
	mov	r0, #0x20
	mov	r1, #0x40
	mov	r2, #0x20
	mov	r3, #0x20
	str	r5, [sp, #4]
	bl	__Func_80105d4
	mov	r0, #0
	mov	r1, #0x40
	mov	r2, #0x20
	mov	r3, #0x20
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__Func_8010704
.L25e:
	mov	r2, #1
	mov	r1, #0
	ldrsh	r3, [r6, r1]
	neg	r2, r2
	cmp	r3, r2
	beq	.L2fa
	mov	r7, #0
	mov	r8, r2
.L26e:
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	ldr	r5, [r6, #8]
	cmp	r3, #1
	bne	.L2d8
	mov	r1, #4
	mov	r0, r5
	bl	__Actor_SetAnim
	mov	r2, r5
	mov	r3, #3
	add	r2, #0x23
	strb	r3, [r2]
	mov	r3, r5
	add	r3, #0x55
	strb	r7, [r3]
	mov	r3, #0xd0
	lsl	r3, #13
	str	r3, [r5, #0xc]
	mov	r1, #6
	ldrsh	r3, [r6, r1]
	cmp	r3, #0
	beq	.L2bc
	mov	r2, #2
	ldrsh	r3, [r6, r2]
	mov	r1, #4
	ldrsh	r2, [r6, r1]
	add	r3, #0x20
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x44
	mov	r1, #0x28
	mov	r2, #1
	mov	r3, #4
	bl	__Func_80105d4
	b	.L2f0
.L2bc:
	mov	r2, #2
	ldrsh	r3, [r6, r2]
	mov	r1, #4
	ldrsh	r2, [r6, r1]
	add	r3, #0x20
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x46
	mov	r1, #0x28
	mov	r2, #4
	mov	r3, #1
	bl	__Func_80105d4
	b	.L2f0
.L2d8:
	mov	r0, r5
	mov	r1, #1
	bl	__Actor_SetAnim
	mov	r2, r5
	add	r2, #0x23
	mov	r3, #1
	strb	r3, [r2]
	add	r2, #0x32
	mov	r3, #2
	strb	r3, [r2]
	str	r7, [r5, #0xc]
.L2f0:
	add	r6, #0xc
	mov	r2, #0
	ldrsh	r3, [r6, r2]
	cmp	r3, r8
	bne	.L26e
.L2fa:
	mov	r3, #0xa
	mov	r2, #0x32
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r1, #0x2a
	mov	r3, #1
	mov	r0, #0x46
	mov	r2, #1
	bl	__Func_80105d4
	ldr	r3, =.L12c8
	ldr	r3, [r3]
	mov	r1, #0
	ldrsh	r3, [r3, r1]
	cmp	r3, #1
	bne	.L338
	mov	r3, #0
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #0x20
	mov	r3, #0x20
	mov	r2, #0x20
	bl	__Func_8010704
	ldr	r3, =.L12c0
	mov	r1, #0xfe
	ldr	r0, [r3]
	bl	OvlFunc_916_2008b3c
	b	.L354
.L338:
	mov	r3, #0
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, #0
	mov	r1, #0x40
	mov	r3, #0x20
	mov	r2, #0x20
	bl	__Func_8010704
	ldr	r3, =.L12c0
	mov	r1, #0xff
	ldr	r0, [r3]
	bl	OvlFunc_916_2008b3c
.L354:
	bl	OvlFunc_916_2008150
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_916_2008194
