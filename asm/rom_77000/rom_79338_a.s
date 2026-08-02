	.include "macros.inc"

@ TestSaveBit
@ r0 = index. Returns 1 when the bit is set, 0 otherwise.
@ Byte is (idx & 0xFFF) >> 3, bit is idx & 7; the `neg`/`orr`/`lsr #31` tail is
@ the branchless "is non-zero". 180 call sites -- the most-used accessor here.
.thumb_func_start GetFlag  @ 0x08079338
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	lsl	r2, r3
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldr	r3, =gFlags
	ldrb	r3, [r3, r0]
	and	r3, r2
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	bx	lr
.func_end GetFlag

@ SetSaveBit
@ r0 = index. Sets the bit. 50 call sites.
.thumb_func_start SetFlag  @ 0x08079358
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	ldr	r1, =gFlags
	lsl	r2, r3
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldrb	r3, [r1, r0]
	orr	r2, r3
	strb	r2, [r1, r0]
	bx	lr
.func_end SetFlag

@ ClearSaveBit
@ r0 = index. Clears the bit. 58 call sites.
.thumb_func_start ClearFlag  @ 0x08079374
	mov	r3, #7
	and	r3, r0
	mov	r2, #1
	lsl	r2, r3
	ldr	r1, =gFlags
	lsl	r3, r0, #20
	lsr	r0, r3, #23
	ldrb	r3, [r1, r0]
	bic	r3, r2
	strb	r3, [r1, r0]
	bx	lr
.func_end ClearFlag

