	.include "macros.inc"

@ ============================================================================
@ THIS WHOLE FILE RUNS FROM IWRAM, NOT FROM ROM.
@
@ stage1.ld places it with `rom_770 : { ... } > iwram AT > rom`, so the section
@ is LINKED for IWRAM but LOADED from ROM, and Func_300c (rom_2e00.s) DMA3-copies
@ all 0x500 words -- 0x1400 bytes, exactly 0x770..0x1B70 -- to iwram_0 during
@ boot, then points the IRQ vector at it.
@
@ THE Func_<offset> NAMING IS MISLEADING HERE. Those names encode each function's
@ ROM LOAD offset; the code actually executes at IWRAM addresses:
@     Func_770  -> 0x03000000        Func_888  -> 0x03000118
@     Func_1af8 -> 0x03001388
@ Confirmed with `arm-none-eabi-nm goldensun.elf`. This is the only section in
@ the ROM where a symbol's name is not its run address, so do not compute
@ branch targets or PC-relative offsets from the names in this file.
@
@ Everything here is ARM and heavily unrolled precisely because it runs from
@ IWRAM at full speed: the division core every module's arithmetic funnels
@ through, the fixed-point multiply, the fill and copy primitives, the 3D vector
@ transform, and two decompressors.
@ ============================================================================

