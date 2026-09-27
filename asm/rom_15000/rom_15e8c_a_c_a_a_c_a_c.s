	.include "macros.inc"
	.include "gba.inc"

@ FlushUiToVram
@ Takes no arguments. Registered by Func_15f30/Func_16018 as a per-frame task.
@ Does nothing while the suspend flag at [iwram_1e8c]+0xEA6 is non-zero.
@ Otherwise it reads the DIRTY MASK at +0xEA3 and DMA-copies one 0x100-byte
@ block to 0x6002000 for each set bit, advancing both source and destination by
@ 0x100 per bit. Bit 0 means "everything": it replaces the mask with 0x3F so all
@ rows are sent. The mask is shifted right once before the loop, so bits 1..5
@ are the five individually-dirtyable blocks. The mask is cleared afterwards.
@ Everything else in this module marks work by setting bits here; this is the
@ only place that touches VRAM for it.
.thumb_func_start Func_80160fc  @ 0x080160fc
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r2, =0xea6
	ldr	r7, [r3]
	add	r3, r7, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L1615a
	sub	r2, #3
	add	r3, r7, r2
	ldrb	r4, [r3]
	cmp	r4, #0
	beq	.L1615a
	ldr	r3, =0x6002000
	mov	r12, r3
	mov	r3, #1
	and	r3, r4
	mov	r5, r7
	cmp	r3, #0
	beq	.L16126
	mov	r4, #0x3f
.L16126:
	mov	r3, #0x3f
	and	r4, r3
	mov	r2, #1
	mov	r6, #0x80
	lsr	r4, #1
	mov	r14, r2
	lsl	r6, #1
.L16134:
	mov	r3, r4
	mov	r2, r14
	and	r3, r2
	cmp	r3, #0
	beq	.L1614a
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	mov	r1, r12
	ldr	r2, =0x84000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.L1614a:
	lsr	r4, #1
	add	r5, r6
	add	r12, r6
	cmp	r4, #0
	bne	.L16134
	ldr	r2, =0xea3
	add	r3, r7, r2
	strb	r4, [r3]
.L1615a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80160fc
