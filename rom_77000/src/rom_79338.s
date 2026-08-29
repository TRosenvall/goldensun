	.include "macros.inc"

@ TestSaveBit
@ r0 = index. Returns 1 when the bit is set, 0 otherwise.
@ Byte is (idx & 0xFFF) >> 3, bit is idx & 7; the `neg`/`orr`/`lsr #31` tail is
@ the branchless "is non-zero". 180 call sites -- the most-used accessor here.
.thumb_func_start Func_79338
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	lsl	r2, r3
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =ewram_40
	ldrb	r3, [r3, r0]
	and	r3, r2
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	bx	lr
.func_end Func_79338

@ SetSaveBit
@ r0 = index. Sets the bit. 50 call sites.
.thumb_func_start Func_79358
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	ldr	r1, =ewram_40
	lsl	r2, r3
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldrb	r3, [r1, r0]
	orr	r2, r3
	strb	r2, [r1, r0]
	bx	lr
.func_end Func_79358

@ ClearSaveBit
@ r0 = index. Clears the bit. 58 call sites.
.thumb_func_start Func_79374
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	lsl	r2, r3
	ldr	r1, =ewram_40
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldrb	r3, [r1, r0]
	bic	r3, r2
	strb	r3, [r1, r0]
	bx	lr
.func_end Func_79374

@ ToggleSaveBit
@ r0 = index. Flips the bit with `eor` and returns its NEW value as 0 or 1.
.thumb_func_start Func_79390
	mov	r4, r0
	mov	r3, #7
	and	r3, r4
	mov	r1, #1
	lsl	r1, r3
	ldr	r2, =ewram_40
	lsl	r3, r4, #20
	lsr	r4, r3, #23
	ldrb	r0, [r2, r4]
	mov	r3, r1
	eor	r3, r0
	strb	r3, [r2, r4]
	ldrb	r3, [r2, r4]
	and	r3, r1
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	bx	lr
.func_end Func_79390

@ ReadSaveByte
@ r0 = index. Returns the whole byte at (idx & 0xFFF) >> 3 -- the same byte the
@ bit accessors address, read in one piece.
.thumb_func_start Func_793b8
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =ewram_40
	ldrb	r0, [r3, r0]
	bx	lr
.func_end Func_793b8

@ WriteSaveByte
@ r0 = index, r1 = value. Overwrites the whole byte, so this clobbers all eight
@ bits at that index.
.thumb_func_start Func_793c8
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =ewram_40
	strb	r1, [r3, r0]
	bx	lr
.func_end Func_793c8

@ IncrementSaveCounter
@ r0 = index. Adds 1 to the byte and returns the new value, SATURATING at 0xFF
@ rather than wrapping -- the `cmp #0xFE / bhi` guard.
.thumb_func_start Func_793d8
	push	{lr}
	lsl	r3, r0, #20
	ldr	r1, =ewram_40
	lsr	r0, r3, #23
	ldrb	r2, [r1, r0]
	mov	r3, r2
	cmp	r3, #0xfe
	bhi	.L793ec
	add	r3, r2, #1
	strb	r3, [r1, r0]
.L793ec:
	ldrb	r0, [r1, r0]
	pop	{r1}
	bx	r1
.func_end Func_793d8

@ DecrementSaveCounter
@ r0 = index. Subtracts 1 and returns the new value, saturating at 0 rather than
@ underflowing.
.thumb_func_start Func_793f8
	push	{lr}
	lsl	r3, r0, #20
	ldr	r1, =ewram_40
	lsr	r0, r3, #23
	ldrb	r2, [r1, r0]
	mov	r3, r2
	cmp	r3, #0
	beq	.L7940c
	add	r3, #0xff
	strb	r3, [r1, r0]
.L7940c:
	ldrb	r0, [r1, r0]
	pop	{r1}
	bx	r1
.func_end Func_793f8

@ ReadSaveNibble
@ r0 = index. Returns one nibble of the byte. Bit 2 of the index selects which:
@ the shift is `idx & 4`, so even groups read the low nibble and odd groups the
@ high one.
.thumb_func_start Func_79418
	lsl	r3, r0, #20
	mov	r1, #4
	and	r1, r0
	lsr	r0, r3, #23
	ldr	r3, =ewram_40
	mov	r2, #0xf
	ldrb	r0, [r3, r0]
	lsl	r2, r1
	and	r0, r2
	asr	r0, r1
	bx	lr
.func_end Func_79418

@ WriteSaveNibble
@ r0 = index, r1 = value (low 4 bits used). Replaces one nibble, preserving the
@ other. Same bit-2 selection as Func_79418.
.thumb_func_start Func_79434
	push	{r5, r6, lr}
	mov	r6, r0
	ldr	r4, =ewram_40
	lsl	r3, r6, #20
	mov	r0, #4
	and	r0, r6
	mov	r5, #0xf
	lsr	r6, r3, #23
	ldrb	r3, [r4, r6]
	mov	r2, r5
	lsl	r2, r0
	and	r1, r5
	bic	r3, r2
	lsl	r1, r0
	orr	r3, r1
	strb	r3, [r4, r6]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_79434
