	.include "macros.inc"

@ ReadSaveByte
@ r0 = index. Returns the whole byte at (idx & 0xFFF) >> 3 -- the same byte the
@ bit accessors address, read in one piece.
.thumb_func_start GetFlagByte  @ 0x080793b8
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =gFlags
	ldrb	r0, [r3, r0]
	bx	lr
.func_end GetFlagByte

@ WriteSaveByte
@ r0 = index, r1 = value. Overwrites the whole byte, so this clobbers all eight
@ bits at that index.
.thumb_func_start SetFlagByte  @ 0x080793c8
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =gFlags
	strb	r1, [r3, r0]
	bx	lr
.func_end SetFlagByte

@ IncrementSaveCounter
@ r0 = index. Adds 1 to the byte and returns the new value, SATURATING at 0xFF
@ rather than wrapping -- the `cmp #0xFE / bhi` guard.
.thumb_func_start IncFlagByte  @ 0x080793d8
	push	{lr}
	lsl	r3, r0, #20
	ldr	r1, =gFlags
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
.func_end IncFlagByte

@ DecrementSaveCounter
@ r0 = index. Subtracts 1 and returns the new value, saturating at 0 rather than
@ underflowing.
.thumb_func_start DecFlagByte  @ 0x080793f8
	push	{lr}
	lsl	r3, r0, #20
	ldr	r1, =gFlags
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
.func_end DecFlagByte

@ ReadSaveNibble
@ r0 = index. Returns one nibble of the byte. Bit 2 of the index selects which:
@ the shift is `idx & 4`, so even groups read the low nibble and odd groups the
@ high one.
.thumb_func_start GetFlagNybble  @ 0x08079418
	lsl	r3, r0, #20
	mov	r1, #4
	and	r1, r0
	lsr	r0, r3, #23
	ldr	r3, =gFlags
	mov	r2, #0xf
	ldrb	r0, [r3, r0]
	lsl	r2, r1
	and	r0, r2
	asr	r0, r1
	bx	lr
.func_end GetFlagNybble

