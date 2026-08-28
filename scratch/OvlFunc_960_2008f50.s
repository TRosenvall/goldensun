	.include "macros.inc"
	.include "gba.inc"

.thumb_func_start OvlFunc_960_2008f50
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0
	mov	r8, r0
	ldr	r0, =0x301
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf6a
	ldr	r0, =0x206
	bl	__SetFlag
.Lf6a:
	ldr	r0, =0x302
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf7a
	ldr	r0, =0x207
	bl	__SetFlag
.Lf7a:
	ldr	r0, =0x303
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf8c
	mov	r0, #0x82
	lsl	r0, #2
	bl	__SetFlag
.Lf8c:
	mov	r0, #0xc1
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lf9e
	ldr	r0, =0x209
	bl	__SetFlag
.Lf9e:
	ldr	r0, =0x305
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lfae
	ldr	r0, =0x20a
	bl	__SetFlag
.Lfae:
	mov	r7, #0x80
	mov	r6, #8
	lsl	r7, #4
.Lfb4:
	mov	r0, r6
	bl	__MapActor_GetActor
	mov	r5, r0
	cmp	r5, #0
	beq	.Lfd6
	ldr	r0, =0x109
	bl	__GetFlag
	cmp	r0, #0
	bne	.Lfce
	str	r7, [r5, #0x18]
	str	r7, [r5, #0x1c]
.Lfce:
	ldr	r3, [r5, #0x50]
	mov	r2, #0
	add	r3, #0x26
	strb	r2, [r3]
.Lfd6:
	add	r6, #1
	cmp	r6, #0xc
	ble	.Lfb4
	ldr	r6, =gDMATaskCount
	ldr	r5, =REG_IME
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r2, [r6]
	cmp	r2, #0x1f
	bgt	.L1008
	lsl	r3, r2, #1
	add	r3, r2
	lsl	r3, #2
	add	r2, #1
	add	r3, r6
	strh	r2, [r6]
	ldr	r2, =0x3f42
	add	r3, #4
	stmia	r3!, {r2}
	ldr	r2, =REG_BLDCNT
	stmia	r3!, {r2}
	mov	r2, #0x80
	lsl	r2, #10
	str	r2, [r3]
.L1008:
	strh	r1, [r5]
	mov	r0, #0xd0
	lsl	r0, #2
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1020
	mov	r3, #0x10
	mov	r0, #0xf4
	mov	r8, r3
	bl	__Func_8091ff0
.L1020:
	ldrh	r3, [r5]
	mov	r1, r3
	strh	r5, [r5]
	ldrh	r3, [r6]
	cmp	r3, #0x1f
	bgt	.L1050
	lsl	r2, r3, #1
	add	r2, r3
	add	r3, #1
	mov	r0, r8
	strh	r3, [r6]
	mov	r3, #0x10
	lsl	r2, #2
	sub	r3, r0
	add	r2, r6
	lsl	r3, #8
	add	r2, #4
	orr	r3, r0
	stmia	r2!, {r3}
	ldr	r3, =REG_BLDALPHA
	stmia	r2!, {r3}
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r2]
.L1050:
	strh	r1, [r5]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_960_2008f50
