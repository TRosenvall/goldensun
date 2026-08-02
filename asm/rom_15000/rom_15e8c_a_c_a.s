	.include "macros.inc"
	.include "gba.inc"

@ InitUiSystem
@ Takes no arguments. Brings up the whole UI layer:
@     galloc_ewram(0xF, 0x12FC) allocates the UI block -- this is what iwram_1e8c
@       points at, and 0x12FC is its size, so every +0xNNN offset in this module
@       is bounded by it
@     the block is DMA-cleared, then defaults are written:
@       +0xEA3 = 1 (dirty mask, see Func_160fc), +0xEA7 = 0x0F, +0x12B6 = 0x63
@     the first 0x140 bytes are filled with 0xF000F000, the empty-tile pattern
@     Func_15ef4 builds the node free list
@     Func_19d0c initialises the menu layer
@     StartTask registers Func_160fc as the per-frame flush at priority 0x480
@     Func_173f4 finishes setup
@ Func_16018 below is the same sequence with one extra field and a different
@ finisher; use this one for the plain case.
.thumb_func_start Func_8015f30  @ 0x08015f30
	push	{r5, lr}
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	galloc_ewram
	mov	r3, #0
	mov	r4, r0
	mov	r5, sp
	str	r3, [r5]
	mov	r0, r5
	ldr	r3, =REG_DMA3SAD
	mov	r1, r4
	ldr	r2, =0x850004bf
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0xea3
	add	r2, r4, r3
	mov	r3, #1
	strb	r3, [r2]
	ldr	r3, =0x12b6
	add	r2, r4, r3
	mov	r3, #0x63
	strh	r3, [r2]
	ldr	r3, =0xea7
	add	r2, r4, r3
	mov	r3, #0xf
	strb	r3, [r2]
	ldr	r3, =0xf000f000
	mov	r0, r5
	str	r3, [r5]
	ldr	r2, =0x85000140
	ldr	r3, =REG_DMA3SAD
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bl	Func_8015ef4
	bl	Func_8019d0c
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_80160fc
	bl	StartTask
	bl	Func_80173f4
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_8015f30

@ CopyTileHalf
@ r0 = source tile index, r1 = destination tile index (both masked to 0x3FF).
@ DMA3-copies 0x10 bytes -- half a 4bpp tile, four pixel rows -- from
@ 0x6000010 + src*0x20 to 0x6000000 + dst*0x20, then clears 0x14 bytes at
@ 0x600000C + dst*0x20 with Func_8d4.
@ The source is read from +0x10 within its tile and the destination written
@ from +0x00, so this shifts the copied rows upward by four as it goes.
@ Called by Func_16018 with (0xF013, 0x80); note 0xF013 is a text control code
@ and only its low 10 bits survive the mask.
.thumb_func_start Func_8015fb8  @ 0x08015fb8
	push	{lr}
	mov	r12, r3
	mov	r3, r9
	push	{r3}
	mov	r3, r12
	mov	r3, r0
	ldr	r0, =0x3ff
	mov	r2, r9
	sub	sp, #4
	mov	r4, r1
	str	r2, [sp]
	and	r4, r0
	ldr	r2, =0x6000010
	and	r0, r3
	lsl	r0, #5
	lsl	r4, #5
	add	r0, r2
	sub	r2, #0x10
	add	r1, r4, r2
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x80000008
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =0x600000c
	add	r4, r3
	mov	r0, r4
	ldr	r3, =Func_80008d4
	mov	r1, #0x14
	bl	_call_via_r3
	add	sp, #4
	pop	{r3}
	mov	r9, r3
	pop	{r1}
	bx	r1
.func_end Func_8015fb8

