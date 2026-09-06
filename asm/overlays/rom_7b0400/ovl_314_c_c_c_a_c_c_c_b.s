	.include "macros.inc"

@ Leaf helper, 34 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: iwram_1ebc
@ Reads offsets +0x8, +0x10.
.thumb_func_start OvlFunc_925_200b1c0
	push	{r5, r6, lr}
	ldr	r3, =iwram_3001ebc
	mov	r2, r1
	asr	r2, #20
	ldr	r1, [r3]
	mov	r3, #0x40
	sub	r2, r3, r2
	mov	r6, r2
	mov	r5, r2
	mov	r4, #0
	add	r6, #8
	add	r5, #0xb
	add	r1, #0x14
.L31da:
	ldmia	r1!, {r3}
	cmp	r3, #0
	beq	.L31f8
	ldr	r2, [r3, #8]
	ldr	r3, [r3, #0x10]
	asr	r2, #20
	sub	r2, #4
	asr	r3, #20
	cmp	r2, #4
	bhi	.L31f8
	cmp	r6, r3
	bgt	.L31f8
	cmp	r3, r5
	bge	.L31f8
	stmia	r0!, {r4}
.L31f8:
	add	r4, #1
	cmp	r4, #0x41
	bls	.L31da
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_925_200b1c0

@ 128 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_31c0, GetSlotEntityChecked, PlaySound, GetSlotEntityChecked x3
@   WaitFrames, GetSlotEntityChecked
.thumb_func_start OvlFunc_925_200b208
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e70
	mov	r1, #0xb2
	ldr	r3, [r3]
	lsl	r1, #1
	add	r1, r3
	sub	sp, #0x18
	mov	r11, r1
	mov	r1, #4
	ldr	r2, =0x1999
	add	r1, sp
	mov	r3, #0
	mov	r8, r1
	mov	r10, r2
	mov	r9, r3
	mov	r2, #0
	mov	r1, #0x42
	mov	r3, r8
.L3238:
	add	r2, #1
	stmia	r3!, {r1}
	cmp	r2, #4
	bls	.L3238
	mov	r2, r11
	ldr	r1, [r2, #0xc]
	mov	r0, r8
	bl	OvlFunc_925_200b1c0
	mov	r1, r8
	ldr	r3, [r1]
	mov	r2, #0
	cmp	r3, #0x42
	beq	.L327c
	mov	r6, #0
	mov	r5, #0
.L3258:
	mov	r3, r8
	ldr	r0, [r5, r3]
	str	r2, [sp]
	bl	__MapActor_GetActor
	ldr	r2, [sp]
	add	r0, #0x55
	mov	r1, #1
	add	r2, #1
	strb	r6, [r0]
	add	r9, r1
	add	r5, #4
	cmp	r2, #4
	bhi	.L327c
	mov	r1, r8
	ldr	r3, [r5, r1]
	cmp	r3, #0x42
	bne	.L3258
.L327c:
	mov	r0, #0xdf
	bl	__PlaySound
	mov	r2, #0
.L3284:
	mov	r1, r11
	ldr	r3, [r1, #0xc]
	mov	r1, r10
	sub	r3, r1
	mov	r7, #0
	mov	r1, r11
	str	r3, [r1, #0xc]
	cmp	r7, r9
	bcs	.L32c0
	mov	r6, r8
.L3298:
	ldr	r0, [r6]
	str	r2, [sp]
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	add	r3, r10
	str	r3, [r0, #0x10]
	ldr	r0, [r6]
	bl	__MapActor_GetActor
	mov	r5, r0
	ldmia	r6!, {r0}
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	add	r7, #1
	str	r3, [r5, #0x40]
	ldr	r2, [sp]
	cmp	r7, r9
	bcc	.L3298
.L32c0:
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	bne	.L32cc
	ldr	r3, =0x1999
	add	r10, r3
.L32cc:
	ldr	r1, =0x17fff
	cmp	r10, r1
	ble	.L32d8
	mov	r3, #0xc0
	lsl	r3, #9
	mov	r10, r3
.L32d8:
	mov	r0, #1
	str	r2, [sp]
	bl	__WaitFrames
	ldr	r2, [sp]
	add	r2, #1
	cmp	r2, #0xe3
	bls	.L3284
	mov	r2, #0
	cmp	r2, r9
	bcs	.L3306
	mov	r6, #0
	mov	r5, r8
.L32f2:
	ldmia	r5!, {r0}
	str	r2, [sp]
	bl	__MapActor_GetActor
	ldr	r2, [sp]
	add	r0, #0x55
	add	r2, #1
	strb	r6, [r0]
	cmp	r2, r9
	bcc	.L32f2
.L3306:
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_925_200b208

@ 121 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   OvlFunc_31c0, GetSlotEntityChecked, PlaySound, GetSlotEntityChecked x3
@   WaitFrames, UpdateMapView, WaitFrames
.thumb_func_start OvlFunc_925_200b324
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001e70
	mov	r1, #0xb2
	ldr	r3, [r3]
	lsl	r1, #1
	add	r1, r3
	sub	sp, #0x18
	mov	r11, r1
	add	r1, sp, #4
	mov	r2, #0xc0
	lsl	r2, #9
	mov	r3, #0
	mov	r8, r1
	mov	r10, r2
	mov	r9, r3
	mov	r2, #0
	mov	r1, #0x42
	mov	r3, r8
.L3354:
	add	r2, #1
	stmia	r3!, {r1}
	cmp	r2, #4
	bls	.L3354
	mov	r2, r11
	ldr	r1, [r2, #0xc]
	mov	r0, r8
	bl	OvlFunc_925_200b1c0
	mov	r1, r8
	ldr	r3, [r1]
	mov	r2, #0
	cmp	r3, #0x42
	beq	.L3398
	mov	r6, #0
	mov	r5, #0
.L3374:
	mov	r3, r8
	ldr	r0, [r5, r3]
	str	r2, [sp]
	bl	__MapActor_GetActor
	ldr	r2, [sp]
	add	r0, #0x55
	mov	r1, #1
	add	r2, #1
	strb	r6, [r0]
	add	r9, r1
	add	r5, #4
	cmp	r2, #4
	bhi	.L3398
	mov	r1, r8
	ldr	r3, [r5, r1]
	cmp	r3, #0x42
	bne	.L3374
.L3398:
	mov	r0, #0xdf
	bl	__PlaySound
	mov	r2, #0
.L33a0:
	mov	r1, r11
	ldr	r3, [r1, #0xc]
	mov	r7, #0
	add	r3, r10
	str	r3, [r1, #0xc]
	cmp	r7, r9
	bcs	.L33da
	mov	r6, r8
.L33b0:
	ldr	r0, [r6]
	str	r2, [sp]
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	mov	r1, r10
	sub	r3, r1
	str	r3, [r0, #0x10]
	ldr	r0, [r6]
	bl	__MapActor_GetActor
	mov	r5, r0
	ldmia	r6!, {r0}
	bl	__MapActor_GetActor
	ldr	r3, [r0, #0x10]
	add	r7, #1
	str	r3, [r5, #0x40]
	ldr	r2, [sp]
	cmp	r7, r9
	bcc	.L33b0
.L33da:
	mov	r3, #3
	and	r3, r2
	cmp	r3, #3
	bne	.L33ea
	cmp	r2, #0x4b
	bls	.L33ea
	ldr	r3, =0xffffcccd
	add	r10, r3
.L33ea:
	ldr	r1, =0xccb
	cmp	r10, r1
	bgt	.L33f4
	ldr	r3, =0xccc
	mov	r10, r3
.L33f4:
	mov	r0, #1
	str	r2, [sp]
	bl	__WaitFrames
	ldr	r2, [sp]
	add	r2, #1
	cmp	r2, #0x55
	bls	.L33a0
	mov	r3, #0x80
	lsl	r3, #19
	mov	r1, r11
	str	r3, [r1, #0xc]
	bl	__Func_800fe9c
	mov	r0, #2
	bl	__WaitFrames
	add	sp, #0x18
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_925_200b324

