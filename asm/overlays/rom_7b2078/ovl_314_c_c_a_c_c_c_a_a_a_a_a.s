	.include "macros.inc"

@ 89 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, WaitFrames, Cos, Sin
@   PlaySound, OvlFunc_common0_10c x3
.thumb_func_start OvlFunc_926_2008bf4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0x13
	sub	sp, #0x10
	bl	__MapActor_GetActor
	mov	r6, #8
	mov	r7, r0
	mov	r8, r6
.Lc08:
	ldr	r3, [r7, #0x50]
	lsl	r5, r6, #12
	strh	r5, [r3, #0x1e]
	mov	r0, r8
	bl	__WaitFrames
	mov	r0, r5
	bl	__cos
	lsl	r2, r0, #1
	ldr	r3, [r7, #8]
	add	r2, r0
	lsl	r2, #1
	add	r3, r2
	str	r3, [r7, #8]
	mov	r0, r5
	bl	__sin
	lsl	r2, r0, #1
	ldr	r3, [r7, #0x10]
	add	r2, r0
	lsl	r2, #1
	add	r3, r2
	str	r3, [r7, #0x10]
	mov	r3, #2
	neg	r3, r3
	sub	r6, #1
	add	r8, r3
	cmp	r6, #3
	bhi	.Lc08
	mov	r3, #0x90
	lsl	r3, #13
	str	r3, [r7, #0xc]
	str	r3, [r7, #0x3c]
	mov	r0, #0xe3
	bl	__PlaySound
	mov	r6, #0x80
	ldr	r0, [r7, #8]
	ldr	r2, [r7, #0x10]
	ldr	r3, =0xfff40000
	lsl	r6, #12
	ldr	r4, =0x6666
	mov	r5, #0
	ldr	r1, [r7, #0xc]
	add	r0, r3
	add	r2, r6
	ldr	r3, =0xffffcccd
	str	r4, [sp]
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	ldr	r2, [r7, #0x10]
	ldr	r4, =0x4ccc
	ldr	r0, [r7, #8]
	ldr	r1, [r7, #0xc]
	add	r2, r6
	ldr	r3, =0xffff3334
	str	r4, [sp]
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	ldr	r0, [r7, #8]
	ldr	r2, [r7, #0x10]
	mov	r3, #0xa0
	ldr	r4, =0x3333
	lsl	r3, #12
	ldr	r1, [r7, #0xc]
	add	r0, r3
	add	r2, r6
	ldr	r3, =0xffff0000
	str	r4, [sp]
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	add	sp, #0x10
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_926_2008bf4

@ 91 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked, WaitFrames, Cos, Sin
@   PlaySound, OvlFunc_common0_10c x3
.thumb_func_start OvlFunc_926_2008cd4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r0, #0x13
	sub	sp, #0x10
	bl	__MapActor_GetActor
	mov	r6, #8
	mov	r7, r0
.Lce6:
	ldr	r3, [r7, #0x50]
	mov	r0, #0xc
	lsl	r5, r6, #12
	sub	r0, r6
	strh	r5, [r3, #0x1e]
	lsl	r0, #1
	bl	__WaitFrames
	mov	r0, r5
	bl	__cos
	lsl	r2, r0, #1
	ldr	r3, [r7, #8]
	add	r2, r0
	lsl	r2, #1
	sub	r3, r2
	str	r3, [r7, #8]
	mov	r0, r5
	bl	__sin
	lsl	r2, r0, #1
	ldr	r3, [r7, #0x10]
	add	r2, r0
	lsl	r2, #1
	sub	r3, r2
	add	r6, #1
	str	r3, [r7, #0x10]
	cmp	r6, #0xc
	bls	.Lce6
	mov	r3, #0x90
	lsl	r3, #13
	str	r3, [r7, #0xc]
	str	r3, [r7, #0x3c]
	ldr	r3, =0xffff3334
	mov	r0, #0xe3
	str	r3, [r7, #0x18]
	bl	__PlaySound
	ldr	r0, [r7, #8]
	ldr	r3, =0xfff40000
	ldr	r2, [r7, #0x10]
	mov	r6, #0x80
	lsl	r6, #12
	ldr	r4, =0x3333
	add	r0, r3
	mov	r3, #0x80
	mov	r5, #0
	ldr	r1, [r7, #0xc]
	add	r2, r6
	lsl	r3, #9
	str	r4, [sp]
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	mov	r8, r4
	bl	OvlFunc_common0_10c
	ldr	r2, [r7, #0x10]
	ldr	r4, =0x4ccc
	ldr	r0, [r7, #8]
	ldr	r1, [r7, #0xc]
	add	r2, r6
	ldr	r3, =0xcccc
	str	r4, [sp]
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	ldr	r0, [r7, #8]
	mov	r3, #0xa0
	lsl	r3, #12
	ldr	r2, [r7, #0x10]
	add	r0, r3
	ldr	r3, =0x6666
	ldr	r1, [r7, #0xc]
	add	r2, r6
	str	r3, [sp]
	mov	r3, r8
	str	r5, [sp, #4]
	str	r5, [sp, #8]
	str	r5, [sp, #0xc]
	bl	OvlFunc_common0_10c
	add	sp, #0x10
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_926_2008cd4
