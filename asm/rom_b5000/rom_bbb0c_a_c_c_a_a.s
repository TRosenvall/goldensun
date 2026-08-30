	.include "macros.inc"
	.include "gba.inc"

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf250  @ 0x080bf250
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r0
	bl	_GetUnit
	mov	r2, #0x99
	lsl	r2, #1
	mov	r1, r0
	add	r5, r1, r2
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf2a4
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r7, #0
	cmp	r3, #0
	bne	.Lbf282
	ldr	r2, =0x133
	add	r3, r1, r2
	strb	r7, [r3]
	mov	r0, #1
	b	.Lbf2a6
.Lbf282:
	ldr	r3, =0x133
	add	r6, r1, r3
	mov	r3, #0
	ldrsb	r3, [r6, r3]
	cmp	r3, #0
	bge	.Lbf2a4
	ldrb	r1, [r5]
	mov	r0, r8
	mov	r2, #0x1e
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf2a4
	strb	r7, [r6]
	mov	r0, #1
	strb	r7, [r5]
	b	.Lbf2a6
.Lbf2a4:
	mov	r0, #0
.Lbf2a6:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80bf250

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf2b4  @ 0x080bf2b4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r0
	bl	_GetUnit
	mov	r2, #0x9a
	lsl	r2, #1
	mov	r1, r0
	add	r5, r1, r2
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf308
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r7, #0
	cmp	r3, #0
	bne	.Lbf2e6
	ldr	r2, =0x135
	add	r3, r1, r2
	strb	r7, [r3]
	mov	r0, #1
	b	.Lbf30a
.Lbf2e6:
	ldr	r3, =0x135
	add	r6, r1, r3
	mov	r3, #0
	ldrsb	r3, [r6, r3]
	cmp	r3, #0
	bge	.Lbf308
	ldrb	r1, [r5]
	mov	r0, r8
	mov	r2, #0x14
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf308
	strb	r7, [r6]
	mov	r0, #1
	strb	r7, [r5]
	b	.Lbf30a
.Lbf308:
	mov	r0, #0
.Lbf30a:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80bf2b4

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf318  @ 0x080bf318
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r8, r0
	bl	_GetUnit
	mov	r2, #0x9b
	lsl	r2, #1
	mov	r1, r0
	add	r5, r1, r2
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf36c
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r7, #0
	cmp	r3, #0
	bne	.Lbf34a
	ldr	r2, =0x137
	add	r3, r1, r2
	strb	r7, [r3]
	mov	r0, #1
	b	.Lbf36e
.Lbf34a:
	ldr	r3, =0x137
	add	r6, r1, r3
	mov	r3, #0
	ldrsb	r3, [r6, r3]
	cmp	r3, #0
	bge	.Lbf36c
	ldrb	r1, [r5]
	mov	r0, r8
	mov	r2, #0x14
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf36c
	strb	r7, [r6]
	mov	r0, #1
	strb	r7, [r5]
	b	.Lbf36e
.Lbf36c:
	mov	r0, #0
.Lbf36e:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80bf318

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf37c  @ 0x080bf37c
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	mov	r3, #0x9c
	lsl	r3, #1
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf3b4
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf3b6
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x1e
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf3b4
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf3b6
.Lbf3b4:
	mov	r0, #0
.Lbf3b6:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf37c

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf3bc  @ 0x080bf3bc
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	ldr	r3, =0x139
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf3f2
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf3f4
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x3c
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf3f2
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf3f4
.Lbf3f2:
	mov	r0, #0
.Lbf3f4:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf3bc

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf400  @ 0x080bf400
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	mov	r3, #0x9d
	lsl	r3, #1
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf438
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf43a
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x46
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf438
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf43a
.Lbf438:
	mov	r0, #0
.Lbf43a:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf400

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf440  @ 0x080bf440
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	ldr	r3, =0x13b
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf476
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf478
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x28
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf476
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf478
.Lbf476:
	mov	r0, #0
.Lbf478:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf440

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf484  @ 0x080bf484
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	mov	r3, #0x9e
	lsl	r3, #1
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf4bc
	add	r3, #0xff
	strb	r3, [r5]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf4be
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x32
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf4bc
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf4be
.Lbf4bc:
	mov	r0, #0
.Lbf4be:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf484

@ TickStatusCounter
@ r0 = combatant id. Decrements one of the per-combatant status counters in the
@ persistent record and returns 1 on the turn it reaches zero, clearing the
@ companion field alongside it. One of ten near-identical routines
@ (Func_bf250..Func_bf4c4) differing only in which record offset they tick.
.thumb_func_start Func_80bf4c4  @ 0x080bf4c4
	push	{r5, r6, lr}
	mov	r6, r0
	bl	_GetUnit
	ldr	r3, =0x13d
	add	r5, r0, r3
	ldrb	r2, [r5]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf516
	cmp	r3, #7
	bls	.Lbf4e2
	add	r3, #0xf8
	strb	r3, [r5]
	mov	r2, r3
.Lbf4e2:
	mov	r3, #7
	and	r3, r2
	cmp	r3, #0
	beq	.Lbf4f2
	mov	r3, r2
	add	r3, #0xff
	strb	r3, [r5]
	mov	r2, r3
.Lbf4f2:
	lsl	r3, r2, #24
	lsr	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf518
	cmp	r3, #7
	bhi	.Lbf516
	ldrb	r1, [r5]
	mov	r0, r6
	mov	r2, #0x1e
	bl	Func_80bf208
	cmp	r0, #0
	beq	.Lbf516
	mov	r3, #0
	strb	r3, [r5]
	mov	r0, #1
	b	.Lbf518
.Lbf516:
	mov	r0, #0
.Lbf518:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80bf4c4

@ TickStatusCounterAt13E
@ r0 = combatant id. Decrements the byte at record+0x13E and returns 1 on the
@ turn it reaches zero. Same shape as the Func_bf250 family without the
@ companion clear.
.thumb_func_start Func_80bf524  @ 0x080bf524
	push	{lr}
	bl	_GetUnit
	mov	r3, #0x9f
	lsl	r3, #1
	add	r1, r0, r3
	ldrb	r2, [r1]
	mov	r3, r2
	cmp	r3, #0
	beq	.Lbf544
	add	r3, #0xff
	strb	r3, [r1]
	lsl	r3, #24
	mov	r0, #1
	cmp	r3, #0
	beq	.Lbf546
.Lbf544:
	mov	r0, #0
.Lbf546:
	pop	{r1}
	bx	r1
.func_end Func_80bf524