@ TransformVertexBatch
@ r0 = source vectors, r1 = destination, r2 = count.
@ Transforms a run of 3-vectors by the matrix at Data_ac0 using `smull`/`smlal`
@ accumulation, keeping the middle 32 bits of each 64-bit product -- the 16.16
@ convention used throughout the ROM.
.arm_func_start Func_770
	mov	r3, #0x4000000
	ldr	r2, [r3, #0x200]!
	ldrh	r1, [r3, #8]
	mrs	r0, SPSR
	push	{r0, r1, r2, r3, r4, r5, lr}
	mov	r0, #1
	strh	r0, [r3, #8]
	and	r1, r2, r2, lsr #16
	ands	r0, r1, #2
	ldrne	r12, .L854
	bne	.L7f8
	ands	r0, r1, #1
	ldrne	r12, .L850
	bne	.L7f8
	ands	r0, r1, #4
	ldrne	r12, .L858
	bne	.L7f8
	ands	r0, r1, #0x40
	ldrne	r12, .L868
	bne	.L7f8
	ands	r0, r1, #0x80
	ldrne	r12, .L86c
	bne	.L7f8
	ands	r0, r1, #0x10
	ldrne	r12, .L860
	bne	.L7f8
	ands	r0, r1, #0x20
	ldrne	r12, .L864
	bne	.L7f8
	ands	r0, r1, #0x1000
	ldrne	r12, .L880
	bne	.L7f8
	ands	r0, r1, #0x2000
	ldr	r12, .L884
.L7f8:
	strh	r0, [r3, #2]
	mov	r1, #0x20c0
	bic	r2, r0
	and	r1, r2
	strh	r1, [r3]
	mrs	r3, CPSR
	bic	r3, #0xdf
	orr	r3, #0x1f
	msr	CPSR_fc, r3
	mov	r5, lr
	adr	lr, .L828
	bx	r12
.L828:
	mov	lr, r5
	mrs	r3, CPSR
	bic	r3, #0xdf
	orr	r3, #0x92
	msr	CPSR_fc, r3
	pop	{r0, r1, r2, r3, r4, r5, lr}
	strh	r2, [r3]
	strh	r1, [r3, #8]
	msr	SPSR_fc, r0
	bx	lr

	.global	Data_850
Data_850:
.L850:	.space	4
.L854:	.space	4
.L858:	.space	8
.L860:	.space	4
	.ssize	Data_850

	.global	Data_864
Data_864:
.L864:	.space	4
.L868:	.space	4
.L86c:	.space	0x14
.L880:	.space	4
.L884:	.space	4
	.ssize	Data_864
.func_end Func_770

@ FixedMul -- Thumb-return veneer
@ r0, r1 = 16.16 values. Returns the 16.16 product: `smull` then take the middle
@ 32 bits of the 64-bit result.
@ The tail is `add r12, #1; bx r12` rather than `bx lr` -- it returns to r12
@ with the Thumb bit set, so this is the entry Thumb callers use. Func_89c below
@ is the identical routine returning normally.
.arm_func_start Func_888
	smull	r2, r0, r1, r0
	lsl	r0, #16
	orr	r0, r2, lsr #16
	add	r12, #1
	bx	r12
.func_end Func_888

@ FixedMul
@ r0, r1 = 16.16 values. Returns the 16.16 product. Identical to Func_888 except
@ that it returns through lr; ARM callers use this one.
.arm_func_start Func_89c
	smull	r2, r0, r1, r0
	lsl	r0, #16
	orr	r0, r2, lsr #16
	bx	lr
.func_end Func_89c

@ FixedDiv
@ r0 = numerator, r1 = denominator, both 16.16. Returns the 16.16 quotient.
@ Computes it as a RECIPROCAL MULTIPLY: Func_af0(0x40000000, denominator) gives
@ a scaled reciprocal, which is then multiplied by the numerator and shifted
@ back by 14. Cheaper than a second full division.
.arm_func_start Func_8ac
	stmfd	sp!, {lr}
	mov	r4, r1
	mov	r1, r0
	mov	r0, #0x40000000
	bl	Func_af0
	smull	r3, r0, r4, r0
	ldmfd	sp!, {lr}
	lsl	r0, #18
	orr	r0, r3, lsr #14
	bx	lr
.func_end Func_8ac

@ FillZero
@ r0 = destination, r1 = byte count. `mov r2, #0` and FALLS THROUGH into
@ Func_8d8, so this is exactly Func_8d8(dst, n, 0). The two share one body.
.arm_func_start Func_8d4
	mov	r2, #0
@ Fill
@ r0 = destination, r1 = byte count, r2 = fill value. Writes the value across
@ the range, widening it to a word and using `stm` for the aligned bulk.
@ Func_8d4 above is the zero-fill entry into this same code.
.arm_func_start Func_8d8
	push	{r5, r6, r7, r8, r9}
	mov	r3, r2
	mov	r4, r2
	mov	r5, r2
	mov	r6, r2
	mov	r7, r2
	mov	r8, r2
	mov	r9, r2
	ands	r12, r1, #0xe0
	rsb	r12, #0xe0
	add	pc, r12, lsr #3
.L904:
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	stmia	r0!, {r2, r3, r4, r5, r6, r7, r8, r9}
	subs	r1, #0x100
	bpl	.L904
	ands	r1, #0x1c
	beq	.L940
.L934:
	stmia	r0!, {r2}
	subs	r1, #4
	bgt	.L934
.L940:
	pop	{r5, r6, r7, r8, r9}
	bx	lr
.func_end Func_8d8
.func_end Func_8d4

@ IntegerSqrt
@ r0 = value. Returns floor(sqrt(value)); a negative input returns 0.
@ A binary search from bit 15 downward -- `mov r2, #0x8000` then repeatedly
@ trial-set a bit and back it out when the square overshoots. No table, no
@ division.
.arm_func_start Func_948
	movs	r1, r0
	mov	r0, #0
	bxmi	lr
	mov	r2, #0x8000
.L958:
	add	r0, r2
	mul	r3, r0, r0
	cmp	r3, r1
	subhi	r0, r2
	add	r0, r2, lsr #1
	mul	r3, r0, r0
	cmp	r3, r1
	subhi	r0, r2, lsr #1
	lsrs	r2, #2
	bne	.L958
	bx	lr
.func_end Func_948

@ ExpandBitmapToBytes
@ r0 = 1bpp source, r1 = destination, r2 = the value to write for a set bit.
@ Reads 14 halfwords (the loop bound is r0+0x1C), and for each SET bit writes
@ one byte of r2 to the destination, advancing 0x10 bytes per source halfword.
@ A 1bpp-to-8bpp expander -- the shape a glyph or mask unpacker takes.
.arm_func_start Func_984
	add	r12, r0, #0x1c
.L988:
	mov	r4, r1
	ldrh	r3, [r0], #2
	bic	r3, #3
	lsls	r3, #17
	bcc	.L9a8
.L99c:
	strb	r2, [r4], #1
.L9a0:
	lsls	r3, #1
	bcs	.L99c
.L9a8:
	add	r4, #1
	bne	.L9a0
	add	r1, #0x10
	cmp	r0, r12
	bne	.L988
	bx	lr
.func_end Func_984

@ TransformVector
@ r0 = a 3-vector. Multiplies it by the 3x3 matrix at Data_ac0 with
@ `smull`/`smlal`, keeps the middle 32 bits of each accumulator, and adds the
@ translation row that follows the matrix. The core 3D transform.
.arm_func_start Func_9c0
	push	{r5, r6, r7, r8, r9, r10, r11, lr}
	ldm	r0, {r2, r3, r4}
	adr	r0, Data_ac0
	ldm	r0!, {r5, r6, r7}
	smull	r8, r9, r2, r5
	smull	r10, r11, r2, r6
	smull	r12, r14, r2, r7
	ldm	r0!, {r5, r6, r7}
	smlal	r8, r9, r3, r5
	smlal	r10, r11, r3, r6
	smlal	r12, r14, r3, r7
	ldm	r0!, {r5, r6, r7}
	smlal	r8, r9, r4, r5
	smlal	r10, r11, r4, r6
	smlal	r12, r14, r4, r7
	lsr	r8, #16
	orr	r8, r9, lsl #16
	lsr	r10, #16
	orr	r10, r11, lsl #16
	lsr	r12, #16
	orr	r12, r14, lsl #16
	ldm	r0!, {r5, r6, r7}
	add	r5, r8
	add	r6, r10
	add	r7, r12
	stmia	r1!, {r5, r6, r7}
	pop	{r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr
.func_end Func_9c0

@ TransformAndProject
@ r0 = a 3-vector. Applies Func_9c0's transform and divides through by the depth
@ to produce screen coordinates.
.arm_func_start Func_a30
	push	{r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, #0x24
	mov	r4, sp
.La3c:
	adr	r1, Data_ac0
	ldm	r0!, {r2, r3}
	ldm	r1!, {r5, r6, r7}
	smull	r8, r9, r5, r2
	smull	r10, r11, r6, r2
	smull	r12, r14, r7, r2
	ldm	r1!, {r5, r6, r7}
	smlal	r8, r9, r5, r3
	smlal	r10, r11, r6, r3
	smlal	r12, r14, r7, r3
	ldm	r1!, {r5, r6, r7}
	ldr	r2, [r0], #4
	smlal	r8, r9, r5, r2
	smlal	r10, r11, r6, r2
	smlal	r12, r14, r7, r2
	lsr	r8, #16
	orr	r8, r9, lsl #16
	lsr	r10, #16
	orr	r10, r11, lsl #16
	lsr	r12, #16
	orr	r12, r14, lsl #16
	sub	r2, r4, sp
	cmp	r2, #0x24
	stmneia	r4!, {r8, r10, r12}
	bne	.La3c
	ldm	r1!, {r5, r6, r7}
	add	r12, r7
	add	r11, r10, r6
	add	r10, r8, r5
	pop	{r0, r2, r3, r4, r5, r6, r7, r8, r9}
	stmdb	r1, {r0, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12}
	pop	{r5, r6, r7, r8, r9, r10, r11, lr}
	bx	lr
.func_end Func_a30

	.global	Data_ac0
Data_ac0:
	.word	0x10000, 0, 0, 0
	.word	0x10000, 0, 0, 0
	.word	0x10000, 0, 0, 0
	.ssize	Data_ac0

@ SignedDiv
@ r0 = numerator, r1 = denominator. Returns the QUOTIENT, sign taken from
@ `eor r12, r0, r1` so the result is negative when exactly one input is.
@ Both operands are made positive and handed to Func_b6c, the shared unsigned
@ division core.
.arm_func_start Func_af0
	eor	r12, r0, r1
	movs	r2, r1
	rsbmi	r2, #0
	movs	r1, r0
	rsbmi	r1, #0
	lsrs	r0, r12, #32
	bcc	Func_b6c
	mov	r12, lr
	bl	Func_b6c
	rsb	r0, #0
	bx	r12
.func_end Func_af0

@ SignedRem
@ r0 = numerator, r1 = denominator. Returns the REMAINDER, whose sign follows
@ the DIVIDEND (r0), not the divisor -- C semantics.
@ Same Func_b6c core as Func_af0; the only difference is that this one returns
@ r1 (the remainder) where that one returns r0 (the quotient). Confusing the two
@ is easy and changes behaviour silently, so check which you have.
.arm_func_start Func_b1c
	stmfd	sp!, {lr}
	eor	r12, r0, r1
	movs	r2, r1
	rsbmi	r2, #0
	movs	r1, r0
	rsbmi	r1, #0
	mov	r0, #0
	bl	Func_b6c
	mov	r0, r1
	movs	r12, r12
	rsbmi	r0, #0
	ldmfd	sp!, {lr}
	bx	lr
.func_end Func_b1c

@ UnsignedRem
@ r0 = numerator, r1 = denominator. Returns the remainder, treating both as
@ unsigned. Calls Func_b60 and takes r1.
.arm_func_start Func_b50
	mov	r12, lr
	bl	Func_b60
	mov	r0, r1
	bx	r12
.func_end Func_b50

@ UnsignedDiv
@ r0 = numerator, r1 = denominator. Shuffles the operands and falls into
@ Func_b6c, returning the unsigned quotient.
.arm_func_start Func_b60
	mov	r2, r1
	mov	r1, r0
	mov	r0, #0
@ DivideCore
@ r0 = 0, r1 = numerator, r2 = denominator (as set up by the wrappers).
@ Returns the quotient in r0 and the remainder in r1.
@ A restoring binary long division: shift the divisor up until it exceeds the
@ dividend, then walk back down subtracting and setting quotient bits. No
@ hardware divide on ARM7TDMI, so every division in the ROM ends up here --
@ Func_af0, Func_b1c, Func_b50 and Func_b60 are all thin wrappers around it.
.arm_func_start Func_b6c
	rsbs	r3, r2, r1, lsr #28
	bcc	.Lba4
	rsbs	r3, r2, r1, lsr #31
	orrcs	r0, #0x80000000
	subcs	r1, r2, lsl #31
	rsbs	r3, r2, r1, lsr #30
	orrcs	r0, #0x40000000
	subcs	r1, r2, lsl #30
	rsbs	r3, r2, r1, lsr #29
	orrcs	r0, #0x20000000
	subcs	r1, r2, lsl #29
	rsbs	r3, r2, r1, lsr #28
	orrcs	r0, #0x10000000
	subcs	r1, r2, lsl #28
.Lba4:
	rsbs	r3, r2, r1, lsr #24
	bcc	.Lbdc
	rsbs	r3, r2, r1, lsr #27
	orrcs	r0, #0x8000000
	subcs	r1, r2, lsl #27
	rsbs	r3, r2, r1, lsr #26
	orrcs	r0, #0x4000000
	subcs	r1, r2, lsl #26
	rsbs	r3, r2, r1, lsr #25
	orrcs	r0, #0x2000000
	subcs	r1, r2, lsl #25
	rsbs	r3, r2, r1, lsr #24
	orrcs	r0, #0x1000000
	subcs	r1, r2, lsl #24
.Lbdc:
	rsbs	r3, r2, r1, lsr #20
	bcc	.Lc14
	rsbs	r3, r2, r1, lsr #23
	orrcs	r0, #0x800000
	subcs	r1, r2, lsl #23
	rsbs	r3, r2, r1, lsr #22
	orrcs	r0, #0x400000
	subcs	r1, r2, lsl #22
	rsbs	r3, r2, r1, lsr #21
	orrcs	r0, #0x200000
	subcs	r1, r2, lsl #21
	rsbs	r3, r2, r1, lsr #20
	orrcs	r0, #0x100000
	subcs	r1, r2, lsl #20
.Lc14:
	rsbs	r3, r2, r1, lsr #16
	bcc	.Lc4c
	rsbs	r3, r2, r1, lsr #19
	orrcs	r0, #0x80000
	subcs	r1, r2, lsl #19
	rsbs	r3, r2, r1, lsr #18
	orrcs	r0, #0x40000
	subcs	r1, r2, lsl #18
	rsbs	r3, r2, r1, lsr #17
	orrcs	r0, #0x20000
	subcs	r1, r2, lsl #17
	rsbs	r3, r2, r1, lsr #16
	orrcs	r0, #0x10000
	subcs	r1, r2, lsl #16
.Lc4c:
	rsbs	r3, r2, r1, lsr #12
	bcc	.Lc84
	rsbs	r3, r2, r1, lsr #15
	orrcs	r0, #0x8000
	subcs	r1, r2, lsl #15
	rsbs	r3, r2, r1, lsr #14
	orrcs	r0, #0x4000
	subcs	r1, r2, lsl #14
	rsbs	r3, r2, r1, lsr #13
	orrcs	r0, #0x2000
	subcs	r1, r2, lsl #13
	rsbs	r3, r2, r1, lsr #12
	orrcs	r0, #0x1000
	subcs	r1, r2, lsl #12
.Lc84:
	rsbs	r3, r2, r1, lsr #8
	bcc	.Lcbc
	rsbs	r3, r2, r1, lsr #11
	orrcs	r0, #0x800
	subcs	r1, r2, lsl #11
	rsbs	r3, r2, r1, lsr #10
	orrcs	r0, #0x400
	subcs	r1, r2, lsl #10
	rsbs	r3, r2, r1, lsr #9
	orrcs	r0, #0x200
	subcs	r1, r2, lsl #9
	rsbs	r3, r2, r1, lsr #8
	orrcs	r0, #0x100
	subcs	r1, r2, lsl #8
.Lcbc:
	rsbs	r3, r2, r1, lsr #4
	bcc	.Lcf4
	rsbs	r3, r2, r1, lsr #7
	orrcs	r0, #0x80
	subcs	r1, r2, lsl #7
	rsbs	r3, r2, r1, lsr #6
	orrcs	r0, #0x40
	subcs	r1, r2, lsl #6
	rsbs	r3, r2, r1, lsr #5
	orrcs	r0, #0x20
	subcs	r1, r2, lsl #5
	rsbs	r3, r2, r1, lsr #4
	orrcs	r0, #0x10
	subcs	r1, r2, lsl #4
.Lcf4:
	rsbs	r3, r2, r1
	bxcc	lr
	rsbs	r3, r2, r1, lsr #3
	orrcs	r0, #8
	subcs	r1, r2, lsl #3
	rsbs	r3, r2, r1, lsr #2
	orrcs	r0, #4
	subcs	r1, r2, lsl #2
	rsbs	r3, r2, r1, lsr #1
	orrcs	r0, #2
	subcs	r1, r2, lsl #1
	rsbs	r3, r2, r1
	orrcs	r0, #1
	subcs	r1, r2
	bx	lr
.func_end Func_b6c
.func_end Func_b60

@ DecompressLz
@ r0 = compressed source, r1 = destination.
@ An LZ77-style decoder: a 16-bit length header, then a control bit stream where
@ a clear bit copies a literal byte and a set bit reads a two-byte
@ offset/length pair and copies from earlier in the output.
@ Returns immediately when the length header is zero.
.arm_func_start Func_d30
	ldrb	r2, [r0], #1
	ldrb	r3, [r0], #1
	orrs	r2, r3, lsl #8
	bxeq	lr
	push	{r5, r6, r7}
	mov	r6, r0
	add	r2, r0
	sub	r2, #2
.Ld50:
	ldrb	r3, [r2], #1
	orr	r3, #0x100
.Ld58:
	lsrs	r3, #1
	bcc	.Ld78
	beq	.Ld50
	ldrb	r4, [r0], #1
	subs	r5, r4, #0xdf
	addhi	r1, r5
	strlsb	r4, [r1], #1
	b	.Ld58
.Ld78:
	ldrb	r4, [r2], #1
	ldrb	r12, [r2], #1
	orrs	r4, r12, r4, lsl #8
	beq	.Ldbc
	bic	r12, r4, #0xf000
	sub	r12, r6, r12
	lsrs	r4, #12
	ldreqb	r4, [r2], #1
	addeq	r4, #0x10
	add	r4, #1
.Lda0:
	ldrb	r5, [r12], #1
	subs	r7, r5, #0xdf
	addhi	r1, r7
	strlsb	r5, [r1], #1
	subs	r4, #1
	bpl	.Lda0
	b	.Ld58
.Ldbc:
	pop	{r5, r6, r7}
	mov	r0, #0
	bx	lr
.func_end Func_d30

@ DecompressHuffman
@ r0 = compressed source, r1 = destination. 609 lines -- the largest routine in
@ the file. A Huffman decoder with an inlined bit reader, distinct from the
@ order-1 text decoder in rom_15000 and from Func_d30's LZ. Traced
@ structurally.
.thumb_func_start Func_dc8
	str	r0, [sp, #4]
	ldr	r3, [r4, #0x24]
	ldrb	r6, [r4]
	mov	r0, #0xc7
	tst	r0, r6
	beq	.Le0a
	lsl	r0, r6, #25
	bcc	.Ldf8
	bmi	.Le06
	mov	r6, #3
	mov	r0, r3
	add	r0, #0x10
	str	r0, [r4, #0x28]
	ldr	r0, [r3, #0xc]
	str	r0, [r4, #0x18]
	mov	r5, #0
	strb	r5, [r4, #9]
	str	r5, [r4, #0x1c]
	ldrb	r2, [r3, #3]
	lsr	r0, r2, #6
	beq	.Le46
	mov	r0, #0x10
	orr	r6, r0
	b	.Le46
.Ldf8:
	ldrb	r5, [r4, #9]
	lsl	r0, r6, #29
	bpl	.Le0c
	ldrb	r0, [r4, #0xd]
	sub	r0, #1
	strb	r0, [r4, #0xd]
	bhi	.Le54
.Le06:
	mov	r0, #0
	strb	r0, [r4]
.Le0a:
	b	.L124c
.Le0c:
	lsl	r0, r6, #25
	bpl	.Le28
	ldrb	r0, [r4, #7]
	mul	r5, r0
	lsr	r5, #8
	ldrb	r0, [r4, #0xc]
	cmp	r5, r0
	bhi	.Le54
.Le1c:
	ldrb	r5, [r4, #0xc]
	cmp	r5, #0
	beq	.Le06
	mov	r0, #4
	orr	r6, r0
	b	.Le52
.Le28:
	mov	r2, #3
	and	r2, r6
	cmp	r2, #2
	bne	.Le42
	ldrb	r0, [r4, #5]
	mul	r5, r0
	lsr	r5, #8
	ldrb	r0, [r4, #6]
	cmp	r5, r0
	bhi	.Le54
	mov	r5, r0
	beq	.Le1c
	b	.Le50
.Le42:
	cmp	r2, #3
	bne	.Le54
.Le46:
	ldrb	r0, [r4, #4]
	add	r5, r0
	cmp	r5, #0xff
	bcc	.Le52
	mov	r5, #0xff
.Le50:
	sub	r6, #1
.Le52:
	strb	r6, [r4]
.Le54:
	strb	r5, [r4, #9]
	ldrb	r0, [r4, #2]
	mul	r0, r5
	lsr	r0, #9
	mov	r10, r0
	ldrb	r0, [r4, #3]
	mul	r0, r5
	lsr	r0, #9
	mov	r11, r0
	mov	r0, #0x10
	and	r0, r6
	beq	.Le76
	add	r3, #8
	ldmia	r3!, {r0, r1}
	add	r3, r0, r3
	str	r3, [sp, #0xc]
	sub	r0, r1, r0
.Le76:
	str	r0, [sp, #0x10]
	ldr	r5, =.L1568
	ldr	r2, [r4, #0x18]
	ldr	r3, [r4, #0x28]
	ldrb	r0, [r4, #1]
	lsr	r1, r0, #4
	adr	r1, .Le8c
	bx	r1
	.pool_aligned
.Le8c:
.arm
	mov	r8, #0x160
	orr	r11, r10, r11, lsl #16
	bcs	.L116c
	push	{r4, r9, r12}
	movs	r2, r2
	orreq	r0, #0x40
	streqb	r0, [r4, #1]
	add	r4, #0x1c
	ldm	r4, {r7, r14}
	mul	r4, r12, r14
	ldrsb	r6, [r3], #1
	ldrsb	r12, [r3]
	ands	r0, #0x40
	bne	.L13fc
	sub	r12, r6
	lsrs	r11, #1
	adc	r11, #0x8000
	bic	r11, #0xff00
	mov	r1, r7
	umlal	r1, r0, r4, r8
	lsr	r1, #23
	orr	r0, r1, r0, lsl #9
	cmp	r2, r0
	ble	.L108c
	sub	r2, r0
	ldr	r10, .L103c
	add	r10, r0
	cmp	r10, sp
	add	r10, r3, r0
	adr	r9, .L110c
	stm	r9, {r2, r10, sp}
	cmpcc	r0, #0x400
	bcs	.Lf38
	bic	r1, r3, #3
	mov	r9, #0x4000000
	add	r9, #0xd4
	add	r0, #7
	lsr	r0, #2
	sub	sp, r0, lsl #2
	and	r3, #3
	add	r3, sp
	orr	r14, r0, #0x84000000
	stm	r9, {r1, sp, r14}
.Lf38:
	subs	r4, #0x800000
	lslpl	r11, #1
	adr	r0, .L1044
	addpl	r0, #0x18
	subpls	r4, #0x800000
	addpl	r0, #0x18
	addpl	r4, #0x800000
	ldr	r2, .L1040
	cmp	r0, r2
	beq	.Lf8c
	str	r0, .L1040
	stmfd	sp!, {r6}
	ldm	r0, {r0, r1, r2, r6, r9, r10}
	adr	r14, .Lf98
.Lf70:
	stmia	r14!, {r0, r1}
	add	r14, #4
	stm	r14, {r2, r6, r9, r10}
	add	r14, #0x18
	adds	r5, #0x40000000
	bcc	.Lf70
	ldmfd	sp!, {r6}
.Lf8c:
	mov	r2, #0xff000000
.Lf90:
	ldm	r5, {r0, r1, r10, r14}
	mul	r9, r7, r12

.Lf98:
	.word	0xfedcba98
	.word	0xfedcba98
	mlane	r0, r11, r9, r0
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	bic	r7, r2, asr #1
	mulne	r9, r7, r12
	.word	0xfedcba98
	.word	0xfedcba98
	mlane	r1, r11, r9, r1
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	bic	r7, r2, asr #1
	mulne	r9, r7, r12
	.word	0xfedcba98
	.word	0xfedcba98
	mlane	r10, r11, r9, r10
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	bic	r7, r2, asr #1
	mulne	r9, r7, r12
	.word	0xfedcba98
	.word	0xfedcba98
	mlane	r14, r11, r9, lr
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98
	.word	0xfedcba98

	bic	r7, r2, asr #1
	stmia	r5!, {r0, r1, r10, r14}
	subs	r8, #4
	bgt	.Lf90
	adr	r12, .L110c
	ldm	r12, {r2, r3, sp}
	b	.L10dc

.L103c:
	.word	iwram_7900
.L1040:
	.space	4

.L1044:
	asr	r9, #22
	adds	r9, r6, lsl #1
	adds	r7, r4
	addpl	r6, r12, r6
	ldrplsb	r12, [r3, #1]!
	subpls	r12, r6
	adds	r9, r6, r9, asr #23
	add	r6, r12, r6
	adds	r7, r4
	ldrplsb	r6, [r3, #1]!
	ldrsb	r12, [r3, #1]!
	subs	r12, r6
	adds	r9, r6, r9, asr #23
	add	r7, r4
	lsr	r12, r7, #23
	ldrsb	r6, [r3, r12]!
	ldrsb	r12, [r3, #1]!
	subs	r12, r6
.L108c:
	add	r5, r8, lsl #2
.L1090:
	mul	r9, r7, r12
	asr	r9, #22
	adds	r9, r6, lsl #1
	ldrne	r0, [r5, -r8, lsl #2]
	mlane	r0, r11, r9, r0
	strne	r0, [r5, -r8, lsl #2]
	add	r7, r4
	lsrs	r9, r7, #23
	beq	.L10d4
	subs	r2, r7, lsr #23
	blle	.L10ec
	subs	r9, #1
	addeq	r6, r12, r6
	ldrnesb	r6, [r3, r9]!
	ldrsb	r12, [r3, #1]!
	sub	r12, r6
	bic	r7, #0x3f800000
.L10d4:
	subs	r8, #1
	bgt	.L1090
.L10dc:
	sub	r3, #1
	pop	{r4, r9, r12}
	str	r7, [r4, #0x1c]
	b	.L123c
.L10ec:
	add	r3, sp, #0x18
	ldm	r3, {r3, r6}
	cmp	r6, #0
	rsbne	r9, r2, #0
	addne	r2, r6, r2
	addne	pc, r14, #8
	pop	{r4, r9, r12}
	b	.L112c

.L110c:
	.space	0xc

.L1118:
	ldr	r2, [sp, #0x18]
	movs	r6, r2
	ldrne	r3, [sp, #0x14]
	movne	pc, lr
	pop	{r4, r9}
.L112c:
	strb	r6, [r4]
	b	.L1244
.L1134:
	lsls	r6, r10, #24
	asrs	r6, #24
	lsls	r6, r10, #16
	asrs	r6, #24
	lsls	r6, r10, #8
	asrs	r6, #24
	asrs	r6, r10, #24
	ldm	r3!, {r10}
	lsls	r6, r10, #24
	asrs	r6, #24
	lsls	r6, r10, #16
	asrs	r6, #24
	lsls	r6, r10, #8
	asrs	r6, #24
.L116c:
	push	{r4, r9}
.L1170:
	mov	r14, r2
	cmp	r2, r8
	movgt	r14, r8
	sub	r14, #1
	lsrs	r14, #2
	beq	.L120c
	sub	r8, r14, lsl #2
	sub	r2, r14, lsl #2
	adr	r1, .L11cc
	adr	r0, .L1134
	lsl	r9, r3, #30
	add	r0, r9, lsr #27
	ldm	r0!, {r6, r7, r9, r10}
	stm	r1, {r6, r7}
	add	r1, #0xc
	stm	r1, {r9, r10}
	add	r1, #0xc
	ldm	r0, {r6, r7, r9, r10}
	stm	r1, {r6, r7}
	add	r1, #0xc
	stm	r1, {r9, r10}
	ldm	r3!, {r10}
.L11c8:
	ldm	r5, {r0, r1, r7, r9}
.L11cc:
	nop
	nop
	mlane	r0, r11, r6, r0
	nop
	nop
	mlane	r1, r11, r6, r1
	nop
	nop
	mlane	r7, r11, r6, r7
	nop
	nop
	mlane	r9, r11, r6, r9
	stmia	r5!, {r0, r1, r7, r9}
	subs	r14, #1
	bne	.L11c8
	sub	r3, #4
.L120c:
	mov	r1, #4
.L1210:
	ldr	r0, [r5]
	ldrsb	r6, [r3], #1
	mla	r0, r11, r6, r0
	str	r0, [r5], #4
	subs	r2, #1
	bleq	.L1118
	subs	r1, #1
	bgt	.L1210
	subs	r8, #4
	bgt	.L1170
	pop	{r4, r9}
.L123c:
	str	r2, [r4, #0x18]
	str	r3, [r4, #0x28]
.L1244:
	adr	r0, .L124c + 1
	bx	r0
.L124c:
.thumb
	ldr	r0, [sp, #4]
	sub	r0, #1
	ble	.L1256
	add	r4, #0x40
	b	Func_dc8
.L1256:
	ldr	r5, [sp, #8]
	adr	r0, .L12d0
	bx	r0
.L125c:
.arm
	ror	r7, r6, #16
	adds	r7, r7
	bvc	.L1274
	orr	r6, #0x4000
	bic	r6, #0x3fc0
	submi	r6, #0x40
.L1274:
	adds	r7, r6, r6
	bxvc	lr
	orr	r6, #0x40000000
	bic	r6, #0x3fc00000
	submi	r6, #0x400000
	bx	lr
.L128c:
	mov	r6, r1
	bl	.L125c
	mov	r1, r6
	mov	r6, r2
	bl	.L125c
	mov	r2, r6
	mov	r6, r3
	bl	.L125c
	mov	r3, r6
	mov	r6, r4
	bl	.L125c
	mov	r4, r6
	mov	r14, #0x80000000
	orr	r14, r14, lsr #16
	str	r14, [pc]
	b	.L1310

	.global	Label_12cc
Label_12cc:
	.space	4

.L12d0:
	adr	r0, .L1568
	mov	r9, #0x160
	mov	r8, #0xff000000
	orr	r8, r8, lsr #16
	mov	r10, #0x80000000
	orr	r14, r10, r10, lsr #16
.L12e8:
	ldm	r0, {r1, r2, r3, r4}
	eor	r6, r1, r1, lsl #1
	ands	r6, r14
	eoreq	r6, r2, r2, lsl #1
	andeqs	r6, r14
	eoreq	r6, r3, r3, lsl #1
	andeqs	r6, r14
	eoreq	r6, r4, r4, lsl #1
	andeqs	r6, r14
	bne	.L128c
.L1310:
	and	r6, r8, r1, ror #15
	and	r7, r8, r2, ror #15
	orr	r6, r7, r6, lsr #8
	and	r11, r8, r3, ror #15
	and	r7, r8, r4, ror #15
	orr	r7, r11, lsr #8
	bic	r11, r6, r10, asr #15
	orr	r11, r7, lsl #16
	and	r7, r10, asr #15
	orr	r7, r6, lsr #16
	ldr	r12, [r5, #0x630]
	str	r11, [r5, #0x630]
	ldr	r11, [r5]
	str	r7, [r5], #4
	.global	Label_1348
Label_1348:
	asrs	r6, r4, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r4, #16
	asrs	r7, r4, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r4, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r2, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r2, #16
	asrs	r7, r2, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r2, r7, r6, lsl #16
	ror	r11, #8
	ror	r12, #8
	asrs	r6, r3, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r3, #16
	asrs	r7, r3, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r3, r7, r6, lsl #16
	ror	r11, #16
	ror	r12, #16
	asrs	r6, r1, #18
	adds	r6, r11, asr #19
	addmi	r6, #0x28
	ror	r1, #16
	asrs	r7, r1, #18
	adds	r7, r12, asr #19
	addmi	r7, #0x28
	adds	r1, r7, r6, lsl #16
	stmia	r0!, {r1, r2, r3, r4}
	subs	r9, #4
	bgt	.L12e8
	add	sp, #0x1c
	pop	{r8, r9, r10, r11}
	pop	{r5, r6, r7, lr}
	bx	lr
.L13fc:
	cmp	r12, #0
	bne	.L1480
	ldrb	r6, [r3, #2]
	add	r2, r6, lsl #24
	ldrb	r6, [r3, #4]
	adds	r6, r2, r6, lsl #24
	mvnmi	r6, r6
	lsr	r10, r6, #8
	ldrb	r1, [r3, #3]
	ldrb	r0, [r3, #1]
	lsl	r0, #24
	mla	r6, r10, r1, r0
.L142c:
	ldm	r5, {r0, r1, r10, r14}
	cmp	r7, r6
	addcc	r0, r11, lsl #6
	subcs	r0, r11, lsl #6
	adds	r7, r4, lsl #3
	cmp	r7, r6
	addcc	r1, r11, lsl #6
	subcs	r1, r11, lsl #6
	adds	r7, r4, lsl #3
	cmp	r7, r6
	addcc	r10, r11, lsl #6
	subcs	r10, r11, lsl #6
	adds	r7, r4, lsl #3
	cmp	r7, r6
	addcc	r14, r11, lsl #6
	subcs	r14, r11, lsl #6
	adds	r7, r4, lsl #3
	stmia	r5!, {r0, r1, r10, r14}
	subs	r8, #4
	bgt	.L142c
	b	.L10dc
.L1480:
	subs	r12, #1
	bne	.L150c
	mov	r6, #0x300
	lsr	r11, #1
	bic	r11, #0xff00
	mov	r12, #0x70
.L1498:
	ldm	r5, {r0, r1, r10, r14}
	adds	r7, r4, lsl #3
	rsb	r9, r12, r7, lsr #24
	lsl	r6, r7, #1
	sub	r9, r6, lsr #27
	adds	r2, r9, r2, asr #1
	mlane	r0, r11, r2, r0
	adds	r7, r4, lsl #3
	rsb	r9, r12, r7, lsr #24
	lsl	r6, r7, #1
	sub	r9, r6, lsr #27
	adds	r2, r9, r2, asr #1
	mlane	r1, r11, r2, r1
	adds	r7, r4, lsl #3
	rsb	r9, r12, r7, lsr #24
	lsl	r6, r7, #1
	sub	r9, r6, lsr #27
	adds	r2, r9, r2, asr #1
	mlane	r10, r11, r2, r10
	adds	r7, r4, lsl #3
	rsb	r9, r12, r7, lsr #24
	lsl	r6, r7, #1
	sub	r9, r6, lsr #27
	adds	r2, r9, r2, asr #1
	mlane	r14, r11, r2, r14
	stmia	r5!, {r0, r1, r10, r14}
	subs	r8, #4
	bgt	.L1498
	b	.L10dc
.L150c:
	mov	r6, #0x80
	mov	r12, #0x180
.L1514:
	ldm	r5, {r0, r1, r10, r14}
	adds	r7, r4, lsl #3
	rsbpl	r9, r6, r7, asr #23
	submi	r9, r12, r7, lsr #23
	mla	r0, r11, r9, r0
	adds	r7, r4, lsl #3
	rsbpl	r9, r6, r7, asr #23
	submi	r9, r12, r7, lsr #23
	mla	r1, r11, r9, r1
	adds	r7, r4, lsl #3
	rsbpl	r9, r6, r7, asr #23
	submi	r9, r12, r7, lsr #23
	mla	r10, r11, r9, r10
	adds	r7, r4, lsl #3
	rsbpl	r9, r6, r7, asr #23
	submi	r9, r12, r7, lsr #23
	mla	r14, r11, r9, r14
	stmia	r5!, {r0, r1, r10, r14}
	subs	r8, #4
	bgt	.L1514
	b	.L10dc

.L1568:	.space	0x590
.func_end Func_dc8

@ FastMemcpy
@ r0 = destination, r1 = source, r2 = byte count.
@ Copies 0x100 bytes per iteration with eight `ldm`/`stm` pairs of eight
@ registers, then a word-at-a-time tail for the remaining 0x1C.
@ The entry `ands r12, r2, #0xe0 / rsb r12, #0xf0 / add pc, r12, lsr #2` is a
@ COMPUTED JUMP INTO THE MIDDLE OF THE UNROLLED BLOCK, so a partial first
@ iteration handles the leading 32-byte remainder without a separate loop.
@ Word-aligned only.
.arm_func_start Func_1af8
	push	{r5, r6, r7, r8, r9}
	ands	r12, r2, #0xe0
	rsb	r12, #0xf0
	add	pc, r12, lsr #2
.L1b08:
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	ldm	r1!, {r3, r4, r5, r6, r7, r8, r9, r12}
	stmia	r0!, {r3, r4, r5, r6, r7, r8, r9, r12}
	subs	r2, #0x100
	bpl	.L1b08
	ands	r2, #0x1c
	beq	.L1b68
.L1b58:
	ldm	r1!, {r3}
	stmia	r0!, {r3}
	subs	r2, #4
	bgt	.L1b58
.L1b68:
	pop	{r5, r6, r7, r8, r9}
	bx	lr
.func_end Func_1af8
