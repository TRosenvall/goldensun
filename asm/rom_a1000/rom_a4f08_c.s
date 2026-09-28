	.include "macros.inc"
	.include "gba.inc"

@ DrawSelectedItemRow
@ Takes no arguments. Draws the highlighted item across the top of the detail
@ window: the icon sprite at (0x70, 8) via _Func_1bcd4 into the node at
@ state+0x21C, the owner's portrait from _Func_77394 at (0x10, 0), and the item
@ name -- STRING 0x182 + itemId, the base for every item name in the game -- at
@ (0x10, 8).
.thumb_func_start Func_80a51d0  @ 0x080a51d0
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001f2c
	ldr	r7, [r3]
	mov	r3, #0x87
	lsl	r3, #2
	add	r5, r7, r3
	mov	r2, #0xbc
	ldr	r3, [r5]
	lsl	r2, #1
	add	r6, r7, r2
	ldrh	r1, [r6]
	ldrb	r2, [r3, #0xe]
	mov	r0, #2
	mov	r3, #0
	bl	_Func_801bcd4
	ldr	r2, [r5]
	mov	r3, #1
	strb	r3, [r2, #5]
	ldr	r2, [r5]
	mov	r3, #0x70
	strh	r3, [r2, #6]
	ldr	r2, [r5]
	mov	r3, #8
	strh	r3, [r2, #8]
	ldr	r0, [r5]
	bl	Func_80a17c4
	ldr	r2, =0x21a
	add	r3, r7, r2
	ldrb	r0, [r3]
	bl	_GetUnit
	mov	r3, #0x86
	lsl	r3, #1
	add	r5, r7, r3
	ldr	r1, [r5]
	mov	r2, #0x10
	mov	r3, #0
	bl	_Func_801e8b0
	ldrh	r3, [r6]
	ldr	r0, =0x1ff
	and	r0, r3
	ldr	r3, =0x182
	ldr	r1, [r5]
	add	r0, r3
	mov	r2, #0x10
	mov	r3, #8
	bl	_Func_801e7c0
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a51d0

@ ConfirmItemAction
@ r0 = ability id. Opens a confirmation window at (0xD, 3, 0x11, 0xA), draws the
@ item name (0x182 + id), the prompt pair 0xAD4 / 0xAD5 and the two choices
@ 0xB2C / 0xB2D, and runs a two-row cursor: Func_a1ac0 places it at (0x68, 0x56)
@ and Up and Down move it by 16 pixels with sound 0x6F. Exits on A, B or save
@ bit 0x150. 132 lines; the loop is traced, the drawing structurally.
.thumb_func_start Func_80a524c  @ 0x080a524c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #4
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #3
	mov	r2, #0x11
	mov	r5, r0
	mov	r3, #0xa
	mov	r0, #0xd
	bl	_CreateUIBox
	ldr	r3, =0x1ff
	and	r5, r3
	mov	r7, r0
	mov	r0, r5
	bl	_GetItemInfo
	ldr	r3, =0x182
	add	r5, r3
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0x18
	mov	r3, #0
	bl	_Func_801e7c0
	ldr	r5, =0xad4
	mov	r1, r7
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0x10
	add	r5, #1
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x18
	bl	_Func_801e7c0
	ldr	r5, =0xb2c
	mov	r1, r7
	mov	r0, r5
	mov	r2, #0x18
	mov	r3, #0x28
	add	r5, #1
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0x18
	mov	r3, #0x38
	bl	_Func_801e7c0
	mov	r6, #1
	mov	r0, #0x68
	mov	r1, #0x56
	mov	r8, r6
	bl	Func_80a1ac0
	b	.La5306
.La52c8:
	lsl	r1, r6, #4
	add	r1, #0x46
	mov	r0, #0x68
	bl	Func_80a1a40
	ldr	r5, =gKeyRepeat
	ldr	r3, [r5]
	mov	r2, #0x40
	and	r3, r2
	cmp	r3, #0
	beq	.La52ea
	mov	r2, #1
	mov	r0, #0x6f
	sub	r6, #1
	mov	r8, r2
	bl	_PlaySound
.La52ea:
	ldr	r3, [r5]
	mov	r2, #0x80
	and	r3, r2
	cmp	r3, #0
	beq	.La5300
	mov	r3, #1
	mov	r0, #0x6f
	add	r6, #1
	mov	r8, r3
	bl	_PlaySound
.La5300:
	mov	r0, #1
	bl	WaitFrames
.La5306:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La534c
	mov	r2, r8
	cmp	r2, #0
	beq	.La5326
	mov	r3, #0
	add	r0, r6, #2
	mov	r1, #2
	mov	r8, r3
	bl	__modsi3
	mov	r6, r0
.La5326:
	ldr	r1, =gKeyPress
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La533a
	mov	r0, #0x70
	bl	_PlaySound
	b	.La534c
.La533a:
	ldr	r3, [r1]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.La52c8
	mov	r0, #0x71
	bl	_PlaySound
	mov	r6, #1
.La534c:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.La535a
	mov	r6, #1
.La535a:
	mov	r0, r7
	mov	r1, #1
	bl	_CloseUIBox
	mov	r0, r6
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a524c

@ RunEquipPreview
@ Takes no arguments. Equips the item at state+0x176 onto the character at
@ state+0x21B, but shows the result before committing: Func_a3ef0 builds the
@ preview, the record is copied to a 0x14C scratch and _Func_78708 applies the
@ change. A return of -2 or -1 means the equip is impossible -- it plays sound
@ 0xAF and leaves. Otherwise it draws labels 0xB2C / 0xB2D and 0xAD6, clears the
@ comparison area with _Func_164d4, and runs a cursor from (0x6E, 0x20) down in
@ 0x30-pixel steps over the party. 188 lines; traced structurally.
.thumb_func_start Func_80a5388  @ 0x080a5388
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r3, #0
	sub	sp, #0xc
	mov	r8, r3
	mov	r3, #1
	str	r3, [sp, #8]
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	ldr	r6, =0x21b
	mov	r9, r3
	add	r6, r9
	ldrb	r0, [r6]
	bl	_GetUnit
	mov	r3, #0xbb
	str	r0, [sp, #4]
	lsl	r3, #1
	add	r3, r9
	ldrh	r1, [r3]
	mov	r10, r3
	ldrb	r3, [r6]
	mov	r5, #0xa6
	mov	r0, r3
	mov	r2, #0
	lsl	r5, #1
	bl	Func_80a3ef0
	mov	r0, r5
	bl	Func_8004938
	ldr	r3, =Func_8001af8
	ldr	r1, [sp, #4]
	mov	r2, r5
	mov	r11, r0
	bl	_call_via_r3
	mov	r3, #0x86
	lsl	r3, #1
	add	r3, r9
	ldr	r7, [r3]
	mov	r3, r10
	ldrb	r0, [r6]
	ldrh	r1, [r3]
	bl	_EquipItem
	add	r0, #2
	cmp	r0, #1
	bhi	.La53fe
	b	.La54c6
.La53f6:
	mov	r0, #0xaf
	bl	_PlaySound
	b	.La54ca
.La53fe:
	ldr	r5, =0xb2c
	mov	r1, r7
	mov	r0, r5
	mov	r2, #0x18
	mov	r3, #0x18
	add	r5, #1
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0x48
	mov	r3, #0x18
	bl	_Func_801e7c0
	mov	r3, #0x18
	str	r3, [sp]
	mov	r0, r7
	mov	r1, #0x10
	mov	r2, #0x10
	mov	r3, #0x60
	bl	_Func_80164d4
	mov	r1, r7
	ldr	r0, =0xad6
	mov	r2, #0
	mov	r3, #0x10
	bl	_Func_801e7c0
	mov	r0, #0x6e
	mov	r1, #0x20
	bl	Func_80a1ac0
	b	.La5488
.La5440:
	mov	r3, r8
	lsl	r0, r3, #1
	add	r0, r8
	lsl	r0, #4
	add	r0, #0x6e
	mov	r1, #0x20
	bl	Func_80a1a40
	ldr	r5, =gKeyRepeat
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.La546c
	mov	r3, #1
	neg	r3, r3
	add	r8, r3
	mov	r0, #0x6f
	mov	r3, #1
	str	r3, [sp, #8]
	bl	_PlaySound
.La546c:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.La5482
	mov	r3, #1
	mov	r0, #0x6f
	add	r8, r3
	str	r3, [sp, #8]
	bl	_PlaySound
.La5482:
	mov	r0, #1
	bl	WaitFrames
.La5488:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La54ca
	ldr	r3, [sp, #8]
	cmp	r3, #0
	beq	.La54aa
	mov	r0, r8
	mov	r3, #0
	add	r0, #2
	mov	r1, #2
	str	r3, [sp, #8]
	bl	__modsi3
	mov	r8, r0
.La54aa:
	ldr	r1, =gKeyPress
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	bne	.La53f6
	ldr	r3, [r1]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.La5440
	mov	r0, #0x71
	bl	_PlaySound
.La54c6:
	mov	r3, #1
	mov	r8, r3
.La54ca:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.La54da
	mov	r3, #1
	mov	r8, r3
.La54da:
	mov	r3, r8
	cmp	r3, #1
	bne	.La54ee
	mov	r2, #0xa6
	ldr	r3, =Func_8001af8
	ldr	r0, [sp, #4]
	mov	r1, r11
	lsl	r2, #1
	bl	_call_via_r3
.La54ee:
	ldr	r5, =0x21b
	mov	r0, r11
	add	r5, r9
	bl	free
	ldrb	r0, [r5]
	bl	_CalcStats
	ldrb	r0, [r5]
	bl	_Func_8078bf0
	mov	r0, r8
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a5388

	.section .rodata

	.global .Laf08c
.Laf08c:
	.incrom 0xaf08c, 0xaf20c
