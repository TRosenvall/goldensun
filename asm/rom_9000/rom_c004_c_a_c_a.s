	.include "macros.inc"
	.include "gba.inc"

@ SetEntityActorPriority
@ r0=entity, r1=priority 0-3. Replaces bits 2-3 of the actor's flag byte at
@ +0x05 -- the OAM priority field -- leaving the affine mode in bits 0-1 alone.
@ Requires an exact draw kind of 1 (not just the low nibble).
.thumb_func_start Func_800c548  @ 0x0800c548
	push	{lr}
	cmp	r0, #0
	beq	.Lc56c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.Lc56c
	ldr	r0, [r0, #0x50]
	mov	r3, #3
	and	r1, r3
	ldrb	r2, [r0, #5]
	mov	r3, #0xd
	neg	r3, r3
	lsl	r1, #2
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #5]
.Lc56c:
	pop	{r0}
	bx	r0
.func_end Func_800c548

@ SetEntityUseCallerScale
@ r0=entity, r1=0 or 1. Replaces bit 1 of the actor's flag byte at +0x1D, the
@ flag Func_b388 tests to decide whether to compute the perspective scale from
@ the depth or take the caller-supplied value. Requires draw kind exactly 1.
.thumb_func_start Func_800c570  @ 0x0800c570
	push	{lr}
	cmp	r0, #0
	beq	.Lc594
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.Lc594
	ldr	r0, [r0, #0x50]
	mov	r3, #1
	and	r1, r3
	ldrb	r2, [r0, #0x1d]
	mov	r3, #3
	neg	r3, r3
	lsl	r1, #1
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #0x1d]
.Lc594:
	pop	{r0}
	bx	r0
.func_end Func_800c570

