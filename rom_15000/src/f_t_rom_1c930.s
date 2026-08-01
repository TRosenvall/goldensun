	.include "macros.inc"
	.include "gba.inc"

@ AllocMenuBlock
@ r0 = size. Allocates a menu state block with Func_48f4.
.thumb_func_start Func_1c930
	push	{lr}
	ldr	r1, =0x1004
	mov	r0, #0x13
	bl	Func_48f4
	mov	r2, r0
	mov	r3, #0
	add	r2, #0x46
	strh	r3, [r2]
	ldr	r2, =0x352
	add	r0, r2
	strh	r3, [r0]
	pop	{r0}
	bx	r0
.func_end Func_1c930

@ CloseMenuScreen
@ Takes no arguments. Waits for the window to go idle with Func_17394, closes it
@ with Func_16418, releases tiles with Func_3f3c and the block with Func_2dd8.
@ State is at iwram_1e9c.
.thumb_func_start Func_1c954
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e9c
	ldr	r2, =0xff4
	ldr	r5, [r3]
	add	r3, r5, r2
	ldr	r0, [r3]
	mov	r1, #0
	bl	Func_16418
	ldr	r3, =0xff4
	add	r6, r5, r3
	b	.L1c972
.L1c96c:
	mov	r0, #1
	bl	Func_30f8
.L1c972:
	ldr	r0, [r6]
	bl	Func_17394
	cmp	r0, #0
	beq	.L1c96c
	mov	r3, r5
	add	r3, #0x46
	ldrh	r3, [r3]
	cmp	r3, #0
	beq	.L1c990
	mov	r3, r5
	add	r3, #0x48
	ldrh	r0, [r3]
	bl	Func_3f3c
.L1c990:
	ldr	r2, =0x352
	add	r3, r5, r2
	ldrh	r3, [r3]
	cmp	r3, #0
	beq	.L1c9a4
	add	r2, #2
	add	r3, r5, r2
	ldrh	r0, [r3]
	bl	Func_3f3c
.L1c9a4:
	mov	r0, #0x13
	bl	Func_2dd8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_1c954

@ NoOp
@ A bare `bx lr`, present as a table entry or placeholder.
.thumb_func_start Func_1c9bc
	bx	lr
.func_end Func_1c9bc

@ NoOp
@ A bare `bx lr`, present as a table entry or placeholder.
.thumb_func_start Func_1c9c0
	bx	lr
.func_end Func_1c9c0

@ NoOp
@ A bare `bx lr`, present as a table entry or placeholder.
.thumb_func_start Func_1c9c4
	bx	lr
.func_end Func_1c9c4

@ ComputeMenuMetrics
@ r0.. = parameters. Pure arithmetic over the menu state with no calls out;
@ 49 lines, traced structurally.
.thumb_func_start Func_1c9c8
	mov	r1, #0x80
	lsl	r1, #3
	mov	r2, #0
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x34
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #0x3c
	strh	r2, [r3, #0xa]
	add	r3, r0, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r0, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r0, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r0, r1
	strh	r2, [r3]
	ldr	r3, =0x57c
	add	r0, r3
	strh	r2, [r0]
	bx	lr
.func_end Func_1c9c8

	.section .rodata

@ PROMOTED: referenced from rom_1aeec.s across the split
	.global	L33ef8
L33ef8:
.L33ef8:
	.incrom 0x33ef8, 0x342f8
@ PROMOTED: referenced from rom_1aeec.s across the split
	.global	L342f8
L342f8:
.L342f8:
	.incrom 0x342f8, 0x346f8
