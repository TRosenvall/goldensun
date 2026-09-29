	.include "macros.inc"

@ RevertClassChange
@ r0 = combatant id. Undoes Func_7a664: clears the save bits with ClearFlag
@ and refreshes with CalcStats and .gcc2_compiled..
.thumb_func_start Func_807a7a0  @ 0x0807a7a0
	push	{r5, r6, lr}
	ldr	r5, =ewram_2001078
	ldrh	r3, [r5]
	ldr	r1, =0x6774
	mov	r12, r3
	add	r5, #2
	cmp	r12, r1
	bne	.L7a812
	mov	r6, #0
.L7a7b2:
	mov	r0, r6
	bl	GetUnit
	mov	r2, #0xe
	add	r0, #0xd8
.L7a7bc:
	ldrh	r3, [r5]
	sub	r2, #1
	strh	r3, [r0]
	add	r5, #2
	add	r0, #2
	cmp	r2, #0
	bge	.L7a7bc
	mov	r0, r6
	bl	Func_8079ae8
	mov	r0, r6
	add	r6, #1
	bl	CalcStats
	cmp	r6, #3
	ble	.L7a7b2
	ldr	r0, =gState
	mov	r4, #0x88
	ldrh	r2, [r5]
	lsl	r4, #2
	add	r3, r0, r4
	strh	r2, [r3]
	add	r5, #2
	ldrh	r2, [r5]
	add	r4, #2
	add	r3, r0, r4
	strh	r2, [r3]
	mov	r3, #0xfc
	add	r5, #2
	lsl	r3, #1
	add	r2, r0, r3
	ldrh	r3, [r5]
	strh	r3, [r2]
	sub	r4, #0x28
	ldrh	r3, [r5, #2]
	ldr	r5, =ewram_2001078
	add	r2, r0, r4
	mov	r1, #0
	strh	r3, [r2]
	ldr	r0, =0x952
	strh	r1, [r5]
	bl	ClearFlag
.L7a812:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_807a7a0

	.section .rodata
	.global .L89258
	.global .L8926c
	.global .L84a8c
	.global .L84b1c
	.global .L88df8
	.global .L88e38
	.global .L84a8c
	.global .L84a9c
	.global .L88db8
	.global .L88e38

.L84a8c:
	.incrom 0x84a8c, 0x84a9c
.L84a9c:
	.incrom 0x84a9c, 0x84b1c
.L84b1c:
	.incrom 0x84b1c, 0x88db8
.L88db8:
	.incrom 0x88db8, 0x88df8
.L88df8:
	.incrom 0x88df8, 0x88e38
.L88e38:
	.incrom 0x88e38, 0x89258
.L89258:
	.incrom 0x89258, 0x8926c
.L8926c:
	.incrom 0x8926c, 0x89624
