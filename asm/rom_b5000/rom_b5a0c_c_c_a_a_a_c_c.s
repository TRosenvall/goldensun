	.include "macros.inc"
	.include "gba.inc"

@ ListLivingCombatants
@ r0.. = parameters. Builds the list of combatants still standing, filtering the
@ Func_b6a60 result by each record's state and the save bits _Func_79338
@ reports.
.thumb_func_start Func_80b6b40  @ 0x080b6b40
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r10, r0
	mov	r0, #0xb6
	mov	r6, r1
	mov	r2, #6
	mov	r1, #0
	lsl	r0, #1
	sub	sp, #0x14
	mov	r8, r1
	mov	r9, r2
	bl	_GetFlag
	cmp	r0, #0
	beq	.Lb6b68
	mov	r3, #3
	mov	r9, r3
.Lb6b68:
	mov	r3, #1
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.Lb6baa
	add	r5, sp, #4
	mov	r0, r5
	bl	Func_80b6a60
	cmp	r8, r0
	bge	.Lb6baa
	mov	r2, r5
	mov	r5, r0
.Lb6b82:
	ldrh	r7, [r2]
	add	r2, #2
	mov	r0, r7
	str	r2, [sp]
	bl	_GetUnit
	mov	r1, #0x38
	ldrsh	r3, [r0, r1]
	ldr	r2, [sp]
	cmp	r3, #0
	ble	.Lb6ba4
	cmp	r6, #0
	beq	.Lb6ba0
	strh	r7, [r6]
	add	r6, #2
.Lb6ba0:
	mov	r3, #1
	add	r8, r3
.Lb6ba4:
	sub	r5, #1
	cmp	r5, #0
	bne	.Lb6b82
.Lb6baa:
	mov	r3, #2
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.Lb6bea
	mov	r7, r9
	mov	r5, #0x80
	add	r7, #0x80
	cmp	r5, r7
	bge	.Lb6bea
.Lb6bbe:
	mov	r0, r5
	bl	_GetUnit
	mov	r2, #0x95
	lsl	r2, #1
	add	r3, r0, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.Lb6be4
	mov	r1, #0x38
	ldrsh	r3, [r0, r1]
	cmp	r3, #0
	ble	.Lb6be4
	cmp	r6, #0
	beq	.Lb6be0
	strh	r5, [r6]
	add	r6, #2
.Lb6be0:
	mov	r2, #1
	add	r8, r2
.Lb6be4:
	add	r5, #1
	cmp	r5, r7
	blt	.Lb6bbe
.Lb6bea:
	cmp	r6, #0
	beq	.Lb6bf2
	ldr	r3, =0xff
	strh	r3, [r6]
.Lb6bf2:
	mov	r0, r8
	add	sp, #0x14
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80b6b40

@ WalkActionQueue
@ r0 = selector mask, r1 = destination (may be 0 to count only).
@ Walks the halfword list at [iwram_1e74]+0x58. THE QUEUE IS SENTINEL-DELIMITED:
@ 0xFF ends it and 0xFE separates groups within it. Bit 0 of the selector picks
@ the first arm; entries are copied out as halfwords.
.thumb_func_start Func_80b6c08  @ 0x080b6c08
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e74
	mov	r6, r0
	ldr	r0, [r3]
	mov	r3, #1
	and	r3, r6
	mov	r5, #0
	cmp	r3, #0
	beq	.Lb6c44
	mov	r3, #0x58
	ldrsh	r3, [r0, r3]
	cmp	r3, #0xff
	beq	.Lb6c44
	mov	r2, r0
	add	r2, #0x58
.Lb6c26:
	mov	r7, #0
	ldrsh	r3, [r2, r7]
	ldrh	r4, [r2]
	cmp	r3, #0xfe
	beq	.Lb6c3a
	cmp	r1, #0
	beq	.Lb6c38
	strh	r4, [r1]
	add	r1, #2
.Lb6c38:
	add	r5, #1
.Lb6c3a:
	add	r2, #2
	mov	r4, #0
	ldrsh	r3, [r2, r4]
	cmp	r3, #0xff
	bne	.Lb6c26
.Lb6c44:
	mov	r3, #2
	and	r3, r6
	cmp	r3, #0
	beq	.Lb6c76
	add	r2, r0, #2
	mov	r3, #0x64
	ldrsh	r3, [r2, r3]
	mov	r12, r2
	cmp	r3, #0xff
	beq	.Lb6c76
	mov	r0, #0x64
.Lb6c5a:
	ldrsh	r3, [r2, r0]
	ldrh	r4, [r2, r0]
	cmp	r3, #0xfe
	beq	.Lb6c6c
	cmp	r1, #0
	beq	.Lb6c6a
	strh	r4, [r1]
	add	r1, #2
.Lb6c6a:
	add	r5, #1
.Lb6c6c:
	add	r0, #2
	mov	r2, r12
	ldrsh	r3, [r2, r0]
	cmp	r3, #0xff
	bne	.Lb6c5a
.Lb6c76:
	cmp	r1, #0
	beq	.Lb6c7e
	ldr	r3, .Lb6c88	@ 0xff
	strh	r3, [r1]
.Lb6c7e:
	mov	r0, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1

	.align	2, 0
.Lb6c88:
	.word	0xff
.func_end Func_80b6c08
