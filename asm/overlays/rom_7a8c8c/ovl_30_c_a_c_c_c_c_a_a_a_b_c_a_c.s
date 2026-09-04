	.include "macros.inc"

@ 329 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   CopyMapRectAttributes, TestSaveBit, OvlFunc_1004, CopyMapRectAttributes
@   TestSaveBit, OvlFunc_1004, CopyMapRectAttributes x3, OvlFunc_1004
@   CopyMapRectAttributes, TestSaveBit, OvlFunc_1004, CopyMapRectAttributes
@   OvlFunc_1004, CopyMapRectAttributes
@   ... and 26 more
@ reads save bits 0x308, 0x309, 0x30a, 0x30b, 0x30c.
.thumb_func_start OvlFunc_922_20092cc
	push	{r5, lr}
	sub	sp, #8
	mov	r3, #0xc
	mov	r2, #0x15
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xc
	mov	r1, #3
	mov	r2, #9
	mov	r3, #0x10
	bl	__Func_8010704
	mov	r0, #0xc2
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1310
	mov	r0, #8
	mov	r1, #0xe
	mov	r2, #0x19
	bl	OvlFunc_922_2009004
	mov	r3, #0x14
	mov	r2, #0x18
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x10
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
	b	.L137c
.L1310:
	ldr	r0, =0x309
	bl	__GetFlag
	cmp	r0, #0
	beq	.L135e
	mov	r0, #8
	mov	r1, #0x11
	mov	r2, #0x19
	bl	OvlFunc_922_2009004
	mov	r3, #0x14
	mov	r5, #0x18
	str	r3, [sp]
	mov	r0, #0x12
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #3
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0xe
	str	r3, [sp]
	mov	r0, #0x12
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #3
	str	r5, [sp, #4]
	bl	__Func_8010704
	mov	r3, #0x11
	str	r3, [sp]
	mov	r0, #8
	mov	r1, #0x29
	mov	r2, #1
	mov	r3, #3
	str	r5, [sp, #4]
	bl	__Func_8010704
	b	.L137c
.L135e:
	mov	r0, #8
	mov	r1, #0x14
	mov	r2, #0x19
	bl	OvlFunc_922_2009004
	mov	r3, #0xe
	mov	r2, #0x18
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x10
	mov	r1, #0x18
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
.L137c:
	ldr	r0, =0x30a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L13a6
	mov	r0, #9
	mov	r1, #0xd
	mov	r2, #0x23
	bl	OvlFunc_922_2009004
	mov	r3, #0xf
	mov	r2, #0x22
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
	b	.L13c4
.L13a6:
	mov	r0, #9
	mov	r1, #0xf
	mov	r2, #0x23
	bl	OvlFunc_922_2009004
	mov	r3, #0xd
	mov	r2, #0x22
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x22
	mov	r2, #1
	mov	r3, #3
	bl	__Func_8010704
.L13c4:
	ldr	r0, =0x30b
	bl	__GetFlag
	cmp	r0, #0
	beq	.L13f6
	mov	r0, #0xa
	mov	r1, #0xf
	mov	r2, #0x16
	bl	OvlFunc_922_2009004
	mov	r3, #0x1e
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x16
	str	r3, [sp, #4]
	mov	r0, #5
	mov	r1, #0x29
	b	.L14c0
.L13f6:
	mov	r0, #0xc3
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L143a
	mov	r0, #0xa
	mov	r1, #0xf
	mov	r2, #0x17
	bl	OvlFunc_922_2009004
	mov	r3, #0x17
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #5
	mov	r1, #0x2a
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1e
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x15
	str	r3, [sp, #4]
	mov	r0, #0xa
	b	.L14be
.L143a:
	ldr	r0, =0x30d
	bl	__GetFlag
	cmp	r0, #0
	beq	.L147e
	mov	r0, #0xa
	mov	r1, #0xf
	mov	r2, #0x1a
	bl	OvlFunc_922_2009004
	mov	r3, #0x16
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1a
	str	r3, [sp, #4]
	mov	r0, #5
	mov	r1, #0x2b
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1e
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	b	.L14c0
.L147e:
	ldr	r0, =0x30e
	bl	__GetFlag
	cmp	r0, #0
	beq	.L14cc
	mov	r0, #0xa
	mov	r1, #0xf
	mov	r2, #0x1b
	bl	OvlFunc_922_2009004
	mov	r3, #0x16
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1e
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1b
	str	r3, [sp, #4]
	mov	r0, #5
.L14be:
	mov	r1, #0x2c
.L14c0:
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	b	.L14d6
.L14cc:
	mov	r0, #0xa
	mov	r1, #0xf
	mov	r2, #0x1e
	bl	OvlFunc_922_2009004
.L14d6:
	ldr	r0, =0x30f
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1508
	mov	r0, #0xb
	mov	r1, #0xf
	mov	r2, #0x17
	bl	OvlFunc_922_2009004
	mov	r3, #0x1f
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x17
	str	r3, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x28
	b	.L159e
.L1508:
	mov	r0, #0xc4
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L153c
	mov	r0, #0xb
	mov	r1, #0xf
	mov	r2, #0x18
	bl	OvlFunc_922_2009004
	mov	r3, #0x1f
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x18
	str	r3, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x29
	b	.L159e
.L153c:
	ldr	r0, =0x311
	bl	__GetFlag
	cmp	r0, #0
	beq	.L156e
	mov	r0, #0xb
	mov	r1, #0xf
	mov	r2, #0x1b
	bl	OvlFunc_922_2009004
	mov	r3, #0x1f
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1b
	str	r3, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x2a
	b	.L159e
.L156e:
	ldr	r0, =0x312
	bl	__GetFlag
	cmp	r0, #0
	beq	.L15aa
	mov	r0, #0xb
	mov	r1, #0xf
	mov	r2, #0x1c
	bl	OvlFunc_922_2009004
	mov	r3, #0x1f
	mov	r5, #0xe
	str	r3, [sp, #4]
	mov	r0, #0xe
	mov	r1, #0x1d
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	mov	r3, #0x1c
	str	r3, [sp, #4]
	mov	r0, #0xa
	mov	r1, #0x2b
.L159e:
	mov	r2, #3
	mov	r3, #1
	str	r5, [sp]
	bl	__Func_8010704
	b	.L15b4
.L15aa:
	mov	r0, #0xb
	mov	r1, #0xf
	mov	r2, #0x1f
	bl	OvlFunc_922_2009004
.L15b4:
	add	sp, #8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end OvlFunc_922_20092cc
