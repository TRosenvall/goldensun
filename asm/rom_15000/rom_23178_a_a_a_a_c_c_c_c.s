	.include "macros.inc"
	.include "gba.inc"

@ OpenSubScreenWindow
@ r0.. = parameters. Opens the sub-screen's window with CreateUIBox, sizing it
@ with Func_af0.
.thumb_func_start Func_8028808  @ 0x08028808
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001f38
	ldr	r7, [r3]
	mov	r3, #0x90
	add	r3, r7
	mov	r8, r3
	mov	r3, r7
	mov	r10, r0
	add	r1, #2
	mov	r5, r8
	add	r3, #0x92
	strh	r1, [r5]
	mov	r6, r10
	strh	r2, [r3]
	add	r3, #2
	strh	r6, [r3]
	mov	r1, #0x8e
	add	r1, r7
	mov	r2, #0
	ldrsh	r6, [r1, r2]
	mov	r9, r1
	mov	r1, r8
	mov	r3, #0
	ldrsh	r0, [r1, r3]
	mov	r1, #3
	lsl	r0, #1
	sub	sp, #4
	bl	__divsi3
	lsl	r5, r6, #1
	add	r5, r6
	add	r5, r0
	lsr	r3, r5, #31
	add	r5, r3
	asr	r5, #1
	mov	r3, #0xf
	mov	r1, #0
	sub	r0, r3, r5
	cmp	r1, r6
	bge	.L2887e
	mov	r2, r10
	lsl	r4, r2, #3
	mov	r12, r9
	mov	r2, r7
.L28868:
	lsl	r3, r0, #3
	strh	r3, [r2, #0xc]
	strh	r4, [r2, #0xe]
	mov	r6, r12
	mov	r5, #0
	ldrsh	r3, [r6, r5]
	add	r1, #1
	add	r0, #3
	add	r2, #0x14
	cmp	r1, r3
	blt	.L28868
.L2887e:
	mov	r3, r8
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	mov	r3, #2
	str	r3, [sp]
	mov	r1, r10
	mov	r3, #3
	bl	CreateUIBox
	str	r0, [r7, #0x78]
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8028808

@ OpenSubScreenWindowAlt
@ r0.. = parameters. As Func_28808 with fixed geometry.
.thumb_func_start Func_80288a8  @ 0x080288a8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r1
	ldr	r1, =iwram_3001f38
	ldr	r5, [r1]
	mov	r1, #0x90
	add	r1, r5
	mov	r14, r1
	add	r2, #2
	mov	r4, r14
	strh	r2, [r4]
	mov	r2, r5
	add	r2, #0x92
	strh	r3, [r2]
	mov	r3, r5
	add	r3, #0x94
	strh	r6, [r3]
	mov	r7, #0x8e
	add	r7, r5
	mov	r2, #0
	ldrsh	r3, [r7, r2]
	mov	r1, #0
	sub	sp, #4
	mov	r12, r7
	cmp	r1, r3
	bge	.L288fc
	lsl	r3, r6, #3
	mov	r8, r3
	mov	r2, r5
.L288e4:
	lsl	r3, r0, #3
	mov	r4, r8
	strh	r4, [r2, #0xe]
	strh	r3, [r2, #0xc]
	mov	r4, r12
	mov	r7, #0
	ldrsh	r3, [r4, r7]
	add	r1, #1
	add	r0, #3
	add	r2, #0x14
	cmp	r1, r3
	blt	.L288e4
.L288fc:
	mov	r1, r14
	mov	r3, #2
	mov	r7, #0
	ldrsh	r2, [r1, r7]
	str	r3, [sp]
	mov	r1, r6
	mov	r3, #3
	bl	CreateUIBox
	str	r0, [r5, #0x78]
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80288a8

@ RunFieldMenu
@ r0 = the previously chosen entry. Draws the field menu and returns which
@ entry the player picked, or a negative value when they backed out.
@
@ _Func_7a5bc(-1) decides how many entries there are: when the party summary
@ comes back empty the extra panel 0x0F is left out, so the menu is three rows
@ instead of four. The two byte tables .L37403 and .L373f7 translate between
@ the row on screen and the caller's entry number in each of those two shapes,
@ indexed by `previous + 6 * short`.
@
@ The panels are appended in order -- 1, then 0x0F when the party is present,
@ then 2 and 7 -- with Func_28808(0x11, 7, 0) placing the box, Func_28574
@ running the cursor and Func_2851c tearing it down.
@
@ Func_1c244 is the caller, and it dispatches the result 0..4 into
@ _Func_8ce74, _Func_a5b94, _Func_aa56c, _Func_a24d0 and _Func_a7478.
.thumb_func_start Func_8028920  @ 0x08028920
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r0, #1
	neg	r0, r0
	mov	r6, #0
	bl	_GetNumDjinn
	cmp	r0, #0
	bne	.L28934
	mov	r6, #1
.L28934:
	lsl	r3, r6, #1
	add	r3, r6
	ldr	r2, =.L37403
	lsl	r7, r3, #1
	add	r3, r5, r7
	ldrsb	r3, [r2, r3]
	sub	r5, r3, #1
	cmp	r5, #0
	bge	.L28948
	mov	r5, #0
.L28948:
	bl	Func_80284dc
	mov	r0, #1
	bl	AddMenuBarOption
	cmp	r6, #0
	bne	.L2895c
	mov	r0, #0xf
	bl	AddMenuBarOption
.L2895c:
	mov	r0, #2
	bl	AddMenuBarOption
	mov	r0, #7
	bl	AddMenuBarOption
	mov	r0, #0x11
	mov	r1, #7
	mov	r2, #0
	bl	Func_8028808
	mov	r0, r5
	bl	Func_8028574
	mov	r5, r0
	bl	Func_802851c
	cmp	r5, #0
	blt	.L2898a
	add	r3, r5, r7
	ldr	r2, =.L373f7
	add	r3, #1
	ldrsb	r5, [r2, r3]
.L2898a:
	mov	r0, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8028920
