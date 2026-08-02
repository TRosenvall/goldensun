	.include "macros.inc"
	.include "gba.inc"

@ CreateFieldEffect
@ r0=effect instance to fill, r1=actor resource id, r2=x, r3=y.
@ Zero-fills the 0x48-byte instance by DMA, creates its actor with _Func_bc70
@ and clears the actor's priority bits (+0x09 bits 2-3), then places it with
@ .gcc2_compiled..
@ Seeds the defaults: +0x20 = 0x20000, the three speed words at +0x24/+0x28/+0x2C
@ = 0x10000, the origin copied to +0x14/+0x18, the actor's option byte at +0x26
@ cleared, and the five enable bytes at +0x41..+0x45 all set to 1.
.thumb_func_start Func_809ba90  @ 0x0809ba90
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #4
	mov	r5, r0
	mov	r6, r2
	mov	r0, sp
	mov	r2, #0
	mov	r4, r1
	mov	r7, r3
	mov	r8, r2
	str	r2, [r0]
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x85000012
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r4
	bl	_CreateSprite
	str	r0, [r5]
	cmp	r0, #0
	beq	.L9bac8
	ldrb	r2, [r0, #9]
	mov	r3, #0xd
	neg	r3, r3
	and	r3, r2
	strb	r3, [r0, #9]
.L9bac8:
	mov	r1, r6
	mov	r0, r5
	mov	r2, r7
	bl	Func_809ba5c
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r5, #0x20]
	mov	r2, #0x80
	ldr	r3, [r5]
	lsl	r2, #9
	str	r2, [r5, #0x28]
	str	r2, [r5, #0x2c]
	str	r2, [r5, #0x24]
	add	r3, #0x26
	mov	r2, r8
	str	r6, [r5, #0x14]
	str	r7, [r5, #0x18]
	strb	r2, [r3]
	mov	r3, r5
	mov	r2, #1
	add	r3, #0x41
	strb	r2, [r3]
	add	r3, #1
	strb	r2, [r3]
	add	r3, #1
	strb	r2, [r3]
	add	r3, #1
	strb	r2, [r3]
	add	r3, #1
	strb	r2, [r3]
	bl	Random
	mov	r3, r5
	add	r3, #0x46
	mov	r2, r5
	strb	r0, [r3]
	add	r2, #0x47
	mov	r3, #4
	strb	r3, [r2]
	mov	r0, r5
	mov	r1, #1
	bl	Func_809ba70
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_809ba90

@ DestroyFieldEffect
@ r0=effect instance. Releases the instance's actor and clears its enable bytes
@ so Field_Avoid stops updating it.
.thumb_func_start Func_809bb34  @ 0x0809bb34
	push	{r5, lr}
	mov	r5, r0
	ldr	r0, [r5]
	sub	sp, #4
	cmp	r0, #0
	beq	.L9bb44
	bl	_DeleteSprite
.L9bb44:
	mov	r0, sp
	mov	r3, #0
	str	r3, [r0]
	mov	r1, r5
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000012
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_809bb34

	.section .rodata
	.global .L9c510

.L9c510:
	.incrom 0x9c510, 0x9c610
