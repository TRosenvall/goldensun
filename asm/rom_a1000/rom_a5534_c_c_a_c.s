	.include "macros.inc"

@ DrawShortcutRow
@ r0 = window. Draws the current field shortcuts. The heading is string 0xAE4
@ when both slots are occupied and 0xAE0 when either is empty. For each occupied
@ slot it prints the item name at 0x333 + (id & 0x3FF), registers the icon
@ through _Func_19908, adds label 0xAE7, and -- when the name is short enough
@ (the measured width from _Func_187ac is 10 or less) -- the owner's portrait
@ from _Func_77394(id >> 10) at x 0x50. 162 lines; traced structurally.
.thumb_func_start Func_80a6614  @ 0x080a6614
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =gState
	mov	r1, #0x88
	lsl	r1, #2
	add	r3, r2, r1
	ldrh	r3, [r3]
	sub	sp, #0x14
	mov	r5, r0
	cmp	r3, #0
	beq	.La664e
	add	r1, #2
	add	r3, r2, r1
	ldrh	r3, [r3]
	cmp	r3, #0
	beq	.La664e
	mov	r3, #8
	ldr	r0, =0xae4
	neg	r3, r3
	mov	r1, r5
	mov	r2, #0
	bl	_Func_801e7c0
	b	.La665c
.La664e:
	mov	r3, #8
	ldr	r0, =0xae0
	neg	r3, r3
	mov	r1, r5
	mov	r2, #0
	bl	_Func_801e7c0
.La665c:
	ldr	r3, =gState
	mov	r2, #0x88
	lsl	r2, #2
	add	r3, r2
	ldrh	r3, [r3]
	ldr	r0, =0x3ff
	and	r0, r3
	ldr	r3, =0x333
	add	r0, r3
	add	r3, sp, #0x10
	mov	r1, #0xc
	mov	r2, #8
	add	r1, sp
	add	r2, sp
	mov	r10, r3
	add	r3, sp, #4
	mov	r9, r1
	mov	r11, r2
	str	r3, [sp]
	mov	r8, r3
	mov	r1, r10
	mov	r3, r11
	mov	r2, r9
	bl	_TextBox
	ldr	r3, [sp, #8]
	mov	r6, #1
	cmp	r3, #0xa
	bhi	.La6698
	mov	r6, #0
.La6698:
	ldr	r3, =gState
	mov	r1, #0x88
	lsl	r1, #2
	add	r7, r3, r1
	ldrh	r2, [r7]
	mov	r3, r2
	cmp	r3, #0
	beq	.La66d6
	ldr	r0, =0x3ff
	mov	r1, #4
	and	r0, r2
	bl	_Func_8019908
	ldr	r0, =0xae7
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_801e7c0
	cmp	r6, #0
	bne	.La66e2
	ldrh	r0, [r7]
	lsr	r0, #10
	bl	_GetUnit
	mov	r1, r5
	mov	r2, #0x50
	mov	r3, #0
	bl	_Func_801e8b0
	b	.La66e2
.La66d6:
	ldr	r0, =0xae5
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_801e7c0
.La66e2:
	ldr	r3, =gState
	ldr	r2, =0x222
	add	r3, r2
	ldrh	r3, [r3]
	ldr	r0, =0x3ff
	and	r0, r3
	ldr	r3, =0x333
	add	r0, r3
	mov	r3, r8
	str	r3, [sp]
	mov	r1, r10
	mov	r3, r11
	mov	r2, r9
	bl	_TextBox
	ldr	r3, [sp, #8]
	mov	r6, #1
	cmp	r3, #0xa
	bhi	.La670a
	mov	r6, #0
.La670a:
	ldr	r3, =gState
	ldr	r1, =0x222
	add	r7, r3, r1
	ldrh	r2, [r7]
	mov	r3, r2
	cmp	r3, #0
	beq	.La674c
	ldr	r0, =0x3ff
	mov	r1, #4
	and	r0, r2
	bl	_Func_8019908
	ldr	r0, =0xae8
	mov	r1, r5
	mov	r2, #0
	mov	r3, #8
	bl	_Func_801e7c0
	cmp	r6, #0
	bne	.La6744
	ldrh	r0, [r7]
	lsr	r0, #10
	bl	_GetUnit
	mov	r1, r5
	mov	r2, #0x50
	mov	r3, #8
	bl	_Func_801e8b0
.La6744:
	mov	r0, #0xf
	bl	_SetTextColor
	b	.La6758
.La674c:
	ldr	r0, =0xae6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #8
	bl	_Func_801e7c0
.La6758:
	mov	r0, #1
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a6614

@ BuildItemScreenLayout
@ Takes no arguments. Builds the item screen's furniture: Func_a1814 and
@ Func_a1870 for the party strip, a (0, 5, 0x1E, 0xF) window into state+0x20,
@ a cursor sprite at state+0x44, the menu entries via _Func_1ec6c, and sixteen
@ panel sprites at y 0x60 stepping 0x10 -- the first eight at priority 8 and the
@ rest at 0x18, so the second row sorts behind the first.
.thumb_func_start Func_80a6794  @ 0x080a6794
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	mov	r10, r3
	mov	r0, r10
	sub	sp, #8
	bl	Func_80a1814
	mov	r5, #0
	mov	r1, #2
	mov	r2, #2
	mov	r3, #8
	str	r5, [sp]
	bl	Func_80a1870
	mov	r6, #2
	mov	r1, #5
	mov	r2, #0x1e
	mov	r3, #0xf
	mov	r0, #0
	str	r6, [sp]
	bl	_CreateUIBox
	mov	r3, #0x88
	lsl	r3, #1
	mov	r2, r10
	add	r3, r10
	str	r0, [r2, #0x20]
	strb	r5, [r3]
	ldr	r3, =0x111
	mov	r2, #0x89
	add	r3, r10
	lsl	r2, #1
	strb	r5, [r3]
	add	r2, r10
	mov	r3, #8
	strb	r3, [r2]
	ldr	r3, =0x113
	add	r3, r10
	strb	r6, [r3]
	mov	r1, #0
	mov	r2, #4
	mov	r8, r0
	bl	Func_80a1778
	mov	r3, #0xd
	strb	r3, [r0, #5]
	mov	r3, r10
	str	r0, [r3, #0x44]
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	mov	r3, r8
	str	r5, [sp]
	str	r5, [sp, #4]
	mov	r6, r10
	bl	_Func_801ec6c
	mov	r2, #8
	mov	r9, r2
	add	r6, #0x48
	mov	r7, #0x60
.La6818:
	mov	r3, r9
	str	r3, [sp]
	mov	r1, r5
	mov	r3, r7
	mov	r0, #4
	mov	r2, r8
	bl	_Func_801eb64
	add	r5, #1
	stmia	r6!, {r0}
	add	r7, #0x10
	cmp	r5, #7
	ble	.La6818
	mov	r2, #0x18
	mov	r6, r10
	mov	r5, #8
	mov	r9, r2
	add	r6, #0x68
	mov	r7, #0x60
.La683e:
	mov	r3, r9
	str	r3, [sp]
	mov	r1, r5
	mov	r3, r7
	mov	r0, #4
	mov	r2, r8
	bl	_Func_801eb64
	add	r5, #1
	stmia	r6!, {r0}
	add	r7, #0x10
	cmp	r5, #0xf
	ble	.La683e
	add	sp, #8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a6794
