	.include "macros.inc"
	.include "gba.inc"

@ DrawCharacterPanel
@ r0 = window slot, r1 = character id, r2 = inventory slot, r3 = mode.
@ The panel that appears beside every list in this module. It resolves the
@ character with _Func_77394, reads the equipment halfword at +0xD8 + slot*2,
@ and turns its low 9 bits into an ability record with _Func_78414. Bit 8 of
@ the mode (0x100) suppresses the window and the menu entries; the low byte
@ selects the layout:
@
@     0  name (string 0x741 + [record+0x129]) + Func_a153c + the value at
@        +0x124 under label 0xB0E
@     2, 3, 4  THE EQUIP PREVIEW -- _Func_7842c decides whether this character
@        can use the item at all; when it cannot, string 0xB21 and nothing
@        else. When it can, the record is copied to a 0x14C scratch, the item
@        applied through _Func_78708, recomputed with _Func_77428, and
@        Func_a15f0 draws the OLD and NEW stats side by side with arrows.
@     5  scans the 32 inventory halfwords at +0x58 + i*4 (mask 0x3FFF) for the
@        item, then string 0x333 + itemId followed by 0xB23 when it is already
@        held and 0xB22 when it is not
@     7  name and Func_a153c only
@     8  the four stats -- +0x3C, +0x3E, +0x40 as halfwords and +0x42 as a byte,
@        under labels 0xB1C, 0xB1D, 0xB20, 0xB1F
@
@ Traced structurally past the dispatch; the individual arms are read.
.thumb_func_start Func_80a112c  @ 0x080a112c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x24
	str	r2, [sp, #0x18]
	mov	r2, #0
	str	r2, [sp, #0x14]
	mov	r11, r3
	ldr	r3, =iwram_3001f2c
	mov	r9, r1
	ldr	r3, [r3]
	mov	r0, r9
	mov	r10, r3
	bl	_GetUnit
	ldr	r3, [sp, #0x18]
	lsl	r3, #1
	str	r3, [sp, #8]
	add	r3, #0xd8
	ldrh	r3, [r0, r3]
	mov	r8, r0
	ldr	r0, =0x1ff
	and	r0, r3
	str	r3, [sp, #0xc]
	bl	_GetItemInfo
	mov	r5, #0x80
	lsl	r5, #1
	mov	r2, r11
	and	r5, r2
	str	r0, [sp, #0x10]
	cmp	r5, #0
	bne	.La118e
	mov	r3, #0xc
	str	r3, [sp]
	mov	r0, r10
	add	r3, #0xf6
	str	r3, [sp, #4]
	add	r0, #0x24
	mov	r1, #0
	mov	r2, #5
	mov	r3, #0xd
	bl	Func_80a10d0
	str	r0, [sp, #0x14]
.La118e:
	mov	r3, r10
	ldr	r7, [r3, #0x24]
	cmp	r5, #0
	bne	.La1248
	ldr	r5, [sp, #0x14]
	cmp	r5, #0
	bne	.La11b4
	mov	r0, #1
	bl	WaitFrames
	mov	r2, r10
	mov	r3, #0x20
	ldr	r0, [r2, #0x24]
	mov	r1, #0
	str	r3, [sp]
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_80164d4
.La11b4:
	mov	r3, #0
	mov	r0, r8
	mov	r1, r7
	mov	r2, #0x20
	bl	_Func_801e8b0
	add	r6, sp, #0x1c
	mov	r0, r6
	mov	r1, #1
	mov	r2, r9
	bl	Func_80a8b10
	ldrb	r3, [r6, #1]
	mov	r5, #0
	cmp	r3, #0
	beq	.La11e2
	ldr	r0, =0xbd6
	mov	r1, r7
	mov	r2, #0x20
	mov	r3, #8
	bl	_Func_801e7c0
	mov	r5, #1
.La11e2:
	ldrb	r3, [r6, #2]
	cmp	r3, #0
	beq	.La11f8
	lsl	r3, r5, #3
	ldr	r0, =0xbd7
	add	r3, #8
	mov	r1, r7
	mov	r2, #0x20
	bl	_Func_801e7c0
	add	r5, #1
.La11f8:
	ldrb	r3, [r6, #3]
	cmp	r3, #0
	beq	.La120e
	lsl	r3, r5, #3
	ldr	r0, =0xbd8
	add	r3, #8
	mov	r1, r7
	mov	r2, #0x20
	bl	_Func_801e7c0
	add	r5, #1
.La120e:
	ldrb	r3, [r6, #4]
	cmp	r3, #0
	beq	.La1224
	lsl	r3, r5, #3
	ldr	r0, =0xbd9
	add	r3, #8
	mov	r1, r7
	mov	r2, #0x20
	bl	_Func_801e7c0
	add	r5, #1
.La1224:
	cmp	r5, #1
	bgt	.La1248
	mov	r3, r8
	ldrb	r6, [r3, #0xf]
	ldr	r0, =.Laf20c
	mov	r1, r7
	mov	r2, #0x28
	mov	r3, #0x10
	bl	_Func_801e8b0
	mov	r3, #0x10
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #4
	mov	r2, r7
	mov	r3, #0x38
	bl	_Func_801ea08
.La1248:
	ldr	r5, [sp, #0x14]
	cmp	r5, #0
	bne	.La1266
	mov	r0, #1
	bl	WaitFrames
	mov	r2, r10
	mov	r3, #0x50
	ldr	r0, [r2, #0x24]
	mov	r1, #0
	str	r3, [sp]
	mov	r2, #0x20
	mov	r3, #0x58
	bl	_Func_80164d4
.La1266:
	mov	r0, r7
	bl	_Func_80164ac
	mov	r3, #0x80
	lsl	r3, #1
	mov	r5, r11
	and	r3, r5
	cmp	r3, #0
	bne	.La1290
	str	r3, [sp]
	str	r3, [sp, #4]
	mov	r0, r9
	mov	r3, r7
	mov	r1, #0
	mov	r2, #0
	bl	_Func_801ec6c
	mov	r3, #0xbe
	lsl	r3, #1
	add	r3, r10
	str	r0, [r3]
.La1290:
	mov	r3, #0xff
	mov	r2, r11
	and	r3, r2
	cmp	r3, #8
	bls	.La129c
	b	.La1490
.La129c:
	ldr	r2, =.La12a4
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.La12a4:
	.word	.La12c8
	.word	.La1490
	.word	.La132c
	.word	.La132c
	.word	.La13ac
	.word	.La1490
	.word	.La130c
	.word	.La1490
	.word	.La140a
.La12c8:
	ldr	r3, =0x129
	add	r3, r8
	ldrb	r2, [r3]
	ldr	r3, =0x741
	add	r6, r2, r3
	mov	r0, r6
	mov	r2, #0
	mov	r3, #0x20
	mov	r1, r7
	bl	_Func_801e7c0
	mov	r0, r8
	mov	r1, r7
	bl	Func_80a153c
	mov	r3, #0x92
	lsl	r3, #1
	add	r3, r8
	ldr	r6, [r3]
	ldr	r0, =0xb0e
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x40
	bl	_Func_801e7c0
	mov	r3, #0x48
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #8
	mov	r2, r7
	mov	r3, #0x18
	bl	_Func_801ea08
	b	.La1490
.La130c:
	ldr	r3, =0x129
	add	r3, r8
	ldrb	r2, [r3]
	ldr	r3, =0x741
	add	r6, r2, r3
	mov	r0, r6
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x20
	bl	_Func_801e7c0
	mov	r0, r8
	mov	r1, r7
	bl	Func_80a153c
	b	.La1490
.La132c:
	mov	r0, r9
	ldr	r1, [sp, #0xc]
	bl	_CanEquipItem
	cmp	r0, #0
	bne	.La1346
	ldr	r0, =0xb21
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e7c0
	b	.La1490
.La1346:
	mov	r5, #0xa6
	lsl	r5, #1
	mov	r0, r5
	bl	Func_8004938
	ldr	r3, =Func_8001af8
	mov	r1, r8
	mov	r2, r5
	mov	r6, r0
	bl	_call_via_r3
	mov	r3, #0x97
	lsl	r3, #2
	add	r3, r10
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.La137e
	ldr	r1, [sp, #8]
	mov	r3, r8
	add	r1, #0xd8
	ldrh	r2, [r3, r1]
	ldr	r3, =0xfdff
	mov	r5, r8
	and	r3, r2
	strh	r3, [r5, r1]
	b	.La1386
.La137e:
	mov	r0, r9
	ldr	r1, [sp, #0x18]
	bl	_EquipItem
.La1386:
	mov	r0, r9
	bl	_CalcStats
	mov	r0, r8
	mov	r1, r6
	mov	r2, r7
	bl	Func_80a15f0
	mov	r2, #0xa6
	ldr	r3, =Func_8001af8
	mov	r0, r8
	mov	r1, r6
	lsl	r2, #1
	bl	_call_via_r3
	mov	r0, r6
	bl	free
	b	.La1490
.La13ac:
	ldr	r2, [sp, #0x10]
	mov	r3, #0x58
	mov	r5, r8
	ldrh	r4, [r2, #0x28]
	ldrh	r2, [r5, r3]
	ldr	r3, =0x3fff
	and	r3, r2
	mov	r0, #0
	mov	r1, #0
	b	.La13d2
.La13c0:
	add	r1, #1
	cmp	r1, #0x1f
	bgt	.La13d8
	lsl	r3, r1, #2
	add	r3, #0x58
	mov	r5, r8
	ldrh	r2, [r5, r3]
	ldr	r3, =0x3fff
	and	r3, r2
.La13d2:
	cmp	r3, r4
	bne	.La13c0
	mov	r0, #1
.La13d8:
	cmp	r0, #0
	beq	.La13ee
	ldr	r0, =0x333
	mov	r1, r7
	add	r0, r4, r0
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e7c0
	ldr	r0, =0xb23
	b	.La13fe
.La13ee:
	ldr	r0, =0x333
	mov	r1, r7
	add	r0, r4, r0
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e7c0
	ldr	r0, =0xb22
.La13fe:
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x38
	bl	_Func_801e7c0
	b	.La1490
.La140a:
	ldr	r5, =0xb1c
	mov	r1, r7
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0x28
	bl	_Func_801e7c0
	mov	r2, r8
	ldrh	r6, [r2, #0x3c]
	mov	r3, #0x28
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #3
	mov	r2, r7
	mov	r3, #0x40
	bl	_Func_801ea08
	add	r0, r5, #1
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e7c0
	mov	r3, r8
	ldrh	r6, [r3, #0x3e]
	mov	r3, #0x30
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #3
	mov	r2, r7
	mov	r3, #0x40
	bl	_Func_801ea08
	add	r0, r5, #4
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x38
	bl	_Func_801e7c0
	mov	r3, r8
	add	r3, #0x40
	ldrh	r6, [r3]
	mov	r3, #0x38
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #3
	mov	r2, r7
	mov	r3, #0x40
	add	r5, #3
	bl	_Func_801ea08
	mov	r0, r5
	mov	r1, r7
	mov	r2, #0
	mov	r3, #0x40
	bl	_Func_801e7c0
	mov	r3, r8
	add	r3, #0x42
	ldrb	r6, [r3]
	mov	r1, #3
	mov	r3, #0x40
	mov	r0, r6
	mov	r2, r7
	str	r3, [sp]
	bl	_Func_801ea08
.La1490:
	add	sp, #0x24
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a112c

@ DrawNumberRightAligned
@ r0 = window, r1 = value, r2 = right edge x, r3 = y.
@ Counts the decimal digits by dividing by 10 up to fifteen times, moves x left
@ by (digits + 1) * 8 -- so the glyphs are 8 pixels wide and one is left for the
@ sign or padding -- and emits through _Func_1e9d4. This is why every number in
@ the menus lines up on its right edge.
.thumb_func_start Func_80a14f0  @ 0x080a14f0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	sub	sp, #4
	mov	r7, r0
	mov	r10, r1
	mov	r6, r2
	mov	r8, r3
	mov	r5, #1
	b	.La1508
.La1506:
	add	r5, #1
.La1508:
	cmp	r5, #0xf
	bgt	.La1516
	mov	r1, #0xa
	bl	__divsi3
	cmp	r0, #9
	bgt	.La1506
.La1516:
	add	r5, #1
	lsl	r3, r5, #3
	sub	r6, r3
	mov	r3, r8
	str	r3, [sp]
	mov	r0, r7
	mov	r1, r5
	mov	r2, r10
	mov	r3, r6
	bl	_Func_801e9d4
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a14f0

@ DrawHpPpReadout
@ r0 = character record, r1 = window. Draws the two `current/max` pairs the
@ menus show under a name, at rows 0x28 and 0x30 with the separator glyph
@ .Laf214 at x 0x30 and the maxima right-aligned at x 0x58.
@
@     +0x34 max HP    +0x38 current HP
@     +0x36 max PP    +0x3A current PP
@
@ The ink is switched before the current HP is drawn: colour 4 when it has
@ fallen below a QUARTER of the maximum, colour 2 when it is zero, then back to
@ 0x0F. That is the low-HP warning colour, and the quarter threshold is the
@ `lsl #16 / asr #18` pair, not a comparison against a stored value.
.thumb_func_start Func_80a153c  @ 0x080a153c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r2, #0
	ldr	r0, =.Laf210
	mov	r3, #0x28
	mov	r6, r1
	bl	_Func_801e8b0
	ldr	r3, =.Laf214
	mov	r8, r3
	mov	r0, r8
	mov	r3, #0x28
	mov	r1, r6
	mov	r2, #0x30
	bl	_UIDrawText
	mov	r3, #0x34
	ldrsh	r5, [r7, r3]
	mov	r1, r6
	mov	r3, #0x28
	mov	r0, r5
	mov	r2, #0x58
	bl	Func_80a14f0
	mov	r3, #0x38
	ldrsh	r5, [r7, r3]
	ldrh	r3, [r7, #0x34]
	lsl	r3, #16
	asr	r3, #18
	cmp	r5, r3
	bge	.La1584
	mov	r0, #4
	bl	_SetTextColor
.La1584:
	cmp	r5, #0
	bne	.La158e
	mov	r0, #2
	bl	_SetTextColor
.La158e:
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0x30
	mov	r3, #0x28
	bl	Func_80a14f0
	mov	r0, #0xf
	bl	_SetTextColor
	mov	r1, r6
	ldr	r0, =.Laf218
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e8b0
	mov	r0, r8
	mov	r1, r6
	mov	r3, #0x30
	mov	r2, #0x30
	bl	_UIDrawText
	mov	r3, #0x3a
	ldrsh	r5, [r7, r3]
	mov	r1, r6
	mov	r0, r5
	mov	r3, #0x30
	mov	r2, #0x30
	bl	Func_80a14f0
	mov	r3, #0x36
	ldrsh	r5, [r7, r3]
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0x58
	mov	r3, #0x30
	bl	Func_80a14f0
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a153c

@ DrawStatComparison
@ r0 = record BEFORE, r1 = record AFTER, r2 = window.
@ The other half of Func_a112c's equip preview. For each of the three halfword
@ stats -- +0x3C under label 0xB1C, +0x3E under 0xB1D, +0x40 under 0xB20 -- it
@ draws the new value at x 0x10 and, when the old value differs, the old one at
@ x 0x40 plus an arrow sprite from Func_ae99c at x 0x2C. Arrow direction 0 is up
@ (the stat rises) and 1 is down. Equal stats draw once with no arrow.
.thumb_func_start Func_80a15f0  @ 0x080a15f0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r6, r2
	mov	r5, r1
	mov	r8, r0
	mov	r1, r6
	ldr	r0, =0xb1c
	mov	r2, #0
	mov	r3, #0x20
	sub	sp, #4
	bl	_Func_801e7c0
	mov	r7, #0x28
	ldrh	r0, [r5, #0x3c]
	mov	r2, r6
	mov	r3, #0x10
	mov	r1, #3
	str	r7, [sp]
	bl	_Func_801e9d4
	mov	r3, r8
	ldrh	r2, [r3, #0x3c]
	ldrh	r3, [r5, #0x3c]
	cmp	r2, r3
	beq	.La1658
	mov	r0, r2
	mov	r3, #0x40
	mov	r2, r6
	mov	r1, #3
	str	r7, [sp]
	bl	_Func_801e9d4
	mov	r3, r8
	ldrh	r2, [r3, #0x3c]
	ldrh	r3, [r5, #0x3c]
	cmp	r2, r3
	bls	.La164c
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x24
	mov	r3, #0
	bl	Func_80ae99c
	b	.La1658
.La164c:
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x24
	mov	r3, #1
	bl	Func_80ae99c
.La1658:
	ldr	r0, =0xb1d
	mov	r1, r6
	mov	r2, #0
	mov	r3, #0x30
	bl	_Func_801e7c0
	mov	r7, #0x38
	ldrh	r0, [r5, #0x3e]
	mov	r2, r6
	mov	r3, #0x10
	mov	r1, #3
	str	r7, [sp]
	bl	_Func_801e9d4
	mov	r3, r8
	ldrh	r2, [r3, #0x3e]
	ldrh	r3, [r5, #0x3e]
	cmp	r2, r3
	beq	.La16b0
	mov	r0, r2
	mov	r3, #0x40
	mov	r2, r6
	mov	r1, #3
	str	r7, [sp]
	bl	_Func_801e9d4
	mov	r3, r8
	ldrh	r2, [r3, #0x3e]
	ldrh	r3, [r5, #0x3e]
	cmp	r2, r3
	bls	.La16a4
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x34
	mov	r3, #0
	bl	Func_80ae99c
	b	.La16b0
.La16a4:
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x34
	mov	r3, #1
	bl	Func_80ae99c
.La16b0:
	ldr	r0, =0xb20
	mov	r1, r6
	mov	r2, #0
	mov	r3, #0x40
	bl	_Func_801e7c0
	mov	r7, r5
	mov	r3, #0x48
	add	r7, #0x40
	mov	r5, r8
	ldrh	r0, [r7]
	mov	r2, r6
	str	r3, [sp]
	mov	r10, r3
	mov	r1, #3
	mov	r3, #0x10
	add	r5, #0x40
	bl	_Func_801e9d4
	ldrh	r2, [r5]
	ldrh	r3, [r7]
	cmp	r2, r3
	beq	.La1710
	mov	r3, r10
	mov	r0, r2
	str	r3, [sp]
	mov	r2, r6
	mov	r3, #0x40
	mov	r1, #3
	bl	_Func_801e9d4
	ldrh	r2, [r5]
	ldrh	r3, [r7]
	cmp	r2, r3
	bls	.La1704
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x44
	mov	r3, #0
	bl	Func_80ae99c
	b	.La1710
.La1704:
	mov	r0, r6
	mov	r1, #0x2c
	mov	r2, #0x44
	mov	r3, #1
	bl	Func_80ae99c
.La1710:
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a15f0

	.section .rodata

.Laf20c:
	.incrom 0xaf20c, 0xaf210
.Laf210:
	.incrom 0xaf210, 0xaf214
.Laf214:
	.incrom 0xaf214, 0xaf218
.Laf218:
	.incrom 0xaf218, 0xaf21c
