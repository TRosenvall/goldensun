	.include "macros.inc"

@ WouldTransferStayBalanced
@ r0 = giver, r1 = receiver. Takes the party's djinn counts from Func_ae7fc,
@ applies the move -- one off the giver, one onto the receiver -- and then
@ compares EVERY pair of members. Returns 1 only when no two counts differ by
@ more than one, and 0 as soon as it finds a pair that does.
@
@ That is a hard balance constraint on how djinn may be distributed, enforced
@ before the move is allowed rather than after.
.thumb_func_start Func_80ae778  @ 0x080ae778
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f2c
	sub	sp, #0x10
	mov	r7, sp
	ldr	r3, [r3]
	mov	r5, r0
	mov	r0, r7
	mov	r6, r1
	mov	r8, r3
	bl	Func_80ae7fc
	ldrb	r3, [r7, r5]
	sub	r3, #1
	strb	r3, [r7, r5]
	ldrb	r3, [r7, r6]
	ldr	r2, =0x219
	add	r3, #1
	strb	r3, [r7, r6]
	add	r2, r8
	mov	r3, #1
	mov	r10, r3
	ldrb	r3, [r2]
	mov	r5, #0
	cmp	r5, r3
	bcs	.Lae7e4
	mov	r12, r3
	mov	r0, r2
	mov	r14, r12
.Lae7b6:
	mov	r1, r5
	cmp	r5, r12
	bcs	.Lae7da
	ldrb	r6, [r0]
	mov	r4, r7
.Lae7c0:
	add	r3, r1, #1
	lsl	r3, #24
	lsr	r1, r3, #24
	cmp	r1, r6
	bcs	.Lae7da
	ldrsb	r3, [r4, r5]
	ldrsb	r2, [r4, r1]
	sub	r3, r2
	add	r3, #1
	cmp	r3, #2
	bls	.Lae7c0
	mov	r3, #0
	mov	r10, r3
.Lae7da:
	add	r3, r5, #1
	lsl	r3, #24
	lsr	r5, r3, #24
	cmp	r5, r14
	bcc	.Lae7b6
.Lae7e4:
	mov	r0, r10
	add	sp, #0x10
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80ae778

@ CountDjinnPerMember
@ r0 = one byte per member. For each roster member, counts the bits set in
@ either the "has" mask at record+0xF8 or the "set" mask at record+0x108, over
@ twenty bits in each of four element words, and stores the total.
@
@ Note it counts a djinn once even when it appears in both masks -- the test is
@ an OR, not a sum -- so this is how many djinn the character owns, not how many
@ are set.
.thumb_func_start Func_80ae7fc  @ 0x080ae7fc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	mov	r10, r3
	ldr	r3, =0x219
	add	r3, r10
	mov	r2, #0
	ldrb	r3, [r3]
	mov	r8, r2
	mov	r11, r0
	cmp	r8, r3
	bge	.Lae874
	mov	r7, #0x82
	mov	r3, #1
	lsl	r7, #2
	mov	r9, r3
	add	r7, r10
.Lae82a:
	ldrh	r0, [r7]
	bl	_GetUnit
	mov	r6, #0
	mov	r4, #0
	add	r0, #0xf8
.Lae836:
	ldr	r5, [r0, #0x10]
	mov	r1, #0
.Lae83a:
	mov	r2, r9
	lsl	r2, r1
	mov	r3, r5
	and	r3, r2
	cmp	r3, #0
	bne	.Lae84e
	ldr	r3, [r0]
	and	r3, r2
	cmp	r3, #0
	beq	.Lae850
.Lae84e:
	add	r4, #1
.Lae850:
	add	r1, #1
	cmp	r1, #0x13
	ble	.Lae83a
	add	r6, #1
	add	r0, #4
	cmp	r6, #3
	ble	.Lae836
	mov	r3, r8
	mov	r2, r11
	strb	r4, [r2, r3]
	ldr	r3, =0x219
	add	r3, r10
	mov	r2, #1
	ldrb	r3, [r3]
	add	r8, r2
	add	r7, #2
	cmp	r8, r3
	blt	.Lae82a
.Lae874:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80ae7fc

	.section .rodata
	.global .Laf304
	.global .Laf304

.Laf304:
	.incrom 0xaf304, 0xaf314