@ InitUiSystemWithMode
@ r0 = mode, passed on to .gcc2_compiled..
@ The same bring-up as Func_15f30 -- same 0x12FC allocation under tag 0xF, same
@ clear, same defaults, same node pool, same Func_160fc registration -- with two
@ differences: it also sets +0xEA5 = 1, and it finishes with .gcc2_compiled.(mode)
@ instead of Func_173f4, then seeds a tile through Func_15fb8(0xF013, 0x80).
@ It does NOT call Func_19d0c, so the menu layer is left uninitialised.
.thumb_func_start Func_8016018  @ 0x08016018
	push	{r5, r6, lr}
	mov	r6, r9
	push	{r6}
	mov	r6, r0
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	galloc_ewram
	mov	r3, #0
	mov	r5, r0
	mov	r4, sp
	str	r3, [r4]
	mov	r0, r4
	ldr	r3, =REG_DMA3SAD
	mov	r1, r5
	ldr	r2, =0x850004bf
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r1, =0xea3
	add	r3, r5, r1
	mov	r1, #1
	strb	r1, [r3]
	ldr	r3, =0x12b6
	add	r2, r5, r3
	mov	r3, #0x63
	strh	r3, [r2]
	ldr	r2, =0xea5
	add	r3, r5, r2
	strb	r1, [r3]
	ldr	r3, =0xea7
	add	r2, r5, r3
	mov	r3, #0xf
	strb	r3, [r2]
	ldr	r3, =0xf000f000
	mov	r0, r4
	str	r3, [r4]
	mov	r1, r5
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000140
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bl	Func_8015ef4
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_80160fc
	bl	StartTask
	mov	r0, r6
	bl	Func_8017464
	add	r1, sp, #4
	mov	r9, r1
	ldr	r0, =0xf013
	mov	r1, #0x80
	bl	Func_8015fb8
	add	r2, sp, #4
	mov	r9, r2
	mov	r1, #0x81
	ldr	r0, =0xf014
	bl	Func_8015fb8
	add	r3, sp, #4
	mov	r9, r3
	mov	r1, #0x82
	ldr	r0, =0xf015
	bl	Func_8015fb8
	ldr	r1, =0xda2
	mov	r2, #4
	mov	r3, #2
	add	r5, r1
.L160ac:
	sub	r3, #1
	strb	r2, [r5]
	sub	r5, #1
	cmp	r3, #0
	bge	.L160ac
	add	sp, #4
	pop	{r3}
	mov	r9, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8016018

@ FlushUiToVram
@ Takes no arguments. Registered by Func_15f30/Func_16018 as a per-frame task.
@ Does nothing while the suspend flag at [iwram_1e8c]+0xEA6 is non-zero.
@ Otherwise it reads the DIRTY MASK at +0xEA3 and DMA-copies one 0x100-byte
@ block to 0x6002000 for each set bit, advancing both source and destination by
@ 0x100 per bit. Bit 0 means "everything": it replaces the mask with 0x3F so all
@ rows are sent. The mask is shifted right once before the loop, so bits 1..5
@ are the five individually-dirtyable blocks. The mask is cleared afterwards.
@ Everything else in this module marks work by setting bits here; this is the
@ only place that touches VRAM for it.
.thumb_func_start Func_80160fc  @ 0x080160fc
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001e8c
	ldr	r2, =0xea6
	ldr	r7, [r3]
	add	r3, r7, r2
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L1615a
	sub	r2, #3
	add	r3, r7, r2
	ldrb	r4, [r3]
	cmp	r4, #0
	beq	.L1615a
	ldr	r3, =0x6002000
	mov	r12, r3
	mov	r3, #1
	and	r3, r4
	mov	r5, r7
	cmp	r3, #0
	beq	.L16126
	mov	r4, #0x3f
.L16126:
	mov	r3, #0x3f
	and	r4, r3
	mov	r2, #1
	mov	r6, #0x80
	lsr	r4, #1
	mov	r14, r2
	lsl	r6, #1
.L16134:
	mov	r3, r4
	mov	r2, r14
	and	r3, r2
	cmp	r3, #0
	beq	.L1614a
	ldr	r3, =REG_DMA3SAD
	mov	r0, r5
	mov	r1, r12
	ldr	r2, =0x84000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.L1614a:
	lsr	r4, #1
	add	r5, r6
	add	r12, r6
	cmp	r4, #0
	bne	.L16134
	ldr	r2, =0xea3
	add	r3, r7, r2
	strb	r4, [r3]
.L1615a:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80160fc

