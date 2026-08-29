	.include "macros.inc"
	.include "gba.inc"

@ CountTableA
@ Takes no arguments. Returns (.L2a2e0 - .L29ee4) / 4 = 255, the number of
@ entries in the first graphic-pointer table. Computed from the label addresses
@ rather than stored, so it stays correct if the table is regenerated.
.thumb_func_start Func_19ebc
	ldr	r0, =.L2a2e0
	ldr	r3, =.L29ee4
	sub	r0, r3
	asr	r0, #2
	bx	lr
.func_end Func_19ebc

@ CountTableB
@ Takes no arguments. Returns (.L2e108 - .L2de88) / 4 = 160, the entry count of
@ the second graphic-pointer table, computed the same way as Func_19ebc.
.thumb_func_start Func_19ed0
	ldr	r0, =.L2e108
	ldr	r3, =.L2de88
	sub	r0, r3
	asr	r0, #2
	bx	lr
.func_end Func_19ed0

@ LoadPortrait
@ r0 = id, r1 = destination. Allocates the 0x608-byte graphic block under tag
@ 0x11 -- which is what iwram_1e94 points at, since iwram_1e50 + 0x11*4 =
@ iwram_1e94 -- then decompresses into it with Func_1a5a4 and reserves OBJ tiles
@ with Func_3fa4 / Func_4080. Func_2dd8 releases the block on the failure path.
.thumb_func_start Func_19ee4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r5, r1
	mov	r1, #0xc1
	sub	sp, #4
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	str	r3, [sp]
	mov	r11, r2
	bl	Func_48b0
	ldr	r3, =.L29a10
	lsl	r5, #2
	ldr	r2, =0x604
	ldr	r3, [r3, r5]
	mov	r7, r0
	add	r2, r7
	str	r3, [r2]
	mov	r3, #0xc0
	lsl	r3, #3
	add	r3, r7
	mov	r10, r3
	ldr	r3, =0x602
	add	r3, r7
	mov	r5, #2
	mov	r8, r3
	mov	r9, r2
	mov	r2, r10
	strh	r5, [r2]
	mov	r2, r8
	strh	r5, [r2]
	mov	r1, #0
	bl	Func_1a5a4
	ldr	r3, =.L29e00
	lsl	r6, #2
	ldr	r3, [r3, r6]
	mov	r2, r9
	str	r3, [r2]
	mov	r3, r10
	mov	r2, r8
	strh	r5, [r3]
	mov	r0, r7
	strh	r5, [r2]
	mov	r1, #1
	bl	Func_1a5a4
	ldr	r3, [sp, #0x24]
	cmp	r3, #0
	bne	.L19f5c
	bl	Func_4080
	mov	r2, r11
	str	r0, [r2]
.L19f5c:
	mov	r3, r11
	ldr	r0, [r3]
	mov	r3, #0x80
	lsl	r3, #3
	add	r2, r7, r3
	mov	r1, #0x80
	bl	Func_3fa4
	ldr	r2, [sp]
	str	r0, [r2]
	mov	r0, #0x11
	bl	Func_2dd8
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_19ee4

@ LoadPortraitForCharacter
@ r0 = character, r1..r3 and arg5 = placement. Resolves the character through
@ _Func_78b9c (rom_77000's party lookup), takes its portrait id from +4 of the
@ record, and forwards to Func_19fcc.
.thumb_func_start Func_19f98
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	sub	sp, #4
	ldr	r5, [sp, #0x18]
	mov	r6, r1
	mov	r8, r2
	mov	r10, r3
	bl	_Func_78b9c
	mov	r1, r6
	ldrb	r0, [r0, #4]
	mov	r2, r8
	mov	r3, r10
	str	r5, [sp]
	bl	Func_19fcc
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_19f98

@ LoadPortraitIndexed
@ r0 = index, r1.. = placement. As Func_19ee4 but bounds-checks the index
@ against Func_19ebc's count of 255 before using it.
.thumb_func_start Func_19fcc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r1
	mov	r1, #0
	mov	r10, r1
	mov	r1, #0xc1
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r8, r2
	mov	r9, r3
	bl	Func_48b0
	mov	r5, r0
	bl	Func_19ebc
	cmp	r6, r0
	bcc	.L19ff8
	mov	r6, #0
.L19ff8:
	cmp	r7, #0
	beq	.L1a022
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29a10
	mov	r1, #0xc0
	ldr	r3, [r3, #8]
	lsl	r1, #3
	str	r3, [r2]
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, #0
	bl	Func_1a5a4
	mov	r2, #1
	mov	r10, r2
.L1a022:
	ldr	r3, =0x604
	ldr	r2, =.L29ee4
	add	r1, r5, r3
	lsl	r3, r6, #2
	ldr	r3, [r2, r3]
	str	r3, [r1]
	mov	r1, #0xc0
	lsl	r1, #3
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, r10
	bl	Func_1a5a4
	ldr	r2, [sp, #0x1c]
	cmp	r2, #0
	bne	.L1a054
	bl	Func_4080
	mov	r3, r8
	str	r0, [r3]
.L1a054:
	mov	r3, #0x80
	mov	r1, r8
	lsl	r3, #3
	ldr	r0, [r1]
	add	r2, r5, r3
	mov	r1, #0x80
	bl	Func_3fa4
	mov	r1, r9
	str	r0, [r1]
	mov	r0, #0x11
	bl	Func_2dd8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_19fcc

@ DrawPortraitTiles
@ r0 = id masked to 0x1FF, r1.. = placement. Emits a portrait into OBJ tiles,
@ working from the block at iwram_1e94 and the character record from
@ _Func_78414. Func_af0 and Func_b1c supply the row and column arithmetic.
@ 256 lines; traced structurally.
.thumb_func_start Func_1a088
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	str	r0, [sp, #8]
	ldr	r2, [sp, #8]
	ldr	r0, =0x1ff
	mov	r11, r1
	and	r0, r2
	mov	r1, #0
	str	r1, [sp, #4]
	mov	r10, r1
	bl	_Func_78414
	ldr	r3, =iwram_1e94
	str	r0, [sp]
	ldr	r5, [r3]
	cmp	r5, #0
	bne	.L1a0bc
	mov	r0, #1
	neg	r0, r0
	b	.L1a272
.L1a0bc:
	mov	r3, #1
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L1a0ec
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29a10
	mov	r1, #0xc0
	ldr	r3, [r3, #8]
	lsl	r1, #3
	str	r3, [r2]
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, #0
	bl	Func_1a5a4
	mov	r2, #1
	str	r2, [sp, #4]
.L1a0ec:
	ldr	r3, =0x604
	ldr	r1, [sp]
	add	r3, r5
	mov	r9, r3
	ldrh	r3, [r1, #6]
	ldr	r2, =.L29ee4
	lsl	r3, #2
	ldr	r3, [r2, r3]
	mov	r2, r9
	str	r3, [r2]
	mov	r3, #0xc0
	lsl	r3, #3
	add	r3, r5
	ldr	r2, =0x602
	mov	r8, r3
	mov	r6, #2
	mov	r1, r8
	add	r7, r5, r2
	strh	r6, [r1]
	strh	r6, [r7]
	ldr	r1, [sp, #4]
	mov	r0, r5
	bl	Func_1a5a4
	mov	r3, #8
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L1a148
	mov	r3, #0x80
	ldr	r2, [sp, #8]
	lsl	r3, #3
	and	r3, r2
	cmp	r3, #0
	beq	.L1a148
	ldr	r3, =.L29acc
	ldr	r3, [r3, #4]
	mov	r1, r9
	mov	r2, r8
	str	r3, [r1]
	mov	r0, r5
	strh	r6, [r2]
	mov	r1, #1
	strh	r6, [r7]
	bl	Func_1a5a4
.L1a148:
	mov	r3, #0x10
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L1a180
	mov	r3, #0x80
	ldr	r2, [sp, #8]
	lsl	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L1a180
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29acc
	mov	r1, #0xc0
	ldr	r3, [r3]
	lsl	r1, #3
	str	r3, [r2]
	add	r3, r5, r1
	mov	r2, #2
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, #1
	bl	Func_1a5a4
.L1a180:
	mov	r3, #0x20
	mov	r2, r11
	and	r3, r2
	cmp	r3, #0
	beq	.L1a1cc
	mov	r3, #0x80
	ldr	r1, [sp, #8]
	lsl	r3, #2
	and	r3, r1
	cmp	r3, #0
	beq	.L1a1cc
	ldr	r2, [sp]
	ldrb	r0, [r2, #3]
	mov	r3, #1
	and	r3, r0
	cmp	r3, #0
	beq	.L1a1cc
	mov	r1, #2
	mov	r3, r1
	and	r3, r0
	cmp	r3, #0
	beq	.L1a1cc
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29acc
	ldr	r3, [r3, #8]
	str	r3, [r2]
	mov	r2, #0xc0
	lsl	r2, #3
	add	r3, r5, r2
	add	r2, #2
	strh	r1, [r3]
	add	r3, r5, r2
	strh	r1, [r3]
	mov	r0, r5
	mov	r1, #1
	bl	Func_1a5a4
.L1a1cc:
	mov	r3, #2
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L1a1f0
	mov	r3, #0xf8
	ldr	r2, [sp, #8]
	lsl	r3, #8
	and	r3, r2
	lsr	r3, #11
	mov	r10, r3
	mov	r3, #1
	add	r10, r3
	mov	r1, r10
	cmp	r1, #1
	bgt	.L1a1f0
	mov	r2, #0
	mov	r10, r2
.L1a1f0:
	mov	r3, #4
	mov	r1, r11
	and	r3, r1
	cmp	r3, #0
	beq	.L1a20a
	mov	r3, #0xf8
	ldr	r2, [sp, #8]
	lsl	r3, #8
	and	r3, r2
	lsr	r3, #11
	mov	r10, r3
	mov	r3, #1
	add	r10, r3
.L1a20a:
	mov	r1, r10
	cmp	r1, #0
	beq	.L1a26e
	cmp	r1, #0x1e
	bgt	.L1a26e
	mov	r1, #0xa
	mov	r0, r10
	bl	Func_b1c_from_thumb
	ldr	r3, =.L29b68
	lsl	r0, #2
	ldr	r2, =0x604
	mov	r11, r3
	mov	r1, #0xc0
	ldr	r3, [r3, r0]
	add	r2, r5
	lsl	r1, #3
	str	r3, [r2]
	add	r1, r5
	ldr	r3, =0x602
	mov	r8, r1
	mov	r6, #2
	add	r7, r5, r3
	mov	r9, r2
	mov	r2, r8
	strh	r6, [r2]
	mov	r0, r5
	mov	r1, #1
	strh	r6, [r7]
	bl	Func_1a5a4
	mov	r0, r10
	mov	r1, #0xa
	bl	Func_af0_from_thumb
	cmp	r0, #0
	beq	.L1a26e
	lsl	r3, r0, #2
	mov	r1, r11
	add	r3, #0x24
	ldr	r3, [r1, r3]
	mov	r2, r9
	str	r3, [r2]
	mov	r3, r8
	strh	r6, [r3]
	mov	r0, r5
	strh	r6, [r7]
	mov	r1, #1
	bl	Func_1a5a4
.L1a26e:
	mov	r0, #0x80
	lsl	r0, #1
.L1a272:
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_1a088

@ LoadAndDrawPortrait
@ r0 = id, r1, r2 = placement. Allocates the tag-0x11 block, draws through
@ Func_1a088, and releases with Func_2dd8. The one-shot form.
.thumb_func_start Func_1a2a4
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	mov	r8, r1
	mov	r1, #0xc1
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r10, r2
	bl	Func_48b0
	mov	r1, r8
	mov	r5, r0
	mov	r0, r6
	bl	Func_1a088
	mov	r3, #0x80
	lsl	r3, #3
	add	r5, r3
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, r10
	bl	Func_3fa4
	mov	r0, #0x11
	bl	Func_2dd8
	mov	r0, #1
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_1a2a4

@ LoadIconSet
@ r0 = index, r1, r2 = placement. Allocates the tag-0x11 block, selects the
@ pointer through Func_1a4c0, and releases with Func_2dd8.
.thumb_func_start Func_1a2ec
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r1, #0xc1
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r8, r2
	bl	Func_48b0
	mov	r5, r0
	mov	r0, r6
	bl	Func_1a4c0
	mov	r3, #0x80
	lsl	r3, #3
	add	r5, r3
	mov	r1, #0x80
	mov	r2, r5
	mov	r0, r8
	bl	Func_3fa4
	mov	r0, #0x11
	bl	Func_2dd8
	mov	r0, #1
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_1a2ec

@ SelectGraphicTable
@ r0 = table selector 1..3, r1, r2 = parameters. A four-way switch returning one
@ of three table base addresses; anything outside 1..3 takes the default arm.
@ Func_3fa4 reserves the tiles for whichever is chosen.
.thumb_func_start Func_1a32c
	push	{lr}
	mov	r3, r2
	cmp	r0, #1
	beq	.L1a342
	cmp	r0, #1
	bcc	.L1a34e
	cmp	r0, #2
	beq	.L1a346
	cmp	r0, #3
	beq	.L1a34a
	b	.L1a34e
.L1a342:
	ldr	r2, .L1a360
	b	.L1a350
.L1a346:
	ldr	r2, .L1a364
	b	.L1a350
.L1a34a:
	ldr	r2, .L1a368
	b	.L1a350
.L1a34e:
	ldr	r2, .L1a36c
.L1a350:
	mov	r0, r3
	mov	r1, #0x20
	bl	Func_3fa4
	mov	r0, #1
	pop	{r1}
	bx	r1
	.align	2, 0
.L1a360:
	.word	Data_31864
.L1a364:
	.word	Data_31864
.L1a368:
	.word	Data_31864
.L1a36c:
	.word	Data_31864
.func_end Func_1a32c

@ SetPortraitPointer
@ r0 = id masked to 0x1FF. Looks the character up with _Func_78414, indexes the
@ 255-entry table at .L29ee4 by the halfword at +6 of that record, and stores the
@ resulting pointer at [iwram_1e94]+0x604. An id of 0 clears it instead.
.thumb_func_start Func_1a370
	push	{r5, r6, lr}
	mov	r5, r0
	ldr	r0, =0x1ff
	ldr	r3, =iwram_1e94
	and	r0, r5
	ldr	r6, [r3]
	bl	_Func_78414
	cmp	r5, #0
	beq	.L1a394
	ldr	r2, =0x604
	ldrh	r3, [r0, #6]
	add	r1, r6, r2
	ldr	r2, =.L29ee4
	lsl	r3, #2
	ldr	r3, [r2, r3]
	str	r3, [r1]
	b	.L1a39e
.L1a394:
	ldr	r2, =.L29ee4
	ldr	r1, =0x604
	ldr	r2, [r2]
	add	r3, r6, r1
	str	r2, [r3]
.L1a39e:
	mov	r2, #0xc0
	lsl	r2, #3
	ldr	r1, =0x602
	add	r3, r6, r2
	mov	r2, #2
	strh	r2, [r3]
	add	r3, r6, r1
	strh	r2, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	Func_1a5a4
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_1a370

@ LoadIconForCharacter
@ r0 = character, r1..r3 and arg5 = placement. The Func_19f98 counterpart for
@ the second table: _Func_78b9c resolves the character and Func_1a404 does the
@ load.
.thumb_func_start Func_1a3d0
	push	{r5, r6, lr}
	mov	r6, r10
	mov	r5, r8
	push	{r5, r6}
	sub	sp, #4
	ldr	r5, [sp, #0x18]
	mov	r6, r1
	mov	r8, r2
	mov	r10, r3
	bl	_Func_78b9c
	mov	r1, r6
	ldrb	r0, [r0, #4]
	mov	r2, r8
	mov	r3, r10
	str	r5, [sp]
	bl	Func_1a404
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_1a3d0

@ LoadIconIndexed
@ r0 = index, r1.. = placement. As Func_19fcc but bounds-checked against
@ Func_19ed0's count of 160 and reading the second pointer table.
.thumb_func_start Func_1a404
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r7, r1
	mov	r1, #0xc1
	mov	r6, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r10, r2
	mov	r9, r3
	bl	Func_48b0
	mov	r1, #0
	mov	r5, r0
	mov	r8, r1
	bl	Func_19ed0
	cmp	r6, r0
	bcc	.L1a430
	mov	r6, #0
.L1a430:
	cmp	r7, #0
	beq	.L1a45a
	ldr	r3, =0x604
	add	r2, r5, r3
	ldr	r3, =.L29a10
	mov	r1, #0xc0
	ldr	r3, [r3, #8]
	lsl	r1, #3
	str	r3, [r2]
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, #0
	bl	Func_1a5a4
	mov	r2, #1
	mov	r8, r2
.L1a45a:
	ldr	r3, =0x604
	ldr	r2, =.L2de88
	add	r1, r5, r3
	lsl	r3, r6, #2
	ldr	r3, [r2, r3]
	str	r3, [r1]
	mov	r1, #0xc0
	lsl	r1, #3
	mov	r2, #2
	add	r3, r5, r1
	add	r1, #2
	strh	r2, [r3]
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, r5
	mov	r1, r8
	bl	Func_1a5a4
	ldr	r2, [sp, #0x1c]
	cmp	r2, #0
	bne	.L1a48c
	bl	Func_4080
	mov	r3, r10
	str	r0, [r3]
.L1a48c:
	mov	r3, #0x80
	mov	r1, r10
	lsl	r3, #3
	ldr	r0, [r1]
	add	r2, r5, r3
	mov	r1, #0x80
	bl	Func_3fa4
	mov	r1, r9
	str	r0, [r1]
	mov	r0, #0x11
	bl	Func_2dd8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_1a404

@ SelectIconPointer
@ r0 = index. Stores .L308a0[index] at [iwram_1e94]+0x604 and sets the two
@ halfwords at +0x600 and +0x602 to 2, marking the block dirty.
.thumb_func_start Func_1a4c0
	push	{lr}
	ldr	r3, =iwram_1e94
	ldr	r1, [r3]
	ldr	r3, =0x604
	add	r2, r1, r3
	ldr	r3, =.L308a0
	lsl	r0, #2
	ldr	r3, [r3, r0]
	mov	r0, #0xc0
	lsl	r0, #3
	str	r3, [r2]
	add	r3, r1, r0
	mov	r2, #2
	add	r0, #2
	strh	r2, [r3]
	add	r3, r1, r0
	strh	r2, [r3]
	mov	r0, r1
	mov	r1, #0
	bl	Func_1a5a4
	pop	{r0}
	bx	r0
.func_end Func_1a4c0

@ LoadNamedGraphic
@ r0 = id, r1, r2, r3 = placement. Allocates the tag-0x11 block, fetches asset
@ 0xF0 with Func_2f40, decompresses with Func_1a5a4, DMA3s it into place and
@ reserves tiles with Func_3fa4 / Func_4080.
.thumb_func_start Func_1a4fc
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #0xc1
	mov	r5, r0
	lsl	r1, #3
	mov	r0, #0x11
	mov	r8, r3
	mov	r7, r2
	bl	Func_48b0
	mov	r6, r0
	ldr	r0, =0xf0
	bl	Func_2f40
	mov	r3, r5
	cmp	r5, #0x7f
	bls	.L1a522
	sub	r3, #0x70
.L1a522:
	lsl	r3, #1
	ldrh	r3, [r3, r0]
	ldr	r1, =0x604
	add	r5, r0, r3
	mov	r3, r5
	add	r2, r6, r1
	add	r3, #0x20
	str	r3, [r2]
	mov	r2, #0xc0
	lsl	r2, #3
	add	r3, r6, r2
	sub	r1, #2
	mov	r2, #4
	strh	r2, [r3]
	add	r3, r6, r1
	strh	r2, [r3]
	mov	r0, r6
	mov	r1, #0
	bl	Func_1a5a4
	ldr	r2, [sp, #0x18]
	cmp	r2, #0
	bne	.L1a556
	bl	Func_4080
	str	r0, [r7]
.L1a556:
	mov	r3, #0x80
	lsl	r3, #3
	mov	r1, #0x80
	add	r2, r6, r3
	ldr	r0, [r7]
	lsl	r1, #2
	bl	Func_3fa4
	mov	r1, r8
	str	r0, [r1]
	mov	r0, #0x11
	bl	Func_2dd8
	ldr	r1, [sp, #0x14]
	ldr	r2, =0x5000200
	lsl	r1, #5
	add	r1, r2
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_1a4fc

@ NoOp
@ A bare `bx lr`. Present so a table entry or a call site has something to point
@ at; calling it does nothing.
.thumb_func_start Func_1a5a0
	bx	lr
.func_end Func_1a5a0

@ DecompressGraphic
@ r0 = compressed source, r1 = destination.
@ Allocates 0x278 bytes under tag 0x31, DMA3-COPIES Func_15afc INTO IT, and
@ calls it there -- the same run-decompressor-from-RAM trick Func_5340 uses in
@ rom_c0. Func_15afc is the move-to-front nibble decoder in rom_15430.s, so the
@ output is unpacked 4bpp ready for Func_15d74 / Func_15e10 to pack.
@ The scratch is released with Func_2dd8 afterwards.
.thumb_func_start Func_1a5a4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r6, r1
	ldr	r5, =0x278
	mov	r0, #0x31
	mov	r1, r5
	bl	Func_48b0
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	mov	r8, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_15afc
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =iwram_1e50
	ldr	r4, =0x604
	add	r3, #0xc4
	add	r2, r7, r4
	ldr	r0, [r2]
	ldr	r3, [r3]
	mov	r1, r7
	bl	_call_via_r3
	mov	r0, #0x31
	bl	Func_2dd8
	cmp	r6, #0
	beq	.L1a5fe
	ldr	r5, =0x9c
	mov	r0, #0x31
	mov	r1, r5
	bl	Func_48b0
	lsr	r5, #2
	mov	r2, r8
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_15d74
	b	.L1a612
.L1a5fe:
	ldr	r5, =0x7c
	mov	r0, #0x31
	mov	r1, r5
	bl	Func_48b0
	lsr	r5, #2
	mov	r2, r8
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_15e10
.L1a612:
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r4, #0xc0
	ldr	r0, =iwram_1e50
	lsl	r4, #3
	mov	r2, #0x80
	add	r3, r7, r4
	lsl	r2, #3
	add	r4, #2
	add	r1, r7, r2
	add	r0, #0xc4
	ldrh	r2, [r3]
	add	r3, r7, r4
	ldrh	r3, [r3]
	ldr	r4, [r0]
	mov	r0, r7
	bl	_call_via_r4
	mov	r0, #0x31
	bl	Func_2dd8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_1a5a4

	.section .rodata

.L29a10:
	.incrom 0x29a10, 0x29acc
.L29acc:
	.incrom 0x29acc, 0x29b68
.L29b68:
	.incrom 0x29b68, 0x29e00
.L29e00:
	.incrom 0x29e00, 0x29ee4
.L29ee4:
	.incrom 0x29ee4, 0x2a2e0
.L2a2e0:
	.incrom 0x2a2e0, 0x2de88
.L2de88:
	.incrom 0x2de88, 0x2e108
.L2e108:
	.incrom 0x2e108, 0x308a0
.L308a0:
	.incrom 0x308a0, 0x310a4
