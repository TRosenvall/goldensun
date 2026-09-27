	.include "macros.inc"
	.include "gba.inc"

@ InitUiSystemWithMode
@ r0 = mode, passed on to .gcc2_compiled..
@ The same bring-up as Func_15f30 -- same 0x12FC allocation under tag 0xF, same
@ clear, same defaults, same node pool, same Func_160fc registration -- with two
@ differences: it also sets +0xEA5 = 1, and it finishes with .gcc2_compiled.(mode)
@ instead of Func_173f4, then seeds a tile through Func_15fb8(0xF013, 0x80).
@ It does NOT call Func_19d0c, so the menu layer is left uninitialised.
.thumb_func_start Func_8016018  @ 0x08016018
	push	{r5, r6, lr}
	mov	r6, r9
	push	{r6}
	mov	r6, r0
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	galloc_ewram
	mov	r3, #0
	mov	r5, r0
	mov	r4, sp
	str	r3, [r4]
	mov	r0, r4
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x850004bf
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r1, =0xea3
	add	r3, r5, r1
	mov	r1, #1
	strb	r1, [r3]
	ldr	r3, =0x12b6
	add	r2, r5, r3
	mov	r3, #0x63
	strh	r3, [r2]
	ldr	r2, =0xea5
	add	r3, r5, r2
	strb	r1, [r3]
	ldr	r3, =0xea7
	add	r2, r5, r3
	mov	r3, #0xf
	strb	r3, [r2]
	ldr	r3, =0xf000f000
	mov	r0, r4
	str	r3, [r4]
	mov	r1, r5
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000140
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bl	Func_8015ef4
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_80160fc
	bl	StartTask
	mov	r0, r6
	bl	Func_8017464
	add	r1, sp, #4
	mov	r9, r1
	ldr	r0, =0xf013
	mov	r1, #0x80
	bl	Func_8015fb8
	add	r2, sp, #4
	mov	r9, r2
	mov	r1, #0x81
	ldr	r0, =0xf014
	bl	Func_8015fb8
	add	r3, sp, #4
	mov	r9, r3
	mov	r1, #0x82
	ldr	r0, =0xf015
	bl	Func_8015fb8
	ldr	r1, =0xda2
	mov	r2, #4
	mov	r3, #2
	add	r5, r1
.L160ac:
	sub	r3, #1
	strb	r2, [r5]
	sub	r5, #1
	cmp	r3, #0
	bge	.L160ac
	add	sp, #4
	pop	{r3}
	mov	r9, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8016018

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