@ RestoreTilemapRect
@ r0 = x column, r1 = y row, r2 = width, r3 = height, all in TILES.
@ Writes the saved background back over a rectangle of the 30x20 tilemap, which
@ is how a window erases itself when it closes.
@ The tilemap index is (row * 32 + column) * 2, so the map stride is 32 entries
@ even though only 30 are visible.
@ Everything is clamped before use: width and height to 2..0x1E, and the height
@ is further trimmed so row + height never exceeds 0x14 (20 rows). Callers can
@ therefore pass sloppy geometry without running off the map.
.thumb_func_start ClearUIRegion  @ 0x08016178
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r3
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r10, r3
	lsl	r3, r1, #5
	add	r3, r0
	lsl	r3, #1
	mov	r6, r2
	mov	r2, r10
	add	r5, r3, r2
	mov	r4, #0xf0
	add	r3, r1, r7
	sub	sp, #4
	mov	r8, r1
	lsl	r4, #8
	cmp	r3, #0x14
	bls	.L161a6
	mov	r3, #0x14
	sub	r7, r3, r1
.L161a6:
	cmp	r6, #1
	bhi	.L161ac
	mov	r6, #2
.L161ac:
	cmp	r6, #0x1e
	bls	.L161b2
	mov	r6, #0x1e
.L161b2:
	cmp	r7, #1
	bhi	.L161b8
	mov	r7, #2
.L161b8:
	cmp	r7, #0x1e
	bls	.L161be
	mov	r7, #0x1e
.L161be:
	mov	r2, r6
	mov	r1, r8
	mov	r3, r7
	str	r4, [sp]
	bl	Func_801e260
	mov	r2, #0
	ldr	r4, [sp]
	cmp	r2, r7
	bcs	.L16208
	ldr	r0, =0xea5
	mov	r3, #0x20
	sub	r3, r6
	add	r0, r10
	lsl	r1, r3, #1
.L161dc:
	ldrb	r3, [r0]
	cmp	r3, #0
	beq	.L161f0
	mov	r4, r8
	add	r3, r4, r2
	ldr	r4, =0xf07f
	cmp	r3, #0x10
	bhi	.L161f0
	mov	r4, #0xf0
	lsl	r4, #8
.L161f0:
	mov	r3, #0
	cmp	r3, r6
	bcs	.L16200
.L161f6:
	add	r3, #1
	strh	r4, [r5]
	add	r5, #2
	cmp	r3, r6
	bcc	.L161f6
.L16200:
	add	r2, #1
	add	r5, r1
	cmp	r2, r7
	bcc	.L161dc
.L16208:
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r10
	strb	r3, [r2]
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end ClearUIRegion

