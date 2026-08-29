	.include "macros.inc"
	.include "gba.inc"

@ ScreenShakeTask
@ Per-frame task. Applies a decaying offset to the background scroll registers,
@ producing the screen shake. The ~190-instruction body is characterised
@ structurally.
.thumb_func_start Func_912b8
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1ee0
	ldr	r1, [r3]
	sub	r3, #0x70
	ldr	r3, [r3]
	add	r3, #0xe4
	mov	r11, r1
	mov	r1, #2
	ldrsh	r2, [r3, r1]
	sub	sp, #0x10
	str	r2, [sp, #8]
	mov	r1, #6
	ldrsh	r2, [r3, r1]
	str	r2, [sp, #4]
	mov	r5, r11
	ldr	r3, [r5, #0x18]
	cmp	r3, #0
	bne	.L912ea
	b	.L91480
.L912ea:
	ldr	r2, [r3, #0x10]
	mov	r10, r2
	mov	r2, #0x16
	ldrsh	r1, [r3, r2]
	ldr	r7, [r3, #8]
	str	r1, [sp]
	add	r3, #0x22
	ldrb	r3, [r3]
	mov	r1, #0xbd
	mov	r8, r3
	ldr	r3, =iwram_1b10
	lsl	r1, #1
	ldr	r2, =0xfff80000
	add	r3, r1
	add	r7, r2
	ldrh	r3, [r3]
	mov	r2, #0x80
	lsl	r2, #13
	lsr	r3, #5
	add	r2, r10
	mov	r1, r7
	mov	r0, r8
	mov	r9, r3
	bl	_Func_11f54
	mov	r2, #0x80
	lsl	r2, #14
	asr	r6, r0, #16
	add	r2, r10
	mov	r0, r8
	mov	r1, r7
	bl	_Func_11f54
	asr	r0, #16
	sub	r0, #0x10
	cmp	r0, r6
	ble	.L91336
	mov	r6, r0
.L91336:
	cmp	r6, #0
	ble	.L913c8
	ldr	r3, [sp]
	cmp	r6, r3
	ble	.L913c8
	ldr	r3, =0x40000800
	mov	r1, #0xd
	str	r3, [r5, #4]
	mov	r3, #0x80
	ldrb	r2, [r5, #9]
	lsl	r3, #3
	neg	r1, r1
	str	r3, [r5, #8]
	mov	r3, r1
	and	r3, r2
	strb	r3, [r5, #9]
	ldr	r3, .L9138c	@ 0x3ff
	mov	r2, r9
	and	r2, r3
	ldrh	r0, [r5, #8]
	ldr	r3, =0xfffffc00
	and	r3, r0
	orr	r3, r2
	strh	r3, [r5, #8]
	ldrb	r3, [r5, #5]
	and	r1, r3
	mov	r3, #4
	orr	r1, r3
	strb	r1, [r5, #5]
	ldr	r3, .L91390	@ 0xfff0
	ldr	r1, [sp, #8]
	asr	r2, r7, #16
	and	r2, r3
	ldr	r3, .L91394	@ 0x1ff
	sub	r2, r1
	and	r2, r3
	ldrh	r1, [r5, #6]
	ldr	r3, =0xfffffe00
	and	r3, r1
	orr	r3, r2
	strh	r3, [r5, #6]
	mov	r2, r10
	b	.L913b0

	.align	2, 0
.L9138c:
	.word	0x3ff
.L91390:
	.word	0xfff0
.L91394:
	.word	0x1ff
	.pool

.L913b0:
	ldr	r1, [sp, #4]
	asr	r3, r2, #16
	mov	r2, #0xf0
	and	r3, r2
	sub	r3, r1
	sub	r3, r6
	add	r3, #0x10
	strb	r3, [r5, #4]
	mov	r0, r5
	mov	r1, #0
	bl	Func_3dec
.L913c8:
	mov	r2, #0x80
	lsl	r2, #13
	add	r7, r2
	mov	r1, r7
	add	r2, r10
	mov	r0, r8
	bl	_Func_11f54
	mov	r2, #0x80
	lsl	r2, #14
	asr	r6, r0, #16
	add	r2, r10
	mov	r0, r8
	mov	r1, r7
	bl	_Func_11f54
	asr	r0, #16
	mov	r5, r11
	sub	r0, #0x10
	add	r5, #0xc
	cmp	r0, r6
	ble	.L913f6
	mov	r6, r0
.L913f6:
	cmp	r6, #0
	ble	.L91480
	ldr	r2, [sp]
	cmp	r6, r2
	ble	.L91480
	ldr	r3, =0x40000800
	mov	r2, #0xd
	ldrb	r1, [r5, #9]
	str	r3, [r5, #4]
	neg	r2, r2
	mov	r3, #0
	str	r3, [r5, #8]
	mov	r3, r2
	and	r3, r1
	strb	r3, [r5, #9]
	ldr	r3, .L9144c	@ 0x3ff
	mov	r1, r9
	and	r1, r3
	mov	r9, r1
	ldr	r3, =0xfffffc00
	ldrh	r1, [r5, #8]
	and	r3, r1
	mov	r1, r9
	orr	r3, r1
	strh	r3, [r5, #8]
	ldrb	r3, [r5, #5]
	and	r2, r3
	mov	r3, #4
	orr	r2, r3
	ldr	r3, .L91450	@ 0xfff0
	strb	r2, [r5, #5]
	asr	r2, r7, #16
	and	r2, r3
	ldr	r3, [sp, #8]
	sub	r2, r3
	ldr	r3, .L91454	@ 0x1ff
	ldrh	r1, [r5, #6]
	and	r2, r3
	ldr	r3, =0xfffffe00
	and	r3, r1
	orr	r3, r2
	b	.L91464

	.align	2, 0
.L9144c:
	.word	0x3ff
.L91450:
	.word	0xfff0
.L91454:
	.word	0x1ff
	.pool

.L91464:
	mov	r1, r10
	strh	r3, [r5, #6]
	mov	r2, #0xf0
	asr	r3, r1, #16
	and	r3, r2
	ldr	r2, [sp, #4]
	sub	r3, r2
	sub	r3, r6
	add	r3, #0x10
	strb	r3, [r5, #4]
	mov	r0, r5
	mov	r1, #0
	bl	Func_3dec
.L91480:
	add	sp, #0x10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_912b8

@ StartScreenShake
@ r0=intensity. Allocates the 0x1C-byte shake state under tag 0x24, seeds it
@ from r0 and registers Func_912b8.
.thumb_func_start Func_91494
	push	{r5, r6, r7, lr}
	mov	r6, r0
	mov	r1, #0x1c
	mov	r0, #0x24
	sub	sp, #4
	bl	Func_48f4
	mov	r1, #0x80
	mov	r7, r0
	lsl	r1, #3
	mov	r0, #0xe
	bl	Func_48b0
	ldr	r3, =0x11111111
	mov	r4, r0
	mov	r5, sp
	str	r3, [r5]
	mov	r0, r5
	ldr	r3, =REG_DMA3SAD
	mov	r1, r4
	ldr	r2, =0x85000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r1, #0x80
	mov	r2, r4
	lsl	r1, #2
	mov	r0, #0x5e
	bl	Func_3fa4
	mov	r0, #0xe
	bl	Func_2dd8
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_912b8
	bl	Func_41d8
	ldr	r2, =0x3f9e
	ldr	r3, =REG_BLDCNT
	strh	r2, [r3]
	mov	r2, #0x10
	add	r3, #2
	strh	r2, [r3]
	mov	r2, #0x1f
	add	r3, #2
	strh	r2, [r3]
	mov	r3, #0
	str	r3, [r5]
	mov	r0, r5
	ldr	r3, =REG_DMA3SAD
	mov	r1, r7
	ldr	r2, =0x85000007
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	cmp	r6, #0
	bne	.L91514
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	bl	Func_8ba1c
	mov	r6, r0
.L91514:
	str	r6, [r7, #0x18]
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_91494

@ SuspendScreenShake
@ Takes no arguments. Pauses the shake task with Func_42c8, leaving the current
@ offset applied.
.thumb_func_start Func_91540
	push	{lr}
	ldr	r0, =Func_912b8
	bl	Func_42c8
	pop	{r0}
	bx	r0
.func_end Func_91540

@ ResumeScreenShake
@ Takes no arguments. Resumes the shake task with Func_439c.
.thumb_func_start Func_91550
	push	{lr}
	ldr	r0, =Func_912b8
	bl	Func_439c
	pop	{r0}
	bx	r0
.func_end Func_91550

@ FindSpeakerRecord
@ r0=speaker id. Searches the table at .L9e9f0 for the record matching the id
@ and returns it. Func_915ac and Func_915dc read the portrait and voice bytes
@ from the result.
.thumb_func_start Func_91560
	push	{lr}
	mov	r2, r0
	ldr	r0, =.L9e9f0
	ldrh	r3, [r0]
	mov	r1, #0
	cmp	r3, r2
	beq	.L9157c
.L9156e:
	add	r1, #1
	add	r0, #4
	cmp	r1, #0x81
	bhi	.L9157c
	ldrh	r3, [r0]
	cmp	r3, r2
	bne	.L9156e
.L9157c:
	pop	{r1}
	bx	r1
.func_end Func_91560

	.section .rodata

@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e680
L9e680:
.L9e680:
	.incrom 0x9e680, 0x9e686
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e686
L9e686:
.L9e686:
	.incrom 0x9e686, 0x9e6b8
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e6b8
L9e6b8:
.L9e6b8:
	.incrom 0x9e6b8, 0x9e6c0
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e6c0
L9e6c0:
.L9e6c0:
	.incrom 0x9e6c0, 0x9e75c
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e75c
L9e75c:
.L9e75c:
	.incrom 0x9e75c, 0x9e87c
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e87c
L9e87c:
.L9e87c:
	.incrom 0x9e87c, 0x9e8a0
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e8a0
L9e8a0:
.L9e8a0:
	.incrom 0x9e8a0, 0x9e8ac
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e8ac
L9e8ac:
.L9e8ac:
	.incrom 0x9e8ac, 0x9e8ce
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e8ce
L9e8ce:
.L9e8ce:
	.incrom 0x9e8ce, 0x9e8ee
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e8ee
L9e8ee:
.L9e8ee:
	.incrom 0x9e8ee, 0x9e92e
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e92e
L9e92e:
.L9e92e:
	.incrom 0x9e92e, 0x9e96e
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e96e
L9e96e:
.L9e96e:
	.incrom 0x9e96e, 0x9e9ae
@ PROMOTED: referenced from rom_8d9a4.s across the split
	.global	L9e9ae
L9e9ae:
.L9e9ae:
	.incrom 0x9e9ae, 0x9e9f0
.L9e9f0:
	.incrom 0x9e9f0, 0x9ebfc
