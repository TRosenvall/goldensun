	.include "macros.inc"
	.include "gba.inc"

.thumb_func_Start Func_a1c6c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r1
	mov	r8, r2
	mov	r7, r3
	cmp	r6, #0xf
	ble	.La1c7e
	mov	r6, #0
.La1c7e:
	ldr	r1, [sp, #0x14]
	ldr	r5, [r0]
	mov	r0, r6
	bl	__divsi3
	lsl	r0, #4
	add	r0, r7
	strh	r0, [r5, #8]
	ldr	r1, [sp, #0x14]
	mov	r0, r6
	bl	__modsi3
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #3
	add	r3, r8
	strh	r3, [r5, #6]
	mov	r0, r5
	bl	Func_80a17c4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1c6c

@ LayOutGridWide
@ r0 = 1 to start at row 0x38, anything else 0x28. Lays the first 15 nodes out
@ five to a row at x 0x74 through Func_a1c6c.
.thumb_func_start Func_80a1cb0  @ 0x080a1cb0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f2c
	mov	r2, #0x38
	sub	sp, #4
	ldr	r3, [r3]
	mov	r8, r2
	cmp	r0, #1
	beq	.La1cca
	mov	r2, #0x28
	mov	r8, r2
.La1cca:
	mov	r5, r3
	add	r5, #0x48
	mov	r3, #5
	mov	r6, #0
	mov	r7, r5
	mov	r10, r3
.La1cd6:
	ldmia	r7!, {r3}
	cmp	r3, #0
	beq	.La1cec
	mov	r2, r10
	str	r2, [sp]
	mov	r0, r5
	mov	r1, r6
	mov	r2, #0x74
	mov	r3, r8
	bl	Func_a1c6c
.La1cec:
	add	r6, #1
	add	r5, #4
	cmp	r6, #0xe
	ble	.La1cd6
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a1cb0

@ ShowMessage
@ r0 = string id, r1 = -1 to return immediately, r2 = -1 to reuse the window
@ already at state+0x2C rather than opening one.
@
@ Hides the cursor, and unless r2 is -1 measures the string with _Func_187ac
@ and opens a window of exactly that size at state+0x3C (resizing an existing
@ one through .gcc2_compiled. rather than reopening it). Renders with _Func_1e7c0
@ when reusing the shared window and _Func_1e74c otherwise.
@
@ When r1 is -1 it sets save bit 0x151 and returns with the box still up --
@ that is how a screen leaves a message on screen while it does something else.
@ Otherwise it spins on WaitFrames(1) until A, B or Start is newly pressed, then
@ tears the window down.
@
@ Either way it raises state+0x222 before returning, so the next Func_a1ac0
@ snaps the cursor instead of gliding it across a screen that has just changed.
.thumb_func_start Func_80a1d08  @ 0x080a1d08
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f2c
	ldr	r7, [r3]
	mov	r8, r2
	ldr	r2, [r7, #0x14]
	mov	r3, #0xd
	strb	r3, [r2, #5]
	mov	r2, #1
	neg	r2, r2
	sub	sp, #0x18
	mov	r6, r0
	mov	r10, r1
	cmp	r8, r2
	beq	.La1d6c
	add	r0, sp, #8
	add	r1, sp, #0x14
	add	r2, sp, #0x10
	add	r3, sp, #0xc
	str	r0, [sp]
	mov	r0, r6
	bl	_TextBox
	ldr	r2, [sp, #8]
	mov	r5, r7
	str	r2, [sp]
	mov	r2, #0x81
	lsl	r2, #1
	add	r5, #0x3c
	str	r2, [sp, #4]
	ldr	r3, [sp, #0xc]
	mov	r0, r5
	mov	r1, r10
	mov	r2, r8
	bl	Func_80a10d0
	cmp	r0, #0
	bne	.La1d68
	ldr	r2, [sp, #8]
	ldr	r0, [r5]
	ldr	r3, [sp, #0xc]
	str	r2, [sp]
	mov	r1, r10
	mov	r2, r8
	bl	Func_80a23f4
.La1d68:
	ldr	r5, [r5]
	b	.La1d6e
.La1d6c:
	ldr	r5, [r7, #0x2c]
.La1d6e:
	mov	r0, r5
	bl	_Func_8016498
	mov	r0, r5
	bl	_Func_80164ac
	mov	r3, #1
	neg	r3, r3
	cmp	r8, r3
	bne	.La1d90
	mov	r0, r6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_801e7c0
	b	.La1d9c
.La1d90:
	mov	r0, r6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_DrawSmallText
.La1d9c:
	mov	r2, #1
	neg	r2, r2
	cmp	r10, r2
	beq	.La1df4
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #1
	ldr	r6, =gKeyPress
	mov	r10, r3
.La1db0:
	mov	r0, #1
	bl	WaitFrames
	ldr	r3, [r6]
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	bne	.La1dd4
	ldr	r3, [r6]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	bne	.La1dd4
	ldr	r3, [r6]
	mov	r2, #8
	and	r3, r2
	cmp	r3, #0
	beq	.La1db0
.La1dd4:
	mov	r3, #1
	neg	r3, r3
	cmp	r8, r3
	bne	.La1de2
	mov	r0, r5
	bl	_Func_8016498
.La1de2:
	mov	r0, r5
	bl	_Func_80164ac
	b	.La1dfa

	.pool_aligned

.La1df4:
	ldr	r0, =0x151
	bl	_SetFlag
.La1dfa:
	ldr	r3, =0x222
	add	r2, r7, r3
	mov	r3, #1
	strh	r3, [r2]
	ldr	r1, .La1e1c	@ 1
	ldr	r3, [r7, #0x14]
	mov	r2, #1
	neg	r2, r2
	strb	r1, [r3, #5]
	cmp	r8, r2
	beq	.La1e28
	mov	r0, r7
	add	r0, #0x3c
	mov	r1, #1
	bl	Func_80a1114
	b	.La1e28

	.align	2, 0
.La1e1c:
	.word	1
	.pool

.La1e28:
	add	sp, #0x18
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a1d08

@ SortAbilityListByCategory
@ r0 = array of 15 ability halfwords, r1 = category order (0, 1, 2 or other).
@ Sorts the list in place into the order Func_a1f74 supplies. It copies the
@ list to a scratch, counts the non-zero entries, then for each category byte
@ in turn scans the scratch for the entry whose ability record +0x02 equals the
@ category's low 7 bits, keeping the LOWEST ability id among the matches, moves
@ it to the output and blanks it in the scratch. Bit 7 of a category byte adds
@ a second condition: the entry must also have bit 9 set. The 0xFF byte ends
@ the category list.
@
@ A plain selection sort, but the key is the category table rather than the
@ value, which is why the menus group Psynergy by element rather than by id.
.thumb_func_start Func_80a1e38  @ 0x080a1e38
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x70
	str	r0, [sp, #0xc]
	add	r5, sp, #0x10
	mov	r0, #0
	str	r0, [sp, #8]
	mov	r11, r0
	mov	r0, r1
	mov	r1, r5
	bl	Func_a1f74
	add	r1, sp, #0x30
	mov	r9, r1
	mov	r2, #0
	mov	r6, #0xe
.La1e62:
	ldr	r0, [sp, #0xc]
	ldrh	r3, [r2, r0]
	sub	r6, #1
	strh	r3, [r2, r1]
	add	r2, #2
	cmp	r6, #0
	bge	.La1e62
	mov	r1, #0
	mov	r8, r1
	mov	r2, r9
	mov	r6, #0xe
.La1e78:
	ldrh	r3, [r2]
	add	r2, #2
	cmp	r3, #0
	beq	.La1e84
	mov	r3, #1
	add	r8, r3
.La1e84:
	sub	r6, #1
	cmp	r6, #0
	bge	.La1e78
	mov	r0, r8
	cmp	r0, #0xe
	bgt	.La1eac
	add	r3, sp, #0x50
	lsl	r2, r0, #1
	add	r2, r3
	ldr	r1, =0
	mov	r3, #0xf
	sub	r6, r3, r0
.La1e9c:
	sub	r6, #1
	strh	r1, [r2]
	add	r2, #2
	cmp	r6, #0
	bne	.La1e9c
	b	.La1eac

	.pool_aligned

.La1eac:
	ldrb	r3, [r5]
	cmp	r3, #0xff
	beq	.La1f44
	mov	r1, sp
	add	r1, #0x50
	str	r1, [sp, #4]
	mov	r10, r9
	mov	r7, r5
.La1ebc:
	mov	r6, #0
	mov	r4, #0
	cmp	r6, r8
	bge	.La1f12
	mov	r5, r9
.La1ec6:
	ldrh	r3, [r5]
	cmp	r3, #0
	beq	.La1f0a
	mov	r0, r3
	str	r4, [sp]
	bl	_GetItemInfo
	ldrb	r1, [r7]
	mov	r2, #0x7f
	ldrb	r3, [r0, #2]
	and	r2, r1
	ldr	r4, [sp]
	cmp	r2, r3
	bne	.La1f0a
	mov	r3, #0x80
	and	r3, r1
	cmp	r3, #0
	beq	.La1efc
	ldrh	r2, [r5]
	ldr	r3, =0x200
	and	r3, r2
	cmp	r3, #0
	beq	.La1f0a
	b	.La1efe

	.pool_aligned

.La1efc:
	ldrh	r2, [r5]
.La1efe:
	ldr	r3, =0x1ff
	and	r3, r2
	cmp	r4, r3
	bge	.La1f0a
	str	r6, [sp, #8]
	mov	r4, r3
.La1f0a:
	add	r6, #1
	add	r5, #2
	cmp	r6, r8
	blt	.La1ec6
.La1f12:
	cmp	r4, #0
	beq	.La1f3c
	ldr	r0, [sp, #8]
	mov	r3, r11
	lsl	r2, r0, #1
	mov	r0, r10
	lsl	r1, r3, #1
	ldrh	r3, [r0, r2]
	ldr	r0, [sp, #4]
	strh	r3, [r0, r1]
	ldr	r3, =0
	mov	r1, r10
	b	.La1f34

	.pool_aligned

.La1f34:
	strh	r3, [r1, r2]
	mov	r3, #1
	add	r11, r3
	b	.La1ebc
.La1f3c:
	add	r7, #1
	ldrb	r3, [r7]
	cmp	r3, #0xff
	bne	.La1ebc
.La1f44:
	mov	r0, r8
	cmp	r0, #0
	ble	.La1f5e
	add	r1, sp, #0x50
	mov	r2, #0
	mov	r6, r8
.La1f50:
	ldrh	r3, [r2, r1]
	ldr	r0, [sp, #0xc]
	sub	r6, #1
	strh	r3, [r2, r0]
	add	r2, #2
	cmp	r6, #0
	bne	.La1f50
.La1f5e:
	mov	r0, #1
	add	sp, #0x70
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a1e38

.thumb_Func_start Func_a1f74
	push	{lr}
	ldr	r2, =.Laf2a6
	cmp	r0, #1
	beq	.La1f90
	cmp	r0, #1
	bgt	.La1f86
	cmp	r0, #0
	beq	.La1f8c
	b	.La1f96
.La1f86:
	cmp	r0, #2
	beq	.La1f94
	b	.La1f96
.La1f8c:
	ldr	r2, =.Laf2d0
	b	.La1f96
.La1f90:
	ldr	r2, =.Laf2bc
	b	.La1f96
.La1f94:
	ldr	r2, =.Laf2b1
.La1f96:
	ldrb	r3, [r2]
	mov	r4, #0xff
	strb	r3, [r1]
	lsl	r4, #24
	lsl	r3, #24
	mov	r0, #0
	cmp	r3, r4
	beq	.La1fbe
.La1fa6:
	add	r0, #1
	cmp	r0, #0x1f
	bgt	.La1fbe
	add	r2, #1
	ldrb	r3, [r2]
	add	r1, #1
	mov	r4, #0xff
	strb	r3, [r1]
	lsl	r4, #24
	lsl	r3, #24
	cmp	r3, r4
	bne	.La1fa6
.La1fbe:
	pop	{r0}
	bx	r0
.func_end Func_a1f74

@ StepGridCursor
@ r0 = 0 to swap the axes, r1 = total entries, r2 = columns, r3 = column
@ pointer, arg5 = row pointer. The shared d-pad handler for every grid in this
@ module. Rows are total / columns rounded up.
@
@ The four direction bits are read from iwram_1b04, so this honours auto-repeat.
@ With r0 non-zero they map the obvious way (0x10 Right, 0x20 Left, 0x40 Up,
@ 0x80 Down); with r0 zero the pairs are exchanged, which is how a list that
@ scrolls horizontally reuses the same code.
@
@ Any accepted move plays sound 0x6F. Both indices wrap rather than stopping,
@ and after a vertical move the column is clamped so the cursor cannot land
@ past the end of a short last row. Func_352c resets the repeat delay on a
@ vertical move only.
@
@ Returns 1 for a vertical move, 0 for a horizontal one and -1 when nothing was
@ pressed -- so callers use `> 0` to mean "the page changed, redraw".
.thumb_func_start Func_80a1fd4  @ 0x080a1fd4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r8, r1
	ldr	r1, [sp, #0x1c]
	mov	r5, r0
	mov	r7, r2
	mov	r0, #1
	mov	r2, r8
	mov	r6, r3
	mov	r10, r1
	neg	r0, r0
	cmp	r2, #0
	bne	.La1ff6
	b	.La212e
.La1ff6:
	ldr	r0, =0x6002500
	bl	_Func_80219c8
	mov	r1, r7
	mov	r0, r8
	bl	__divsi3
	mov	r1, r7
	mov	r9, r0
	mov	r0, r8
	bl	__modsi3
	cmp	r0, #0
	beq	.La2016
	mov	r3, #1
	add	r9, r3
.La2016:
	cmp	r5, #0
	beq	.La2034
	ldr	r2, =gKeyRepeat
	ldr	r4, [r2]
	mov	r3, #0x10
	ldr	r1, [r2]
	and	r4, r3
	ldr	r5, [r2]
	mov	r3, #0x20
	and	r1, r3
	ldr	r2, [r2]
	mov	r3, #0x40
	and	r5, r3
	mov	r3, #0x80
	b	.La204c
.La2034:
	ldr	r2, =gKeyRepeat
	ldr	r4, [r2]
	mov	r3, #0x80
	ldr	r1, [r2]
	and	r4, r3
	ldr	r5, [r2]
	mov	r3, #0x40
	and	r1, r3
	ldr	r2, [r2]
	mov	r3, #0x20
	and	r5, r3
	mov	r3, #0x10
.La204c:
	and	r2, r3
	cmp	r5, #0
	beq	.La2084
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r1, r10
	ldr	r3, [r1]
	sub	r3, #1
	str	r3, [r1]
	cmp	r3, #0
	bge	.La206a
	mov	r3, r9
	sub	r3, #1
	str	r3, [r1]
.La206a:
	mov	r2, r10
	ldr	r3, [r2]
	mov	r0, r7
	mul	r0, r3
	ldr	r3, [r6]
	mov	r2, r8
	add	r3, r0
	sub	r2, #1
	cmp	r3, r2
	ble	.La20c6
	mov	r1, r8
	sub	r3, r1, r0
	b	.La20ba
.La2084:
	cmp	r2, #0
	beq	.La20ce
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r2, r10
	ldr	r3, [r2]
	add	r3, #1
	str	r3, [r2]
	mov	r2, r9
	sub	r2, #1
	cmp	r3, r2
	ble	.La20a2
	mov	r3, r10
	str	r5, [r3]
.La20a2:
	mov	r1, r10
	ldr	r3, [r1]
	mov	r0, r7
	mul	r0, r3
	ldr	r3, [r6]
	mov	r2, r8
	add	r3, r0
	sub	r2, #1
	cmp	r3, r2
	ble	.La20c6
	mov	r2, r8
	sub	r3, r2, r0
.La20ba:
	sub	r3, #1
	sub	r1, r7, #1
	str	r3, [r6]
	cmp	r3, r1
	ble	.La20c6
	str	r1, [r6]
.La20c6:
	bl	Func_800352c
	mov	r0, #1
	b	.La212e
.La20ce:
	cmp	r1, #0
	beq	.La20fc
	mov	r0, #0x6f
	bl	_PlaySound
	ldr	r3, [r6]
	sub	r3, #1
	str	r3, [r6]
	cmp	r3, #0
	bge	.La212c
	sub	r2, r7, #1
	str	r2, [r6]
	mov	r1, r10
	ldr	r3, [r1]
	mul	r3, r7
	mov	r1, r8
	sub	r3, r1, r3
	sub	r3, #1
	str	r3, [r6]
	cmp	r3, r2
	ble	.La212c
	str	r2, [r6]
	b	.La212c
.La20fc:
	mov	r0, #1
	neg	r0, r0
	cmp	r4, #0
	beq	.La212e
	mov	r0, #0x6f
	bl	_PlaySound
	ldr	r2, [r6]
	add	r2, #1
	str	r2, [r6]
	mov	r1, r10
	ldr	r3, [r1]
	mul	r3, r7
	mov	r1, r8
	sub	r3, r1, r3
	mov	r0, #0
	cmp	r2, r3
	bne	.La2122
	str	r0, [r6]
.La2122:
	ldr	r3, [r6]
	sub	r2, r7, #1
	cmp	r3, r2
	ble	.La212c
	str	r0, [r6]
.La212c:
	mov	r0, #0
.La212e:
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a1fd4

@ LoadMenuPalette
@ r0 = OBJ palette bank. DMA3-copies the 16 colours at 0x50001E0 into that
@ bank, queues a second eight-halfword copy for the next VBlank, and then
@ brightens the bank's colour 4 by nine steps on each of red, green and blue,
@ clamped at 31. That brightened entry is the highlight the cursor and the
@ selected row are drawn in.
.thumb_func_start Func_80a2144  @ 0x080a2144
	push	{r5, lr}
	mov	r3, #0xa0
	lsl	r0, #5
	lsl	r3, #19
	add	r5, r0, r3
	mov	r1, r5
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x50001e0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r0, =0x50001e0
	ldr	r2, =0x84000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldrh	r2, [r5, #8]
	lsl	r3, r2, #16
	lsr	r4, r3, #26
	lsr	r1, r3, #21
	ldr	r3, .La218c	@ 0x1f
	mov	r0, #0x1f
	add	r4, #9
	and	r1, r3
	and	r0, r2
	cmp	r4, #0x1f
	bls	.La217a
	mov	r4, #0x1f
.La217a:
	add	r1, #9
	cmp	r1, #0x1f
	bls	.La2182
	mov	r1, #0x1f
.La2182:
	add	r0, #9
	cmp	r0, #0x1f
	bls	.La21a0
	mov	r0, #0x1f
	b	.La21a0

	.align	2, 0
.La218c:
	.word	0x1f
	.pool

.La21a0:
	lsl	r3, r4, #10
	lsl	r2, r1, #5
	orr	r3, r2
	orr	r3, r0
	strh	r3, [r5, #8]
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_80a2144

@ DrawPageIndicator
@ r0 = window, r1 = total entries, r2 = per page, r3 = current page,
@ arg5 = right-hand column. Draws the segmented page bar along the window's
@ first interior row when there is more than one page.
@
@ Pages are total / perPage rounded up. The bar occupies the columns ending at
@ arg5: cap tile 0xF128, then one tile per page taken from 0x31 upward, then
@ cap 0xF129. The segment matching r3 is written in palette bank 2 and the rest
@ in bank 3, which is the whole highlight.
.thumb_func_start Func_80a21b0  @ 0x080a21b0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r5, r1
	mov	r6, r2
	sub	sp, #4
	mov	r1, r6
	mov	r9, r0
	mov	r11, r3
	mov	r0, r5
	mov	r3, #0x31
	ldr	r7, [sp, #0x24]
	mov	r10, r3
	bl	__divsi3
	mov	r1, r6
	mov	r8, r0
	mov	r0, r5
	bl	__modsi3
	cmp	r0, #0
	beq	.La21e8
	mov	r3, #1
	add	r8, r3
.La21e8:
	mov	r3, r8
	sub	r7, r3
	cmp	r3, #1
	ble	.La224c
	mov	r0, #0
	mov	r3, #1
	str	r0, [sp]
	ldr	r1, =0xf128
	sub	r2, r7, #1
	neg	r3, r3
	mov	r0, r9
	mov	r5, #0
	bl	_Func_8019000
	cmp	r5, r8
	bge	.La223a
.La2208:
	cmp	r5, r11
	bne	.La221e
	mov	r3, #2
	str	r3, [sp]
	mov	r0, r9
	mov	r1, r10
	mov	r2, r7
	sub	r3, #3
	bl	_Func_8019000
	b	.La222e
.La221e:
	mov	r3, #3
	str	r3, [sp]
	mov	r0, r9
	mov	r1, r10
	mov	r2, r7
	sub	r3, #4
	bl	_Func_8019000
.La222e:
	mov	r3, #1
	add	r5, #1
	add	r10, r3
	add	r7, #1
	cmp	r5, r8
	blt	.La2208
.La223a:
	mov	r2, #0
	mov	r3, #1
	str	r2, [sp]
	ldr	r1, =0xf129
	neg	r3, r3
	mov	r0, r9
	mov	r2, r7
	bl	_Func_8019000
.La224c:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a21b0

@ TintTilemapRect
@ r0 = window, r1 = x, r2 = y, r3 = width, arg5 = height, arg6 = palette bank.
@ ORs the bank into bits 12..15 of every tilemap entry in the rectangle, which
@ recolours what is already drawn without touching the tile indices. Coordinates
@ are relative to the window's own origin and clipped to the 30x20 map, and the
@ dirty byte at [iwram_1e8c]+0xEA3 is raised so the next frame uploads it.
.thumb_func_start Func_80a2268  @ 0x080a2268
	push	{r5, r6, r7, lr}
	mov	r6, r3
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r12, r3
	ldrh	r3, [r0, #0xc]
	add	r3, r1, r3
	add	r1, r3, #1
	ldrh	r3, [r0, #0xe]
	ldr	r7, [sp, #0x14]
	add	r3, r2, r3
	ldr	r5, [sp, #0x10]
	add	r2, r3, #1
	lsl	r7, #12
	cmp	r1, #0
	bge	.La228c
	add	r6, r1
	mov	r1, #0
.La228c:
	add	r3, r1, r6
	cmp	r3, #0x1d
	ble	.La2296
	mov	r3, #0x1e
	sub	r6, r3, r1
.La2296:
	cmp	r2, #0
	bge	.La229e
	add	r5, r2
	mov	r2, #0
.La229e:
	add	r3, r2, r5
	cmp	r3, #0x1d
	ble	.La22a8
	mov	r3, #0x14
	sub	r5, r3, r2
.La22a8:
	cmp	r6, #0
	ble	.La22e2
	cmp	r5, #0
	ble	.La22e2
	lsl	r2, #6
	lsl	r3, r1, #1
	add	r1, r2, r3
.La22b6:
	mov	r3, r12
	mov	r0, r6
	add	r4, r1, r3
	cmp	r0, #0
	beq	.La22d2
	ldr	r2, =0xffffefff
.La22c2:
	ldrh	r3, [r4]
	and	r3, r2
	orr	r3, r7
	sub	r0, #1
	strh	r3, [r4]
	add	r4, #2
	cmp	r0, #0
	bne	.La22c2
.La22d2:
	sub	r5, #1
	add	r1, #0x40
	cmp	r5, #0
	bne	.La22b6
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r12
	strb	r3, [r2]
.La22e2:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a2268

@ SyncObjPaletteToBg
@ Takes no arguments. DMA3-copies OBJ palette bank 0 (0x5000200) down into BG
@ bank 14 (0x50001C0), plus one further colour. Keeps the text drawn into the
@ tilemap the same colours as the sprites drawn over it.
.thumb_func_start Func_80a22f4  @ 0x080a22f4
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	ldr	r1, =0x50001c0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001e8
	ldr	r2, =0x80000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bx	lr
.func_end Func_80a22f4

@ ShowNodeRun
@ r0 = count, r1 = first index, r2 = unused, r3 = x, arg5 = y.
@ Hides all 32 nodes at state+0x48 by setting each one's +0x05 to 0x0D, then
@ walks `count` of them from `first`, placing each at x and a y that steps down
@ by 0x10, rewinding it with Func_a17c4 and marking it live. Stops early on a
@ null slot or once the index passes the visible-row count at state+0x218.
.thumb_func_start Func_80a2324  @ 0x080a2324
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r9, r3
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	sub	sp, #4
	mov	r8, r3
	mov	r2, #0xd
	add	r3, #0x48
	mov	r6, #0x1f
.La233e:
	ldmia	r3!, {r5}
	cmp	r5, #0
	beq	.La2346
	strb	r2, [r5, #5]
.La2346:
	sub	r6, #1
	cmp	r6, #0
	bge	.La233e
	mov	r6, r1
	add	r0, r6
	cmp	r6, r0
	bge	.La23ac
	lsl	r2, r6, #2
	mov	r3, r2
	add	r3, #0x48
	mov	r1, r8
	ldr	r5, [r1, r3]
	cmp	r5, #0
	beq	.La23ac
	mov	r3, #0x86
	lsl	r3, #2
	add	r3, r8
	ldrb	r3, [r3]
	sub	r3, #1
	cmp	r6, r3
	bgt	.La23ac
	add	r3, r2, r1
	mov	r2, r3
	ldr	r7, [sp, #0x20]
	mov	r10, r0
	add	r2, #0x48
.La237a:
	mov	r3, r9
	strh	r3, [r5, #6]
	strh	r7, [r5, #8]
	mov	r0, r5
	str	r2, [sp]
	bl	Func_80a17c4
	add	r6, #1
	mov	r3, #1
	strb	r3, [r5, #5]
	add	r7, #0x10
	ldr	r2, [sp]
	cmp	r6, r10
	bge	.La23ac
	add	r2, #4
	ldr	r5, [r2]
	cmp	r5, #0
	beq	.La23ac
	mov	r3, #0x86
	lsl	r3, #2
	add	r3, r8
	ldrb	r3, [r3]
	sub	r3, #1
	cmp	r6, r3
	ble	.La237a
.La23ac:
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a2324

@ DrawCoinTotal
@ r0 = window. Draws the party's money -- the word at ewram_240+0x10 -- as a
@ seven-digit number at x 8, then label 0xB0B at x 0x40.
.thumb_func_start Func_80a23c0  @ 0x080a23c0
	push	{r5, lr}
	ldr	r3, =gState
	sub	sp, #4
	mov	r5, r0
	ldr	r0, [r3, #0x10]
	mov	r3, #0
	str	r3, [sp]
	mov	r2, r5
	mov	r1, #7
	mov	r3, #8
	bl	_Func_801e9d4
	ldr	r0, =0xb0b
	mov	r1, r5
	mov	r2, #0x40
	mov	r3, #0
	bl	_Func_801e7c0
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_80a23c0