@ RedrawWindowContents
@ r0 = window record. Clears the pending count at +0x1A, then repaints from the
@ record's own geometry (+0x0C, +0x0E, +0x08, +0x0A).
@ Flag bit 3 of +0x16 selects a background pass through Func_170f8; flag bit 5
@ then chooses whether the 0xF00-byte text scratch at 0x6002500 is filled with
@ 0x44444444 (opaque colour 4) or cleared -- the same two fills Func_16738 and
@ Func_1671c provide as standalone helpers.
.thumb_func_start Func_8016230  @ 0x08016230
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r10, r3
	ldrh	r3, [r0, #0xa]
	ldrh	r2, [r0, #0x16]
	mov	r8, r3
	mov	r3, #0
	strh	r3, [r0, #0x1a]
	mov	r3, #8
	and	r3, r2
	sub	sp, #4
	ldrh	r5, [r0, #0xc]
	ldrh	r6, [r0, #0xe]
	ldrh	r7, [r0, #8]
	cmp	r3, #0
	beq	.L1629c
	mov	r3, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.L1627c
	mov	r0, r5
	mov	r1, r6
	mov	r2, r7
	mov	r3, r8
	bl	Func_80170f8
	mov	r1, #0xf0
	ldr	r3, =Func_80008d8
	ldr	r0, =0x6002500
	lsl	r1, #4
	ldr	r2, =0x44444444
	bl	_call_via_r3
	b	.L1628a
.L1627c:
	mov	r1, #0xf0
	ldr	r3, =Func_80008d8
	ldr	r0, =0x6002500
	lsl	r1, #4
	mov	r2, #0
	bl	_call_via_r3
.L1628a:
	mov	r3, #0
	str	r3, [sp]
	mov	r0, r5
	mov	r1, r6
	mov	r2, r7
	mov	r3, r8
	bl	Func_8017248
	b	.L162a8
.L1629c:
	mov	r0, r5
	mov	r1, r6
	mov	r2, r7
	mov	r3, r8
	bl	Func_80170f8
.L162a8:
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r10
	strb	r3, [r2]
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8016230

@ OpenWindow
@ r0 = x column, r1 = y row, r2 = width, r3 = height, arg5 on the stack = flags.
@ All geometry in TILES. Returns the window record, or 0 when all eight slots
@ are taken.
@ Scans the pool at [iwram_1e8c]+0x500 (8 records, stride 0x24) for a slot that
@ is free by .gcc2_compiled.'s test -- bit 0 of +0x16 clear AND the signed halfword
@ at +0x1A zero -- then fills it in:
@     +0x0C,+0x0E = x, y     +0x08,+0x0A = width, height
@     +0x00,+0x04 cleared    +0x10 = 1     +0x14 = 0     +0x16 = 1 | flags
@ Func_173ac resets the global text style so the new window starts clean, then
@ selected flag bits are copied into +0x16 and Func_16230 paints it.
@ The standard message box is CreateUIBox(0, 0xF, 0x1E, 6, ...).
.thumb_func_start CreateUIBox  @ 0x080162d4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r14, r3
	ldr	r3, =iwram_3001e8c
	mov	r12, r2
	ldr	r3, [r3]
	mov	r2, #0xa0
	lsl	r2, #3
	add	r4, r3, r2
	ldrh	r2, [r4, #0x16]
	mov	r3, #1
	and	r3, r2
	mov	r7, r1
	ldr	r6, [sp, #0x14]
	mov	r5, #0
	mov	r1, #0
	b	.L1630a

	.pool_aligned

.L162fc:
	add	r1, #1
	add	r4, #0x24
	cmp	r1, #8
	beq	.L16318
	ldrh	r2, [r4, #0x16]
	mov	r3, #1
	and	r3, r2
.L1630a:
	cmp	r3, #0
	bne	.L162fc
	mov	r2, #0x1a
	ldrsh	r3, [r4, r2]
	cmp	r3, #0
	bne	.L162fc
	mov	r5, r4
.L16318:
	cmp	r5, #0
	beq	.L163e0
	mov	r3, #0
	mov	r8, r3
	mov	r2, r12
	mov	r3, r14
	strh	r7, [r5, #0xe]
	strh	r2, [r5, #8]
	strh	r3, [r5, #0xa]
	mov	r2, r8
	mov	r3, r8
	mov	r7, #1
	strh	r0, [r5, #0xc]
	strh	r3, [r5, #0x14]
	str	r2, [r5]
	str	r4, [r5, #4]
	strh	r7, [r5, #0x10]
	strh	r7, [r5, #0x16]
	bl	Func_80173ac
	mov	r0, #8
	mov	r3, r6
	and	r3, r0
	cmp	r3, #0
	beq	.L16352
	ldrh	r3, [r5, #0x16]
	ldr	r2, =8
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16352:
	mov	r3, #0x20
	and	r3, r6
	cmp	r3, #0
	beq	.L16362
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x20
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16362:
	mov	r3, #0x40
	and	r3, r6
	cmp	r3, #0
	beq	.L16380
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x40
	orr	r3, r2
	strh	r3, [r5, #0x16]
	b	.L16380

	.pool_aligned

.L16380:
	mov	r3, #0x80
	and	r3, r6
	cmp	r3, #0
	beq	.L16390
	ldrh	r3, [r5, #0x16]
	ldr	r2, =0x80
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L16390:
	mov	r1, #0x80
	lsl	r1, #1
	mov	r3, r6
	and	r3, r1
	cmp	r3, #0
	beq	.L163a4
	ldrh	r2, [r5, #0x16]
	mov	r3, r1
	orr	r3, r2
	strh	r3, [r5, #0x16]
.L163a4:
	mov	r3, #2
	and	r3, r6
	cmp	r3, #0
	beq	.L163ce
	ldrh	r3, [r5, #0x16]
	ldr	r2, =2
	orr	r3, r2
	mov	r2, r8
	strh	r3, [r5, #0x16]
	strh	r2, [r5, #0x18]
	b	.L163c4

	.pool_aligned

.L163c4:
	strh	r7, [r5, #0x1a]
	mov	r0, r5
	bl	Func_8016230
	b	.L163e0
.L163ce:
	mov	r3, #7
	strh	r0, [r5, #0x1a]
	strh	r3, [r5, #0x18]
	mov	r0, r5
	bl	UIBox_WaitAnim
	mov	r0, #1
	bl	WaitFrames
.L163e0:
	mov	r0, r5
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end CreateUIBox

