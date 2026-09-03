	.include "macros.inc"
	.include "gba.inc"

@ InitUiSystem
@ Takes no arguments. Brings up the whole UI layer:
@     galloc_ewram(0xF, 0x12FC) allocates the UI block -- this is what iwram_1e8c
@       points at, and 0x12FC is its size, so every +0xNNN offset in this module
@       is bounded by it
@     the block is DMA-cleared, then defaults are written:
@       +0xEA3 = 1 (dirty mask, see Func_160fc), +0xEA7 = 0x0F, +0x12B6 = 0x63
@     the first 0x140 bytes are filled with 0xF000F000, the empty-tile pattern
@     Func_15ef4 builds the node free list
@     Func_19d0c initialises the menu layer
@     StartTask registers Func_160fc as the per-frame flush at priority 0x480
@     Func_173f4 finishes setup
@ Func_16018 below is the same sequence with one extra field and a different
@ finisher; use this one for the plain case.
.thumb_func_start Func_8015f30  @ 0x08015f30
	push	{r5, lr}
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	galloc_ewram
	mov	r3, #0
	mov	r4, r0
	mov	r5, sp
	str	r3, [r5]
	mov	r0, r5
	ldr	r3, =REG_DMA3SAD
	mov	r1, r4
	ldr	r2, =0x850004bf
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0xea3
	add	r2, r4, r3
	mov	r3, #1
	strb	r3, [r2]
	ldr	r3, =0x12b6
	add	r2, r4, r3
	mov	r3, #0x63
	strh	r3, [r2]
	ldr	r3, =0xea7
	add	r2, r4, r3
	mov	r3, #0xf
	strb	r3, [r2]
	ldr	r3, =0xf000f000
	mov	r0, r5
	str	r3, [r5]
	ldr	r2, =0x85000140
	ldr	r3, =REG_DMA3SAD
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bl	Func_8015ef4
	bl	Func_8019d0c
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_80160fc
	bl	StartTask
	bl	Func_80173f4
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_8015f30
