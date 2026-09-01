	.include "macros.inc"

@ DrawCharacterHeader
@ r0.. = placement. Draws the name and class line -- STRING 0x741 + the class
@ id at record+0x129 -- with the level and the coin label 0xB0E. 209 lines;
@ traced structurally.
.thumb_func_start Func_80a8914  @ 0x080a8914
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	mov	r6, r0
	mov	r0, r1
	mov	r8, r2
	ldr	r5, [r3]
	sub	sp, #4
	bl	_GetUnit
	mov	r2, #0xbe
	lsl	r2, #1
	add	r5, r2
	ldr	r2, [r5]
	mov	r3, #1
	strb	r3, [r2, #5]
	mov	r2, r8
	add	r3, #0xff
	and	r2, r3
	mov	r7, r0
	mov	r8, r2
	cmp	r2, #0
	bne	.La895c
	mov	r3, #0x28
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #0
	mov	r2, #0
	mov	r3, #0x80
	bl	_Func_80164d4
.La895c:
	mov	r0, r7
	mov	r1, r6
	mov	r2, #0x28
	mov	r3, #0
	bl	_Func_801e8b0
	ldr	r2, =0x129
	add	r3, r7, r2
	ldrb	r0, [r3]
	ldr	r3, =0x741
	mov	r1, r6
	add	r0, r3
	mov	r2, #0
	mov	r3, #0x20
	bl	_Func_801e7c0
	mov	r1, r6
	mov	r2, #0x68
	mov	r3, #0
	ldr	r0, =.Laf22c
	bl	_Func_801e8b0
	mov	r0, #0xf
	bl	_SetTextColor
	mov	r3, #0
	ldrb	r0, [r7, #0xf]
	mov	r1, #2
	str	r3, [sp]
	mov	r2, r6
	mov	r3, #0x80
	bl	_Func_801ea08
	mov	r2, #0x28
	ldr	r0, =.Laf234
	mov	r1, r6
	mov	r3, #0x10
	bl	_Func_801e8b0
	mov	r3, #0x10
	mov	r2, #0x38
	ldrsh	r0, [r7, r2]
	mov	r9, r3
	str	r3, [sp]
	mov	r2, r6
	mov	r1, #4
	mov	r3, #0x48
	bl	_Func_801ea08
	mov	r3, r9
	mov	r2, #0x34
	ldrsh	r0, [r7, r2]
	mov	r1, #4
	str	r3, [sp]
	mov	r2, r6
	mov	r3, #0x70
	bl	_Func_801ea08
	ldr	r5, =.Laf230
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0x68
	mov	r3, #0x10
	bl	_UIDrawText
	mov	r2, #0x28
	ldr	r0, =.Laf238
	mov	r1, r6
	mov	r3, #0x18
	bl	_Func_801e8b0
	mov	r3, #0x18
	mov	r2, #0x3a
	ldrsh	r0, [r7, r2]
	mov	r10, r3
	str	r3, [sp]
	mov	r2, r6
	mov	r1, #4
	mov	r3, #0x48
	bl	_Func_801ea08
	mov	r3, r10
	mov	r2, #0x36
	ldrsh	r0, [r7, r2]
	mov	r1, #4
	str	r3, [sp]
	mov	r2, r6
	mov	r3, #0x70
	bl	_Func_801ea08
	mov	r0, r5
	mov	r1, r6
	mov	r2, #0x68
	mov	r3, #0x18
	bl	_UIDrawText
	ldr	r5, =0xb0e
	mov	r1, r6
	mov	r0, r5
	mov	r2, #0x28
	mov	r3, #8
	bl	_Func_801e7c0
	mov	r2, #0x92
	lsl	r2, #1
	add	r3, r7, r2
	ldr	r0, [r3]
	mov	r3, #8
	str	r3, [sp]
	mov	r2, r6
	mov	r11, r3
	mov	r1, #7
	mov	r3, #0x58
	bl	_Func_801ea08
	mov	r2, r8
	cmp	r2, #0
	bne	.La8a5e
	mov	r0, #1
	bl	WaitFrames
	mov	r3, #0x28
	str	r3, [sp]
	mov	r0, r6
	mov	r1, #0x90
	mov	r2, #0
	mov	r3, #0xe0
	bl	_Func_80164d4
.La8a5e:
	mov	r0, r5
	mov	r1, r6
	sub	r0, #0x17
	mov	r2, #0x98
	mov	r3, #0
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r6
	sub	r0, #0x16
	mov	r2, #0x98
	mov	r3, #8
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r6
	sub	r0, #0x15
	mov	r2, #0x98
	mov	r3, #0x10
	bl	_Func_801e7c0
	mov	r0, r5
	mov	r1, r6
	sub	r0, #0x14
	mov	r2, #0x98
	mov	r3, #0x18
	bl	_Func_801e7c0
	mov	r3, #0
	ldrh	r0, [r7, #0x3c]
	mov	r2, r6
	str	r3, [sp]
	mov	r1, #3
	mov	r3, #0xc8
	bl	_Func_801ea08
	mov	r2, r11
	ldrh	r0, [r7, #0x3e]
	mov	r1, #3
	str	r2, [sp]
	mov	r3, #0xc8
	mov	r2, r6
	bl	_Func_801ea08
	mov	r3, r7
	add	r3, #0x40
	ldrh	r0, [r3]
	mov	r3, r9
	str	r3, [sp]
	mov	r2, r6
	mov	r1, #3
	mov	r3, #0xc8
	bl	_Func_801ea08
	mov	r3, r7
	mov	r2, r10
	add	r3, #0x42
	ldrb	r0, [r3]
	mov	r1, #3
	str	r2, [sp]
	mov	r3, #0xc8
	mov	r2, r6
	bl	_Func_801ea08
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a8914

@ CollectStatusFlags
@ r0 = five-byte destination, r1 = 1 to include the downed flag, r2 = character
@ id. Zeroes the five bytes and sets:
@
@     [0]  the character is DOWN -- current HP (record+0x38) is zero -- and
@          only when r1 is 1
@     [1]  record+0x131 is exactly 1
@     [2]  record+0x131 is anything else non-zero
@     [3]  record+0x130 is non-zero
@     [4]  record+0x140 is non-zero
@
@ Returns how many of [1]..[4] were set. Those four map one for one onto the
@ strings 0xBD6, 0xBD7, 0xBD8 and 0xBD9 that Func_a112c prints, so this is the
@ status-icon set.
.thumb_func_start Func_80a8b10  @ 0x080a8b10
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r0, r2
	mov	r6, r1
	bl	_GetUnit
	mov	r1, #0
	mov	r2, r0
	add	r3, r5, #4
	mov	r12, r5
.La8b24:
	strb	r1, [r3]
	sub	r3, #1
	cmp	r3, r12
	bge	.La8b24
	mov	r1, #0x38
	ldrsh	r3, [r2, r1]
	mov	r0, #0
	cmp	r3, #0
	bne	.La8b3e
	cmp	r6, #1
	bne	.La8b3e
	strb	r6, [r5]
	mov	r0, #1
.La8b3e:
	ldr	r1, =0x131
	add	r3, r2, r1
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.La8b5a
	cmp	r3, #1
	bne	.La8b54
	strb	r3, [r5, #1]
	b	.La8b58
.La8b54:
	mov	r3, #1
	strb	r3, [r5, #2]
.La8b58:
	add	r0, #1
.La8b5a:
	mov	r1, #0x98
	lsl	r1, #1
	add	r3, r2, r1
	ldrb	r3, [r3]
	lsl	r3, #24
	asr	r3, #24
	cmp	r3, #0
	beq	.La8b70
	mov	r3, #1
	strb	r3, [r5, #3]
	add	r0, #1
.La8b70:
	mov	r1, #0xa0
	lsl	r1, #1
	add	r3, r2, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.La8b82
	mov	r3, #1
	strb	r3, [r5, #4]
	add	r0, #1
.La8b82:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80a8b10
