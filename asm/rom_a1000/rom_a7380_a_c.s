	.include "macros.inc"
	.include "gba.inc"

@ PickCharacterOnce
@ Takes no arguments. Clears state+0x174, runs one pass of Func_a77a4(0), and
@ returns the character id at state+0x21A -- or -1 unchanged when Func_a77a4
@ returned -1.
.thumb_func_start Func_80a7440  @ 0x080a7440
	push	{r5, lr}
	ldr	r3, =iwram_3001f2c
	ldr	r5, [r3]
	mov	r2, #0
	mov	r1, #0xba
	lsl	r1, #1
	add	r3, r5, r1
	strh	r2, [r3]
	mov	r0, #0
	bl	Func_80a77a4
	mov	r3, #1
	neg	r3, r3
	mov	r2, r0
	cmp	r0, r3
	beq	.La7466
	ldr	r1, =0x21a
	add	r3, r5, r1
	ldrb	r2, [r3]
.La7466:
	mov	r0, r2
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_80a7440

@ RunStatusScreen -- MENU INDEX 4
@ Takes no arguments. The Status screen, and the most elaborate of the five. On
@ top of the standard scaffold it:
@
@   * takes two extra scratches, 0x40 and 0x2000 bytes, and saves the palette at
@     0x5000000 and the tiles at 0x6004000 into them, restoring both on the way
@     out -- so it can repaint the whole background and put it back
@   * queues four DMA3 palette transfers to build its own gradient
@   * seeds the eight party-sprite y values at state+0x144.. to 0x1E
@   * calls _Func_7a5bc(-1) and, when the party has anyone in it, spawns the
@     four animated portrait actors through Func_ad274
@   * writes three entries at state+0x234 with x stepping 0x20 from 0x82 and a
@     constant 0x80 at +8
@
@ Func_a76d0 is the state machine; the return value is its answer, or -1 when
@ save bit 0x150 (Start) went up.
.thumb_func_start Func_80a7478  @ 0x080a7478
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r1, #0xa7
	lsl	r1, #4
	mov	r0, #0x37
	sub	sp, #4
	bl	galloc_iwram
	mov	r7, r0
	mov	r0, #0x40
	bl	Func_8004970
	mov	r6, #0x80
	lsl	r6, #6
	mov	r9, r0
	mov	r0, r6
	bl	Func_8004970
	ldr	r3, =iwram_3001e68
	ldr	r2, [r3]
	mov	r3, #1
	mov	r1, #0
	strh	r3, [r2, #4]
	mov	r11, r0
	mov	r2, #0x1e
	mov	r3, #0x14
	mov	r0, #0
	bl	_Func_80170f8
	mov	r0, #1
	bl	WaitFrames
	bl	Func_80a1070
	mov	r0, #0
	bl	Func_80a1090
	mov	r0, #0x88
	lsl	r0, #2
	mov	r2, #0x82
	add	r3, r7, r0
	lsl	r2, #2
	mov	r5, #0
	strh	r5, [r3]
	add	r0, r7, r2
	bl	_Func_80796c4
	ldr	r2, =0x219
	add	r3, r7, r2
	strb	r0, [r3]
	mov	r1, #3
	mov	r3, #7
	mov	r0, #0
	mov	r2, #0
	bl	Func_80a8034
	mov	r1, #0xa0
	lsl	r1, #19
	mov	r2, #0x40
	ldr	r5, =Func_8001af8
	mov	r0, r9
	bl	_call_via_r5
	mov	r0, #0xe
	bl	Func_80a2144
	mov	r1, #0xa0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	lsl	r1, #19
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001c8
	ldr	r2, =0x80000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #4
	ldr	r0, =0x5000200
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001e8
	ldr	r2, =0x80000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, r6
	ldr	r1, =0x6004000
	mov	r0, r11
	bl	_call_via_r5
	ldr	r3, =Func_80008d8
	mov	r1, r6
	ldr	r2, =0x33333333
	ldr	r0, =0x6004000
	bl	_call_via_r3
	mov	r0, #1
	bl	_Func_801e3c8
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #0
	mov	r2, #0x11
	mov	r3, #5
	mov	r0, #0xd
	bl	_CreateUIBox
	mov	r2, #0x86
	lsl	r2, #1
	add	r3, r7, r2
	str	r0, [r3]
	mov	r0, #0xa9
	lsl	r0, #1
	ldr	r1, .La75a0	@ 0x1e
	mov	r2, #7
	add	r3, r7, r0
.La7570:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La7570
	mov	r0, #1
	neg	r0, r0
	bl	_GetNumDjinn
	cmp	r0, #0
	beq	.La7594
	mov	r2, #0x86
	lsl	r2, #1
	add	r3, r7, r2
	ldr	r0, [r3]
	mov	r1, #0
	bl	Func_80ad274
.La7594:
	mov	r2, #0x8d
	lsl	r2, #2
	ldr	r0, .La75a4	@ 0x80
	add	r3, r7, r2
	b	.La75d8

	.align	2, 0
.La75a0:
	.word	0x1e
.La75a4:
	.word	0x80
	.pool

.La75d8:
	mov	r1, #0x82
	mov	r2, #3
.La75dc:
	sub	r2, #1
	strh	r1, [r3]
	strh	r0, [r3, #8]
	add	r1, #0x20
	add	r3, #2
	cmp	r2, #0
	bge	.La75dc
	ldr	r0, =0x6002500
	bl	_Func_80219c8
	bl	Func_80a2474
	mov	r2, #0
	mov	r0, #0x88
	mov	r8, r2
	lsl	r0, #2
	add	r3, r7, r0
	mov	r0, r8
	strh	r0, [r3]
	bl	Func_80a76d0
	mov	r10, r0
	bl	Func_80a2490
	ldr	r0, [r7, #0x24]
	bl	_Func_80164ac
	bl	Func_80ad318
	bl	Func_80a1050
	mov	r3, #0x14
	mov	r1, #0
	mov	r2, #0x1e
	mov	r0, #0
	bl	_Func_80170f8
	mov	r0, #1
	bl	WaitFrames
	bl	_Func_801e318
	mov	r0, #0
	bl	_Func_801e3c8
	mov	r0, #1
	bl	WaitFrames
	mov	r0, #0xa0
	ldr	r5, =Func_8001af8
	mov	r1, r9
	mov	r2, #0x40
	lsl	r0, #19
	bl	_call_via_r5
	mov	r2, #0x80
	mov	r1, r11
	lsl	r2, #6
	ldr	r0, =0x6004000
	bl	_call_via_r5
	mov	r0, r11
	bl	free
	mov	r0, r9
	bl	free
	ldr	r5, =iwram_3001e8c
	ldr	r6, =0xea6
	ldr	r2, [r5]
	mov	r3, #1
	strb	r3, [r2, r6]
	bl	Func_80a34c0
	mov	r1, #0
	mov	r2, #0x1e
	mov	r3, #0x14
	mov	r0, #0
	bl	_Func_80170f8
	mov	r0, #0x37
	bl	gfree
	mov	r3, r5
	sub	r3, #0x24
	ldr	r3, [r3]
	mov	r2, r8
	strh	r2, [r3, #4]
	mov	r0, #1
	bl	WaitFrames
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0x1e
	mov	r3, #0x14
	bl	_ClearUIRegion
	ldr	r3, [r5]
	mov	r0, #0
	add	r3, r6
	strb	r0, [r3]
	add	sp, #4
	mov	r0, r10
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a7478

@ RunStatusStates
@ Takes no arguments. A four-state loop, with save bit 0x150 as the global exit:
@
@     0  Func_a77a4(0) -- pick a character
@     1  Func_a8114    -- the stats page
@     2  Func_a90bc    -- the second page
@     3  Func_a96d8    -- the third page
@
@ Each page hides the cursor at [state+0x14] first by setting its +0x05 to 0x0D.
@ Backing out of a page (-1) returns to state 0 for page 1 and 3, and to state 0
@ via a full reset for page 2. Backing out of state 0 ends the loop.
@ Start at any depth forces the return value to -1.
.thumb_func_start Func_80a76d0  @ 0x080a76d0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f2c
	mov	r5, #0
	mov	r2, #0xd
	ldr	r7, [r3]
	mov	r8, r5
	mov	r6, #0
	mov	r10, r2
	b	.La776e
.La76e8:
	cmp	r5, #1
	beq	.La7722
	cmp	r5, #1
	bgt	.La76f6
	cmp	r5, #0
	beq	.La7700
	b	.La776a
.La76f6:
	cmp	r5, #2
	beq	.La773a
	cmp	r5, #3
	beq	.La7754
	b	.La776a
.La7700:
	mov	r2, #0xba
	lsl	r2, #1
	add	r3, r7, r2
	mov	r2, r8
	strh	r2, [r3]
	mov	r0, #0
	bl	Func_80a77a4
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	bne	.La771e
	mov	r2, #1
	mov	r6, r0
	mov	r8, r2
.La771e:
	mov	r5, #1
	b	.La776e
.La7722:
	ldr	r3, [r7, #0x14]
	mov	r2, r10
	strb	r2, [r3, #5]
	bl	Func_80a8114
	mov	r6, r0
	mvn	r2, r6
	neg	r3, r2
	orr	r3, r2
	lsr	r5, r3, #31
	lsl	r5, #1
	b	.La776e
.La773a:
	ldr	r3, [r7, #0x14]
	mov	r2, r10
	strb	r2, [r3, #5]
	bl	Func_80a90bc
	mov	r3, #1
	mov	r6, r0
	neg	r3, r3
	mov	r5, #0
	cmp	r6, r3
	beq	.La776e
	mov	r5, #3
	b	.La776e
.La7754:
	ldr	r3, [r7, #0x14]
	mov	r2, r10
	strb	r2, [r3, #5]
	bl	Func_80a96d8
	mov	r6, r0
	mvn	r2, r6
	neg	r3, r2
	orr	r3, r2
	lsr	r5, r3, #31
	b	.La776e
.La776a:
	mov	r3, #1
	mov	r8, r3
.La776e:
	mov	r2, r8
	cmp	r2, #0
	bne	.La7780
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.La76e8
.La7780:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.La7790
	mov	r6, #1
	neg	r6, r6
.La7790:
	mov	r0, r6
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a76d0

@ SelectStatusCharacter
@ r0 = which cursor. Shows the cursor at state+0x14 + r0*4, releases the header
@ window's nodes and -- when save bit 0x172 is set -- clears a 3x9 block with
@ _Func_1e41c. Glides to (index * 24 - 10, 0x10) unless the selection is -1, in
@ which case it resets to member 0. Then runs Func_a7d68 when state+0x220 is 3
@ and Func_a7a34 otherwise.
.thumb_func_start Func_80a77a4  @ 0x080a77a4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r2, #0x1c
	add	r2, r0
	ldr	r3, =iwram_3001f2c
	lsl	r0, #2
	mov	r10, r0
	ldr	r5, [r3]
	mov	r3, r10
	add	r3, #0x14
	ldr	r0, [r5, r3]
	mov	r6, #0
	mov	r3, #1
	strb	r3, [r0, #5]
	strh	r6, [r0, #0xc]
	ldr	r0, [r5, #0x10]
	sub	sp, #4
	mov	r8, r2
	ldrsb	r7, [r5, r2]
	bl	_Func_8016498
	mov	r0, #0xb9
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	beq	.La77ee
	mov	r3, #3
	ldr	r0, [r5, #0x10]
	mov	r1, #9
	str	r3, [sp]
	mov	r2, #1
	mov	r3, #9
	bl	_Func_801e41c
.La77ee:
	mov	r3, #1
	neg	r3, r3
	cmp	r7, r3
	bne	.La77fe
	ldr	r3, .La7820	@ 0
	mov	r2, r8
	strb	r3, [r5, r2]
	b	.La780c
.La77fe:
	lsl	r0, r7, #1
	add	r0, r7
	lsl	r0, #3
	sub	r0, #0xa
	mov	r1, #0x10
	bl	Func_80a1ac0
.La780c:
	mov	r2, #0x88
	lsl	r2, #2
	add	r3, r5, r2
	ldrh	r3, [r3]
	cmp	r3, #3
	bne	.La7828
	bl	Func_80a7d68
	b	.La782c

	.align	2, 0
.La7820:
	.word	0
	.pool

.La7828:
	bl	Func_80a7a34
.La782c:
	mov	r6, r0
	mov	r3, r10
	add	r3, #0x14
	ldr	r0, [r5, r3]
	bl	Func_80a17c4
	mov	r0, #1
	bl	WaitFrames
	mov	r0, r6
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a77a4

@ DrawStatusPage
@ r0.. = placement. Paints the body of the stats page: clears with _Func_1e41c,
@ plots the frame tiles one at a time through _Func_19000, and prints labels
@ 0xB17 and 0xB18 and the 0x45F block. Func_a9d84 rewinds the sprites and the
@ tilemap dirty byte at [iwram_1e8c]+0xEA3 is raised at the end.
@ 219 lines; traced structurally.
.thumb_func_start Func_80a7850  @ 0x080a7850
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	sub	sp, #0x1c
	str	r3, [sp, #0x18]
	bl	Func_80a9d84
	mov	r0, #0x70
	bl	_PlaySound
	mov	r1, #0x86
	ldr	r0, [sp, #0x18]
	lsl	r1, #1
	add	r5, r0, r1
	ldr	r0, [r5]
	bl	_Func_8016498
	ldr	r1, [r5]
	ldr	r0, =0xb17
	mov	r2, #0
	mov	r3, #0x10
	bl	_Func_801e7c0
	ldr	r2, [sp, #0x18]
	ldr	r3, [r2, #0x14]
	mov	r2, #0xd
	strb	r2, [r3, #5]
	ldr	r0, [sp, #0x18]
	mov	r1, #0xbe
	lsl	r1, #1
	add	r3, r0, r1
	ldr	r3, [r3]
	mov	r0, #1
	strb	r2, [r3, #5]
	bl	WaitFrames
	ldr	r2, [sp, #0x18]
	ldr	r3, [sp, #0x18]
	ldr	r2, [r2, #0x24]
	mov	r0, #0xa2
	mov	r1, #0x8d
	lsl	r0, #1
	lsl	r1, #2
	ldr	r5, .La78c4	@ 0x46
	ldr	r4, .La78c8	@ 0x1e
	mov	r10, r2
	add	r2, r3, r0
	add	r3, r1
	mov	r0, #0x20
	mov	r1, #3
	b	.La78d4

	.align	2, 0
.La78c4:
	.word	0x46
.La78c8:
	.word	0x1e
	.pool

.La78d4:
	sub	r1, #1
	strh	r0, [r3]
	strh	r5, [r3, #8]
	add	r0, #0x38
	strh	r4, [r2]
	add	r3, #2
	add	r2, #2
	cmp	r1, #0
	bge	.La78d4
	mov	r0, r10
	bl	_Func_8016498
	mov	r3, #0xb
	str	r3, [sp]
	mov	r0, r10
	mov	r1, #0
	mov	r2, #0xb
	mov	r3, #0x1c
	bl	_Func_801e41c
	mov	r1, #0x86
	ldr	r2, [sp, #0x18]
	lsl	r1, #1
	add	r3, r2, r1
	mov	r2, #0x60
	ldr	r1, [r3]
	neg	r2, r2
	ldr	r0, =0xb18
	mov	r3, #0x84
	bl	_DrawSmallText
	mov	r2, #0
	str	r2, [sp, #0xc]
	str	r2, [sp, #8]
	str	r2, [sp, #4]
	mov	r9, r2
.La791c:
	ldr	r3, [sp, #4]
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #0xc]
	mov	r2, #0x18
	mov	r5, r3
	str	r3, [sp, #0x14]
	str	r0, [sp, #0x10]
	mov	r6, #0
	mov	r11, r1
	mov	r8, r2
	add	r5, #0x30
.La7932:
	ldr	r3, [sp, #0x14]
	mov	r0, r5
	add	r7, r3, r6
	bl	_GetFlag
	cmp	r0, #0
	beq	.La7964
	mov	r2, #0
	str	r2, [sp]
	ldr	r1, =0x1001
	ldr	r2, [sp, #0x10]
	add	r3, r6, #3
	mov	r0, r10
	add	r1, r9
	add	r2, #1
	bl	_Func_8019000
	ldr	r0, =0x45f
	mov	r2, r11
	add	r0, r7, r0
	mov	r1, r10
	add	r2, #0x10
	mov	r3, r8
	bl	_Func_801e7c0
.La7964:
	mov	r0, #8
	add	r6, #1
	add	r8, r0
	add	r5, #1
	cmp	r6, #6
	ble	.La7932
	ldr	r1, [sp, #0xc]
	ldr	r2, [sp, #8]
	ldr	r3, [sp, #4]
	mov	r0, #1
	add	r1, #0x38
	add	r9, r0
	str	r1, [sp, #0xc]
	add	r2, #7
	add	r3, #0x14
	mov	r1, r9
	str	r2, [sp, #8]
	str	r3, [sp, #4]
	cmp	r1, #3
	ble	.La791c
	ldr	r3, =iwram_3001e8c
	ldr	r2, =0xea3
	ldr	r3, [r3]
	add	r3, r2
	mov	r2, #1
	strb	r2, [r3]
	ldr	r6, =gKeyPress
	mov	r5, #7
.La799c:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La79b6
	mov	r0, #1
	bl	WaitFrames
	ldr	r3, [r6]
	and	r3, r5
	cmp	r3, #0
	beq	.La799c
.La79b6:
	ldr	r3, [sp, #0x18]
	ldr	r0, [r3, #0x24]
	bl	_Func_8016498
	mov	r1, #0x86
	ldr	r0, [sp, #0x18]
	lsl	r1, #1
	add	r3, r0, r1
	ldr	r0, [r3]
	bl	_Func_80164ac
	mov	r1, #0x8d
	ldr	r2, [sp, #0x18]
	lsl	r1, #2
	ldr	r0, .La79dc	@ 0x80
	add	r3, r2, r1
	mov	r1, #0x82
	mov	r2, #3
	b	.La79f8

	.align	2, 0
.La79dc:
	.word	0x80
	.pool

.La79f8:
	sub	r2, #1
	strh	r1, [r3]
	strh	r0, [r3, #8]
	add	r1, #0x20
	add	r3, #2
	cmp	r2, #0
	bge	.La79f8
	ldr	r2, [sp, #0x18]
	ldr	r3, [r2, #0x14]
	mov	r2, #1
	strb	r2, [r3, #5]
	ldr	r0, [sp, #0x18]
	mov	r1, #0xbe
	lsl	r1, #1
	add	r3, r0, r1
	ldr	r3, [r3]
	mov	r0, #0x71
	strb	r2, [r3, #5]
	bl	_PlaySound
	add	sp, #0x1c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a7850

@ RunStatsPage
@ Takes no arguments. The first status page: draws the character at state+0x21A
@ with Func_a7850 and Func_a8088, prints labels 0xB0D and 0xB16, and loops on
@ the d-pad. Left and Right change character; L and R swap party order through
@ Func_a7f44, which destroys and respawns the actors (Func_a195c then
@ Func_a1870) so the strip re-sorts. 376 lines; traced structurally.
.thumb_func_start Func_80a7a34  @ 0x080a7a34
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	mov	r1, #0x1c
	ldrsb	r1, [r3, r1]
	mov	r2, #0x1e
	ldrsb	r2, [r3, r2]
	mov	r8, r3
	sub	sp, #0xc
	mov	r3, #1
	str	r3, [sp, #8]
	mov	r10, r1
	mov	r3, #0x88
	mov	r1, #0
	str	r1, [sp, #4]
	lsl	r3, #2
	add	r3, r8
	ldrh	r3, [r3]
	mov	r11, r2
	mov	r1, #0x82
	mov	r2, r10
	lsl	r1, #2
	lsl	r7, r2, #1
	mov	r9, r3
	mov	r2, r8
	add	r3, r7, r1
	ldrh	r0, [r2, r3]
	bl	_GetUnit
	mov	r3, #0x8d
	lsl	r3, #2
	ldr	r0, .La7ab0	@ 0x80
	add	r3, r8
	mov	r1, #0x82
	mov	r2, #3
.La7a86:
	sub	r2, #1
	strh	r1, [r3]
	strh	r0, [r3, #8]
	add	r1, #0x20
	add	r3, #2
	cmp	r2, #0
	bge	.La7a86
	mov	r0, #0xe
	bl	Func_80a2144
	mov	r1, #0xa0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	lsl	r1, #19
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001c8
	ldr	r2, =0x80000001
	b	.La7acc

	.align	2, 0
.La7ab0:
	.word	0x80
	.pool

.La7acc:
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #4
	ldr	r0, =0x5000200
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001e8
	ldr	r2, =0x80000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	b	.La7d26
.La7ae6:
	ldr	r3, [sp, #8]
	cmp	r3, #0
	beq	.La7bac
	mov	r1, #0
	mov	r5, #0x86
	str	r1, [sp, #8]
	lsl	r5, #1
	add	r5, r8
	ldr	r0, [r5]
	bl	_Func_8016498
	ldr	r6, =0xb0d
	ldr	r1, [r5]
	mov	r0, r6
	mov	r2, #0
	mov	r3, #0
	bl	_Func_801e7c0
	mov	r0, #0x30
	bl	_GetFlag
	cmp	r0, #0
	beq	.La7b20
	ldr	r1, [r5]
	ldr	r0, =0xb16
	mov	r2, #0
	mov	r3, #0x10
	bl	_Func_801e7c0
.La7b20:
	ldr	r1, [r5]
	sub	r0, r6, #3
	mov	r2, #0
	mov	r3, #8
	bl	_Func_801e7c0
	mov	r0, r10
	mov	r1, r11
	add	r0, r11
	bl	__modsi3
	mov	r2, #0x82
	lsl	r2, #2
	lsl	r7, r0, #1
	add	r5, r7, r2
	mov	r3, r8
	mov	r10, r0
	ldrh	r0, [r3, r5]
	bl	_GetUnit
	mov	r0, r9
	mov	r1, #3
	add	r0, #3
	bl	__modsi3
	mov	r1, r8
	mov	r9, r0
	ldrh	r0, [r1, r5]
	mov	r1, r9
	bl	Func_80a8088
	mov	r2, r8
	ldrh	r1, [r2, r5]
	mov	r0, r8
	bl	Func_80a1804
	mov	r3, #0xa9
	lsl	r3, #1
	ldr	r1, .La7b8c	@ 0x1e
	mov	r6, r7
	mov	r2, #7
	add	r3, r8
.La7b74:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La7b74
	mov	r3, #0xa2
	lsl	r3, #1
	add	r2, r6, r3
	ldr	r3, .La7b90	@ 0x1a
	mov	r1, r8
	strh	r3, [r1, r2]
	b	.La7bb0

	.align	2, 0
.La7b8c:
	.word	0x1e
.La7b90:
	.word	0x1a
	.pool

.La7bac:
	mov	r2, r10
	lsl	r7, r2, #1
.La7bb0:
	mov	r3, r10
	add	r0, r7, r3
	lsl	r0, #3
	mov	r1, #0x10
	sub	r0, #0xa
	bl	Func_80a1a40
	mov	r0, #1
	bl	WaitFrames
	ldr	r1, =gKeyPress
	ldr	r2, [r1]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La7bdc
	mov	r0, #0x70
	bl	_PlaySound
	mov	r1, #1
	str	r1, [sp, #4]
	b	.La7d38
.La7bdc:
	ldr	r6, [r1]
	mov	r3, #2
	and	r6, r3
	cmp	r6, #0
	beq	.La7bf4
	mov	r0, #0x71
	bl	_PlaySound
	mov	r2, #1
	neg	r2, r2
	str	r2, [sp, #4]
	b	.La7d38
.La7bf4:
	ldr	r7, =gKeyRepeat
	mov	r3, #0x80
	ldr	r5, [r7]
	lsl	r3, #1
	and	r5, r3
	cmp	r5, #0
	beq	.La7c58
	mov	r0, r10
	mov	r1, #1
	bl	Func_80a7f44
	cmp	r0, #0
	beq	.La7cb6
	mov	r0, #0x70
	bl	_PlaySound
	mov	r3, #1
	add	r10, r3
	bl	Func_80a195c
	mov	r1, r8
	ldr	r0, [r1, #0x10]
	mov	r2, #2
	mov	r1, #2
	mov	r3, #8
	str	r6, [sp]
	bl	Func_80a1870
	mov	r3, #0xa9
	lsl	r3, #1
	ldr	r1, .La7c4c	@ 0x1e
	mov	r2, #7
	add	r3, r8
.La7c36:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La7c36
	mov	r2, r10
	mov	r3, #0xa2
	lsl	r3, #1
	lsl	r7, r2, #1
	b	.La7cac

	.align	2, 0
.La7c4c:
	.word	0x1e
	.pool

.La7c58:
	ldr	r2, [r7]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La7ccc
	mov	r0, r10
	mov	r1, #0
	bl	Func_80a7f44
	cmp	r0, #0
	beq	.La7cb6
	mov	r0, #0x70
	bl	_PlaySound
	mov	r2, #1
	neg	r2, r2
	add	r10, r2
	bl	Func_80a195c
	mov	r3, r8
	ldr	r0, [r3, #0x10]
	mov	r1, #2
	mov	r2, #2
	mov	r3, #8
	str	r5, [sp]
	bl	Func_80a1870
	mov	r3, #0xa9
	lsl	r3, #1
	ldr	r1, =0x1e
	mov	r2, #7
	add	r3, r8
.La7c9a:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La7c9a
	mov	r1, r10
	mov	r3, #0xa2
	lsl	r7, r1, #1
	lsl	r3, #1
.La7cac:
	add	r2, r7, r3
	ldr	r3, =0x1a
	mov	r1, r8
	strh	r3, [r1, r2]
	b	.La7cbc
.La7cb6:
	mov	r0, #0x72
	bl	_PlaySound
.La7cbc:
	mov	r0, #1
	bl	WaitFrames
	b	.La7d26

	.pool_aligned

.La7ccc:
	ldr	r2, [r1]
	mov	r3, #4
	and	r2, r3
	cmp	r2, #0
	beq	.La7cea
	mov	r0, #0x30
	bl	_GetFlag
	cmp	r0, #0
	beq	.La7cea
	bl	Func_80a7850
	mov	r2, #1
	str	r2, [sp, #8]
	b	.La7d26
.La7cea:
	ldr	r2, [r7]
	mov	r3, #0x20
	and	r2, r3
	cmp	r2, #0
	beq	.La7d0a
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r3, r11
	cmp	r3, #1
	ble	.La7d0a
	mov	r1, #1
	mov	r2, #1
	neg	r1, r1
	str	r2, [sp, #8]
	add	r10, r1
.La7d0a:
	ldr	r2, [r7]
	mov	r3, #0x10
	and	r2, r3
	cmp	r2, #0
	beq	.La7d26
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r3, r11
	cmp	r3, #1
	ble	.La7d26
	mov	r1, #1
	str	r1, [sp, #8]
	add	r10, r1
.La7d26:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_GetFlag
	cmp	r0, #0
	bne	.La7d34
	b	.La7ae6
.La7d34:
	mov	r2, r10
	lsl	r7, r2, #1
.La7d38:
	mov	r1, r8
	mov	r3, r10
	strb	r3, [r1, #0x1c]
	mov	r3, #0x82
	lsl	r3, #2
	add	r2, r7, r3
	ldrh	r3, [r1, r2]
	str	r3, [r1, #8]
	ldr	r3, =0x21a
	ldrh	r2, [r1, r2]
	add	r3, r8
	strb	r2, [r3]
	ldr	r0, [sp, #4]
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a7a34

@ RunEquipPage
@ Takes no arguments. The equipment view of the status screen -- reached when
@ state+0x220 is 3. Builds the filtered item list with Func_a68ec, loads the
@ icons with Func_a68a8, draws through Func_a8088 and Func_a9b94, and prints
@ string 0xC05. 217 lines; traced structurally.
.thumb_func_start Func_80a7d68  @ 0x080a7d68
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001f2c
	ldr	r7, [r3]
	mov	r0, #0x1c
	ldrsb	r0, [r7, r0]
	sub	sp, #8
	mov	r2, #1
	mov	r8, r0
	mov	r0, #0x88
	mov	r1, #0x1e
	ldrsb	r1, [r7, r1]
	lsl	r0, #2
	str	r2, [sp, #4]
	add	r3, r7, r0
	ldrh	r3, [r3]
	mov	r9, r1
	mov	r1, r8
	str	r3, [sp]
	lsl	r1, #1
	mov	r3, #0x82
	mov	r10, r1
	lsl	r3, #2
	add	r3, r10
	ldrh	r0, [r7, r3]
	bl	_GetUnit
	mov	r2, #0x86
	lsl	r2, #1
	add	r6, r7, r2
	ldr	r0, [r6]
	bl	_Func_8016498
	ldr	r5, =0xc05
	ldr	r1, [r6]
	mov	r0, r5
	mov	r2, #0
	mov	r3, #0
	add	r5, #1
	bl	_Func_801e7c0
	mov	r3, #0x10
	ldr	r1, [r6]
	mov	r0, r5
	mov	r2, #0
	bl	_Func_801e7c0
	mov	r3, #0xe4
	lsl	r3, #1
	add	r3, r7
	mov	r11, r3
.La7dd8:
	ldr	r0, [sp, #4]
	cmp	r0, #0
	beq	.La7e84
	mov	r1, #0
	mov	r0, r8
	str	r1, [sp, #4]
	add	r0, r9
	mov	r1, r9
	bl	__modsi3
	mov	r8, r0
	mov	r2, r8
	lsl	r2, #1
	mov	r5, #0x82
	mov	r10, r2
	lsl	r5, #2
	add	r5, r10
	ldrh	r0, [r7, r5]
	bl	_GetUnit
	ldr	r0, [sp]
	mov	r1, #3
	add	r0, #3
	bl	__modsi3
	str	r0, [sp]
	ldr	r1, [sp]
	ldrh	r0, [r7, r5]
	bl	Func_80a8088
	ldrh	r1, [r7, r5]
	mov	r0, r7
	bl	Func_80a1804
	mov	r0, #0xa9
	lsl	r0, #1
	ldr	r1, .La7e58	@ 0x1e
	mov	r6, r10
	mov	r2, #7
	add	r3, r7, r0
.La7e28:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La7e28
	mov	r1, #0xa2
	lsl	r1, #1
	ldr	r3, .La7e5c	@ 0x1a
	add	r2, r6, r1
	strh	r3, [r7, r2]
	mov	r2, #0x82
	lsl	r2, #2
	add	r3, r6, r2
	ldrh	r0, [r7, r3]
	bl	_GetUnit
	mov	r2, #0
	mov	r1, r11
	bl	Func_80a68ec
	mov	r1, #0x86
	lsl	r1, #2
	b	.La7e68

	.align	2, 0
.La7e58:
	.word	0x1e
.La7e5c:
	.word	0x1a
	.pool

.La7e68:
	add	r3, r7, r1
	strb	r0, [r3]
	mov	r0, r11
	bl	Func_80a68a8
	mov	r0, #0x60
	mov	r1, #0x60
	mov	r2, #8
	bl	Func_80a9b94
	mov	r0, r11
	bl	Func_80a3d24
	b	.La7e8a
.La7e84:
	mov	r2, r8
	lsl	r2, #1
	mov	r10, r2
.La7e8a:
	mov	r0, r10
	add	r0, r8
	lsl	r0, #3
	mov	r1, #0x10
	sub	r0, #0xa
	bl	Func_80a1a40
	mov	r0, #1
	bl	WaitFrames
	ldr	r1, =gKeyPress
	ldr	r2, [r1]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La7eb4
	mov	r0, #0x70
	bl	_PlaySound
	mov	r0, #1
	b	.La7f0e
.La7eb4:
	ldr	r2, [r1]
	mov	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La7eca
	mov	r0, #0x71
	bl	_PlaySound
	mov	r0, #1
	neg	r0, r0
	b	.La7f0e
.La7eca:
	ldr	r5, =gKeyRepeat
	ldr	r2, [r5]
	mov	r3, #0x20
	and	r2, r3
	cmp	r2, #0
	beq	.La7eec
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r3, r9
	cmp	r3, #1
	ble	.La7eec
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	str	r1, [sp, #4]
	add	r8, r0
.La7eec:
	ldr	r2, [r5]
	mov	r3, #0x10
	and	r2, r3
	cmp	r2, #0
	bne	.La7ef8
	b	.La7dd8
.La7ef8:
	mov	r0, #0x6f
	bl	_PlaySound
	mov	r2, r9
	cmp	r2, #1
	bgt	.La7f06
	b	.La7dd8
.La7f06:
	mov	r3, #1
	add	r8, r3
	str	r3, [sp, #4]
	b	.La7dd8
.La7f0e:
	mov	r1, r8
	mov	r2, #0x82
	strb	r1, [r7, #0x1c]
	lsl	r2, #2
	add	r2, r10
	ldrh	r3, [r7, r2]
	str	r3, [r7, #8]
	ldr	r1, =0x21a
	ldrh	r2, [r7, r2]
	add	r3, r7, r1
	strb	r2, [r3]
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a7d68

@ SwapPartyOrder
@ r0 = index, r1 = 1 to swap with the NEXT member, anything else the previous.
@ Reorders the party. It cannot swap past either end -- index 0 with r1 zero, or
@ the last index with r1 one, both return 0 -- and a party of one always
@ returns 0.
@
@ The swap is done the long way round, because the roster is derived state:
@ the ids are copied to a stack array, two entries exchanged, then EVERY member
@ is removed with _Func_79664 and re-added with _Func_7961c in the new order,
@ and _Func_796c4 re-reads the result back into state+0x208. Returns 1.
.thumb_func_start Func_80a7f44  @ 0x080a7f44
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	ldr	r2, =0x219
	mov	r8, r3
	add	r2, r8
	ldrb	r3, [r2]
	sub	sp, #0x38
	cmp	r3, #1
	bls	.La7f6a
	cmp	r1, #1
	bne	.La7f6e
	ldrb	r3, [r2]
	sub	r3, #1
	cmp	r0, r3
	bne	.La7f72
.La7f6a:
	mov	r0, #0
	b	.La801e
.La7f6e:
	cmp	r0, #0
	beq	.La7f6a
.La7f72:
	mov	r7, sp
	mov	r2, #0
	add	r3, sp, #0x34
	mov	r12, r7
.La7f7a:
	str	r2, [r3]
	sub	r3, #4
	cmp	r3, r12
	bge	.La7f7a
	ldr	r3, =0x219
	add	r3, r8
	ldrb	r3, [r3]
	mov	r6, #0
	cmp	r6, r3
	bge	.La7fa8
	ldr	r5, =0x219
	mov	r2, #0x82
	lsl	r2, #2
	add	r5, r8
	mov	r4, r7
	add	r2, r8
.La7f9a:
	ldrh	r3, [r2]
	stmia	r4!, {r3}
	ldrb	r3, [r5]
	add	r6, #1
	add	r2, #2
	cmp	r6, r3
	blt	.La7f9a
.La7fa8:
	cmp	r1, #1
	bne	.La7fb2
	lsl	r3, r0, #2
	add	r1, r3, #4
	b	.La7fb6
.La7fb2:
	lsl	r3, r0, #2
	sub	r1, r3, #4
.La7fb6:
	ldr	r6, [r7, r3]
	ldr	r2, [r7, r1]
	str	r2, [r7, r3]
	str	r6, [r7, r1]
	ldr	r3, =0x219
	add	r3, r8
	ldrb	r3, [r3]
	mov	r6, #0
	cmp	r6, r3
	bge	.La7fe8
	ldr	r2, =0x219
	mov	r5, #0x82
	add	r2, r8
	lsl	r5, #2
	mov	r10, r2
	add	r5, r8
.La7fd6:
	ldrh	r0, [r5]
	bl	_Func_8079664
	mov	r2, r10
	ldrb	r3, [r2]
	add	r6, #1
	add	r5, #2
	cmp	r6, r3
	blt	.La7fd6
.La7fe8:
	ldr	r3, =0x219
	add	r3, r8
	ldrb	r3, [r3]
	mov	r6, #0
	cmp	r6, r3
	bge	.La800c
	ldr	r3, =0x219
	add	r3, r8
	mov	r10, r3
	mov	r5, r7
.La7ffc:
	ldmia	r5!, {r0}
	bl	_AddPartyMember
	mov	r2, r10
	ldrb	r3, [r2]
	add	r6, #1
	cmp	r6, r3
	blt	.La7ffc
.La800c:
	mov	r0, #0x82
	lsl	r0, #2
	add	r0, r8
	bl	_Func_80796c4
	ldr	r3, =0x219
	add	r3, r8
	strb	r0, [r3]
	mov	r0, #1
.La801e:
	add	sp, #0x38
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a7f44
