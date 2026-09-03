	.include "macros.inc"
	.include "gba.inc"

@ DrawLevelLine
@ r0 = window, r1 = line, r2 = 0 to skip past line 3.
@ Line 1 is the experience-to-next-level readout: the character's level is the
@ byte at record+0x0F, and _Func_79008(id, level + 1) gives the threshold, from
@ which the current total at record+0x124 is subtracted and the difference
@ registered with _Func_19908.
@
@ A level of 0x63 -- NINETY-NINE, the cap -- switches to line 8 instead, which
@ is the "maximum" text. Whichever line is chosen renders as string 0xBE6 + line
@ into a 0x100 scratch and out through _Func_17aa4.
.thumb_func_start Func_80a8578  @ 0x080a8578
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	mov	r8, r0
	mov	r7, r1
	ldr	r3, [r3]
	cmp	r2, #0
	bne	.La8590
	cmp	r7, #3
	ble	.La8590
	add	r7, #1
.La8590:
	cmp	r7, #1
	bne	.La85c4
	ldr	r2, =0x21a
	add	r6, r3, r2
	ldrb	r0, [r6]
	bl	_GetUnit
	mov	r5, r0
	ldrb	r3, [r5, #0xf]
	cmp	r3, #0x63
	bne	.La85aa
	mov	r7, #8
	b	.La85c4
.La85aa:
	ldrb	r1, [r5, #0xf]
	ldrb	r0, [r6]
	add	r1, #1
	bl	_Func_8079008
	mov	r2, #0x92
	lsl	r2, #1
	add	r3, r5, r2
	ldr	r3, [r3]
	mov	r1, #5
	sub	r0, r3
	bl	_Func_8019908
.La85c4:
	mov	r0, #0x80
	lsl	r0, #1
	bl	Func_8004938
	mov	r5, r0
	ldr	r0, =0xbe6
	mov	r1, r5
	add	r0, r7, r0
	mov	r2, #0x80
	bl	_Func_801965c
	mov	r3, #1
	mov	r0, r5
	neg	r3, r3
	mov	r1, r8
	mov	r2, #0
	bl	_Func_8017aa4
	mov	r0, r5
	bl	free
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a8578

	.section .rodata
	.global .Laf2fc

.Laf2fc:
	.incrom 0xaf2fc, 0xaf304
