	.include "macros.inc"
	.include "gba.inc"

@ AllocNode
@ Takes no arguments. Pops the head off the free list of display nodes and
@ returns it (0 when exhausted). The list head is [iwram_1e8c]+0xD98 and the
@ tail cache is +0xD9C; popping the last node moves the tail back to the head
@ slot. The popped node's link word is cleared.
.thumb_func_start Func_15e8c
	push	{lr}
	ldr	r3, =iwram_1e8c
	ldr	r2, =0xd98
	ldr	r3, [r3]
	add	r1, r3, r2
	ldr	r0, [r1]
	cmp	r0, #0
	beq	.L15eae
	ldr	r2, [r0]
	cmp	r2, #0
	bne	.L15ea8
	ldr	r4, =0xd9c
	add	r3, r4
	str	r1, [r3]
.L15ea8:
	mov	r3, #0
	str	r2, [r1]
	str	r3, [r0]
.L15eae:
	pop	{r1}
	bx	r1
.func_end Func_15e8c

@ FreeNode
@ r0 = node. Pushes a node back onto the free list, appending at the tail
@ cached in [iwram_1e8c]+0xD9C.
@ BOUNDS-CHECKED: the node is only accepted when it lies inside the pool,
@ [iwram_1e8c]+0x698 up to +0xD98. Anything outside is silently ignored, so
@ callers may pass statically allocated records without corrupting the list.
.thumb_func_start Func_15ec0
	push	{lr}
	ldr	r3, =iwram_1e8c
	mov	r1, #0xd3
	ldr	r2, [r3]
	lsl	r1, #3
	add	r3, r2, r1
	cmp	r0, r3
	bcc	.L15ee6
	ldr	r1, =0xd98
	add	r3, r2, r1
	cmp	r0, r3
	bcs	.L15ee6
	add	r1, #4
	add	r3, r2, r1
	ldr	r2, [r3]
	str	r0, [r3]
	mov	r3, #0
	str	r0, [r2]
	str	r3, [r0]
.L15ee6:
	pop	{r0}
	bx	r0
.func_end Func_15ec0

@ InitNodePool
@ Takes no arguments. Builds the display-node free list: 64 nodes of 0x1C bytes
@ running from [iwram_1e8c]+0x698 to +0xD98, each linked to the next, head
@ stored at +0xD98 and tail at +0xD9C. The last node's link is cleared.
@ 64 * 0x1C = 0x700 = 0xD98 - 0x698, so the pool exactly fills its region.
.thumb_func_start Func_15ef4
	push	{lr}
	ldr	r3, =iwram_1e8c
	mov	r1, #0xd3
	ldr	r0, [r3]
	lsl	r1, #3
	add	r2, r0, r1
	ldr	r1, =0xd98
	add	r3, r0, r1
	str	r2, [r3]
	mov	r3, #0x3e
.L15f08:
	mov	r1, r2
	add	r1, #0x1c
	sub	r3, #1
	str	r1, [r2]
	mov	r2, r1
	cmp	r3, #0
	bge	.L15f08
	ldr	r2, =0xd9c
	mov	r3, #0
	str	r3, [r1]
	add	r3, r0, r2
	str	r1, [r3]
	pop	{r0}
	bx	r0
.func_end Func_15ef4

@ InitUiSystem
@ Takes no arguments. Brings up the whole UI layer:
@     Func_48f4(0xF, 0x12FC) allocates the UI block -- this is what iwram_1e8c
@       points at, and 0x12FC is its size, so every +0xNNN offset in this module
@       is bounded by it
@     the block is DMA-cleared, then defaults are written:
@       +0xEA3 = 1 (dirty mask, see Func_160fc), +0xEA7 = 0x0F, +0x12B6 = 0x63
@     the first 0x140 bytes are filled with 0xF000F000, the empty-tile pattern
@     Func_15ef4 builds the node free list
@     Func_19d0c initialises the menu layer
@     Func_41d8 registers Func_160fc as the per-frame flush at priority 0x480
@     Func_173f4 finishes setup
@ Func_16018 below is the same sequence with one extra field and a different
@ finisher; use this one for the plain case.
.thumb_func_start Func_15f30
	push	{r5, lr}
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	Func_48f4
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
	bl	Func_15ef4
	bl	Func_19d0c
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_160fc
	bl	Func_41d8
	bl	Func_173f4
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_15f30

@ CopyTileHalf
@ r0 = source tile index, r1 = destination tile index (both masked to 0x3FF).
@ DMA3-copies 0x10 bytes -- half a 4bpp tile, four pixel rows -- from
@ 0x6000010 + src*0x20 to 0x6000000 + dst*0x20, then clears 0x14 bytes at
@ 0x600000C + dst*0x20 with Func_8d4.
@ The source is read from +0x10 within its tile and the destination written
@ from +0x00, so this shifts the copied rows upward by four as it goes.
@ Called by Func_16018 with (0xF013, 0x80); note 0xF013 is a text control code
@ and only its low 10 bits survive the mask.
.thumb_func_start Func_15fb8
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
	ldr	r3, =Func_8d4
	mov	r1, #0x14
	bl	_call_via_r3
	add	sp, #4
	pop	{r3}
	mov	r9, r3
	pop	{r1}
	bx	r1
.func_end Func_15fb8

@ InitUiSystemWithMode
@ r0 = mode, passed on to Func_17464.
@ The same bring-up as Func_15f30 -- same 0x12FC allocation under tag 0xF, same
@ clear, same defaults, same node pool, same Func_160fc registration -- with two
@ differences: it also sets +0xEA5 = 1, and it finishes with Func_17464(mode)
@ instead of Func_173f4, then seeds a tile through Func_15fb8(0xF013, 0x80).
@ It does NOT call Func_19d0c, so the menu layer is left uninitialised.
.thumb_func_start Func_16018
	push	{r5, r6, lr}
	mov	r6, r9
	push	{r6}
	mov	r6, r0
	ldr	r1, =0x12fc
	mov	r0, #0xf
	sub	sp, #4
	bl	Func_48f4
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
	bl	Func_15ef4
	mov	r1, #0x90
	lsl	r1, #3
	ldr	r0, =Func_160fc
	bl	Func_41d8
	mov	r0, r6
	bl	Func_17464
	add	r1, sp, #4
	mov	r9, r1
	ldr	r0, =0xf013
	mov	r1, #0x80
	bl	Func_15fb8
	add	r2, sp, #4
	mov	r9, r2
	mov	r1, #0x81
	ldr	r0, =0xf014
	bl	Func_15fb8
	add	r3, sp, #4
	mov	r9, r3
	mov	r1, #0x82
	ldr	r0, =0xf015
	bl	Func_15fb8
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
.func_end Func_16018

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
.thumb_func_start Func_160fc
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1e8c
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
.func_end Func_160fc

@ RestoreTilemapRect
@ r0 = x column, r1 = y row, r2 = width, r3 = height, all in TILES.
@ Writes the saved background back over a rectangle of the 30x20 tilemap, which
@ is how a window erases itself when it closes.
@ The tilemap index is (row * 32 + column) * 2, so the map stride is 32 entries
@ even though only 30 are visible.
@ Everything is clamped before use: width and height to 2..0x1E, and the height
@ is further trimmed so row + height never exceeds 0x14 (20 rows). Callers can
@ therefore pass sloppy geometry without running off the map.
.thumb_func_start Func_16178
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r7, r3
	ldr	r3, =iwram_1e8c
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
	bl	Func_1e260
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
.func_end Func_16178

@ RedrawWindowContents
@ r0 = window record. Clears the pending count at +0x1A, then repaints from the
@ record's own geometry (+0x0C, +0x0E, +0x08, +0x0A).
@ Flag bit 3 of +0x16 selects a background pass through Func_170f8; flag bit 5
@ then chooses whether the 0xF00-byte text scratch at 0x6002500 is filled with
@ 0x44444444 (opaque colour 4) or cleared -- the same two fills Func_16738 and
@ Func_1671c provide as standalone helpers.
.thumb_func_start Func_16230
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1e8c
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
	bl	Func_170f8
	mov	r1, #0xf0
	ldr	r3, =Func_8d8
	ldr	r0, =0x6002500
	lsl	r1, #4
	ldr	r2, =0x44444444
	bl	_call_via_r3
	b	.L1628a
.L1627c:
	mov	r1, #0xf0
	ldr	r3, =Func_8d8
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
	bl	Func_17248
	b	.L162a8
.L1629c:
	mov	r0, r5
	mov	r1, r6
	mov	r2, r7
	mov	r3, r8
	bl	Func_170f8
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
.func_end Func_16230

@ OpenWindow
@ r0 = x column, r1 = y row, r2 = width, r3 = height, arg5 on the stack = flags.
@ All geometry in TILES. Returns the window record, or 0 when all eight slots
@ are taken.
@ Scans the pool at [iwram_1e8c]+0x500 (8 records, stride 0x24) for a slot that
@ is free by Func_17394's test -- bit 0 of +0x16 clear AND the signed halfword
@ at +0x1A zero -- then fills it in:
@     +0x0C,+0x0E = x, y     +0x08,+0x0A = width, height
@     +0x00,+0x04 cleared    +0x10 = 1     +0x14 = 0     +0x16 = 1 | flags
@ Func_173ac resets the global text style so the new window starts clean, then
@ selected flag bits are copied into +0x16 and Func_16230 paints it.
@ The standard message box is Func_162d4(0, 0xF, 0x1E, 6, ...).
.thumb_func_start Func_162d4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r14, r3
	ldr	r3, =iwram_1e8c
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
	bl	Func_173ac
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
	bl	Func_16230
	b	.L163e0
.L163ce:
	mov	r3, #7
	strh	r0, [r5, #0x1a]
	strh	r3, [r5, #0x18]
	mov	r0, r5
	bl	Func_163ec
	mov	r0, #1
	bl	Func_30f8
.L163e0:
	mov	r0, r5
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_162d4

@ WaitForWindowIdle
@ r0 = window record. Spins on Func_30f8(1) until the pending count at +0x1A
@ reaches 0, so the caller can be sure queued text has finished drawing.
@ Returns immediately when flag bit 1 of +0x16 is set, which marks a window that
@ never queues work.
.thumb_func_start Func_163ec
	push	{r5, lr}
	mov	r5, r0
	ldrh	r2, [r5, #0x16]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	bne	.L16410
	mov	r2, #0x1a
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	beq	.L16410
.L16402:
	mov	r0, #1
	bl	Func_30f8
	mov	r2, #0x1a
	ldrsh	r3, [r5, r2]
	cmp	r3, #0
	bne	.L16402
.L16410:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_163ec

@ CloseWindow
@ r0 = window record, r1 = non-zero to also erase and wipe.
@ Always: Func_16478 releases the window's resources, the geometry is SAVED to
@ +0x1C..+0x22 in the order x, y, w, h, and the flags at +0x16 are cleared --
@ which alone is enough to return the slot to Func_162d4's scan.
@ When r1 is non-zero it additionally calls Func_16178 to restore the tilemap
@ underneath and then zeroes the entire record. Pass 0 to keep the pixels on
@ screen while freeing the slot.
.thumb_func_start Func_16418
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r7, r1
	cmp	r5, #0
	beq	.L16472
	bl	Func_16478
	ldrh	r3, [r5, #0xc]
	strh	r3, [r5, #0x1c]
	ldrh	r3, [r5, #0xe]
	strh	r3, [r5, #0x1e]
	ldrh	r3, [r5, #8]
	strh	r3, [r5, #0x20]
	ldrh	r3, [r5, #0xa]
	mov	r6, #0
	strh	r6, [r5, #0x16]
	strh	r3, [r5, #0x22]
	cmp	r7, #0
	beq	.L1646c
	ldrh	r0, [r5, #0xc]
	ldrh	r1, [r5, #0xe]
	ldrh	r2, [r5, #8]
	ldrh	r3, [r5, #0xa]
	bl	Func_16178
	str	r6, [r5]
	str	r6, [r5, #4]
	strh	r6, [r5, #8]
	strh	r6, [r5, #0xa]
	strh	r6, [r5, #0xc]
	strh	r6, [r5, #0xe]
	strh	r6, [r5, #0x10]
	strh	r6, [r5, #0x12]
	strh	r6, [r5, #0x14]
	strh	r6, [r5, #0x16]
	strh	r6, [r5, #0x18]
	strh	r6, [r5, #0x1a]
	strh	r6, [r5, #0x1c]
	strh	r6, [r5, #0x1e]
	strh	r6, [r5, #0x20]
	strh	r6, [r5, #0x22]
	b	.L16472
.L1646c:
	mov	r3, #4
	strh	r7, [r5, #0x18]
	strh	r3, [r5, #0x1a]
.L16472:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_16418

@ ReleaseWindowResources
@ r0 = window record. Calls Func_16498 then Func_164ac -- the tilemap side and
@ the node/OBJ side of tearing a window down. Split out because Func_16418 needs
@ both and other paths need only one.
.thumb_func_start Func_16478
	push	{r5, lr}
	mov	r5, r0
	ldrh	r2, [r5, #0x16]
	mov	r3, #8
	and	r3, r2
	cmp	r3, #0
	bne	.L16490
	bl	Func_16498
	mov	r0, r5
	bl	Func_164ac
.L16490:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_16478

@ ReleaseWindowTiles
@ r0 = window record. Hands the window's tile region back through Func_170f8
@ using its saved geometry.
.thumb_func_start Func_16498
	push	{lr}
	ldrh	r4, [r0, #0xc]
	ldrh	r1, [r0, #0xe]
	ldrh	r2, [r0, #8]
	ldrh	r3, [r0, #0xa]
	mov	r0, r4
	bl	Func_170f8
	pop	{r0}
	bx	r0
.func_end Func_16498

@ ReleaseWindowNodes
@ r0 = window record. Walks the window's node list and returns each node with
@ Func_16594, then clears the list head.
.thumb_func_start Func_164ac
	push	{r5, lr}
	mov	r3, r0
	mov	r5, #0
	cmp	r3, #0
	beq	.L164cc
	ldr	r0, [r3]
	str	r3, [r3, #4]
	str	r5, [r3]
	cmp	r0, #0
	beq	.L164cc
.L164c0:
	ldr	r5, [r0]
	bl	Func_16594
	mov	r0, r5
	cmp	r0, #0
	bne	.L164c0
.L164cc:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_164ac

@ MarkTileRectDirty
@ r0 = window record, r1 = left, r2 = top, r3 = right, arg5 = bottom, all in
@ PIXELS. Converts to tiles by adding 7 and shifting right 3 -- a round-up to
@ whole tiles -- then offsets by the window's own origin (+0x0C, +0x0E) and
@ marks that tile rectangle for the next flush.
@ Callers that have drawn text at pixel coordinates use this to tell
@ Func_160fc which rows changed.
.thumb_func_start Func_164d4
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #4
	mov	r7, r2
	ldr	r2, =iwram_1e8c
	mov	r5, r1
	ldr	r1, [sp, #0x18]
	ldr	r2, [r2]
	lsr	r4, r5, #3
	add	r3, #7
	ldrh	r5, [r0, #0xc]
	add	r1, #7
	ldrh	r0, [r0, #0xe]
	mov	r8, r2
	lsr	r3, #3
	lsr	r2, r7, #3
	lsr	r1, #3
	add	r2, r0
	add	r4, r5
	add	r3, r5
	add	r1, r0
	add	r5, r4, #1
	add	r7, r2, #1
	sub	r6, r3, r4
	sub	r4, r1, r2
	mov	r3, r4
	mov	r1, r7
	mov	r2, r6
	mov	r0, r5
	str	r4, [sp]
	bl	Func_1e260
	lsl	r3, r7, #5
	add	r3, r5
	ldr	r4, [sp]
	lsl	r3, #1
	mov	r2, r8
	mov	r1, #0
	add	r0, r3, r2
	cmp	r1, r4
	bcs	.L16548
	mov	r3, #0x20
	sub	r3, r6
	lsl	r3, #1
.L1652e:
	mov	r5, #0
	cmp	r5, r6
	bcs	.L16540
	ldr	r2, .L16554	@ 0xf020
.L16536:
	add	r5, #1
	strh	r2, [r0]
	add	r0, #2
	cmp	r5, r6
	bcc	.L16536
.L16540:
	add	r1, #1
	add	r0, r3
	cmp	r1, r4
	bcc	.L1652e
.L16548:
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r8
	strb	r3, [r2]
	add	sp, #4
	b	.L16560

	.align	2, 0
.L16554:
	.word	0xf020
	.pool

.L16560:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_164d4

@ CacheListTail
@ r0 = list head. Walks the singly-linked list to its last node and stores that
@ node at [r0+4], so later appends are O(1). Used with Func_16584.
.thumb_func_start Func_1656c
	push	{lr}
	ldr	r3, [r0]
	mov	r2, r0
	cmp	r3, #0
	beq	.L1657e
.L16576:
	mov	r2, r3
	ldr	r3, [r2]
	cmp	r3, #0
	bne	.L16576
.L1657e:
	str	r2, [r0, #4]
	pop	{r0}
	bx	r0
.func_end Func_1656c

@ AppendNode
@ r0 = list, r1 = node. Links the node after the cached tail at [r0+4] and
@ updates the cache. Does nothing when r0 is null.
.thumb_func_start Func_16584
	push	{lr}
	cmp	r0, #0
	beq	.L16590
	ldr	r3, [r0, #4]
	str	r1, [r3]
	str	r1, [r0, #4]
.L16590:
	pop	{r0}
	bx	r0
.func_end Func_16584

@ FreeDisplayNode
@ r0 = node. Releases the node's OBJ tile allocation with Func_3f3c when it
@ holds one, then returns the node itself to the free list with Func_15ec0.
.thumb_func_start Func_16594
	push	{r5, lr}
	mov	r5, r0
	bl	Func_15ec0
	ldrb	r3, [r5, #4]
	cmp	r3, #0
	beq	.L165c0
	ldrb	r0, [r5, #0xe]
	bl	Func_3f3c
	ldrb	r3, [r5, #4]
	cmp	r3, #2
	bne	.L165c0
	ldr	r3, =iwram_1e8c
	ldr	r1, [r3]
	ldrb	r3, [r5, #0x19]
	ldr	r2, =0x12d0
	lsr	r3, #4
	lsl	r3, #1
	add	r3, r2
	ldr	r2, .L165cc	@ 0x3e7
	strh	r2, [r1, r3]
.L165c0:
	mov	r3, #0
	strb	r3, [r5, #5]
	pop	{r5}
	pop	{r0}
	bx	r0

	.align	2, 0
.L165cc:
	.word	0x3e7
.func_end Func_16594

@ OpenMessageBox
@ r0..r3 and arg5 are the box parameters. Scans the THREE message-box slots at
@ [iwram_1e8c]+0x620 (stride 0x28) for one whose first word is 0 -- an unused
@ slot -- and claims it. Returns 0 when all three are busy.
@ These slots sit immediately after the eight window records at +0x500, since
@ 8 * 0x24 = 0x120 and 0x500 + 0x120 = 0x620.
.thumb_func_start Func_165d8
	push	{r5, r6, r7, lr}
	mov	r7, r3
	ldr	r3, =iwram_1e8c
	mov	r12, r1
	ldr	r3, [r3]
	mov	r1, #0xc4
	lsl	r1, #3
	add	r4, r3, r1
	ldr	r3, [r4]
	mov	r6, r0
	ldr	r5, [sp, #0x10]
	mov	r0, #0
	mov	r1, #0
	b	.L165fe
.L165f4:
	add	r1, #1
	add	r4, #0x28
	cmp	r1, #3
	beq	.L16604
	ldr	r3, [r4]
.L165fe:
	cmp	r3, #0
	bne	.L165f4
	mov	r0, r4
.L16604:
	cmp	r0, #0
	beq	.L16668
	lsl	r3, r2, #8
	strh	r3, [r0, #0x1e]
	strh	r3, [r0, #4]
	lsl	r3, r7, #8
	strh	r3, [r0, #6]
	mov	r3, r12
	strh	r3, [r0, #0x12]
	mov	r3, #0xf
	strh	r3, [r0, #0x16]
	mov	r3, #0xa
	strh	r3, [r0, #0x1a]
	ldr	r3, [sp, #0x14]
	mov	r2, #0
	str	r6, [r0]
	strh	r2, [r0, #0x14]
	strh	r2, [r0, #0x18]
	strh	r2, [r0, #0x20]
	strh	r3, [r0, #0x24]
	cmp	r5, #0
	beq	.L1664c
	mov	r2, r0
	mov	r1, #0
	add	r2, #8
.L16636:
	ldrh	r3, [r5]
	add	r1, #1
	strh	r3, [r2]
	add	r5, #2
	add	r2, #2
	cmp	r1, #3
	bls	.L16636
	b	.L16664

	.pool_aligned

.L1664c:
	mov	r3, r0
	ldr	r2, =0
	mov	r1, #0
	add	r3, #8
.L16654:
	add	r1, #1
	strh	r2, [r3]
	add	r3, #2
	cmp	r1, #3
	bls	.L16654
	b	.L16664

	.pool_aligned

.L16664:
	mov	r3, #0
	strh	r3, [r0, #0x10]
.L16668:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_165d8

@ CloseMessageBoxes
@ r0, r1 = teardown parameters. Walks the three message-box slots looking for
@ the first that is either empty or already finished (its record's +0x14 is
@ zero), and tears down from there with Func_167d8.
.thumb_func_start Func_16670
	push	{r5, r6, lr}
	ldr	r3, =iwram_1e8c
	mov	r4, #0xc4
	ldr	r3, [r3]
	lsl	r4, #3
	mov	r6, r1
	mov	r5, #0
	add	r1, r3, r4
	mov	r4, #0
	b	.L16688
.L16684:
	add	r1, #0x28
	add	r4, #1
.L16688:
	cmp	r4, #3
	beq	.L1669a
	ldr	r3, [r1]
	cmp	r3, #0
	beq	.L16698
	ldrh	r3, [r3, #0x14]
	cmp	r3, #0
	beq	.L16684
.L16698:
	mov	r5, r1
.L1669a:
	cmp	r5, #0
	beq	.L1670e
	ldr	r3, [r5]
	cmp	r3, #0
	bne	.L166b2
	mov	r3, #0xa0
	lsl	r3, #4
	strh	r3, [r5, #6]
	mov	r3, #0xc0
	lsl	r3, #2
	str	r0, [r5]
	b	.L166de
.L166b2:
	cmp	r2, #0
	bne	.L166e0
	ldrh	r3, [r5, #6]
	mov	r2, r3
	cmp	r2, #0
	bne	.L166c6
	mov	r3, #0xa0
	lsl	r3, #4
	strh	r3, [r5, #6]
	b	.L166da
.L166c6:
	mov	r1, #0xd0
	lsl	r1, #4
	cmp	r2, r1
	bcs	.L166d4
	add	r3, r1
	strh	r3, [r5, #6]
	b	.L166da
.L166d4:
	mov	r0, r5
	bl	Func_167d8
.L166da:
	mov	r3, #0xc0
	lsl	r3, #2
.L166de:
	strh	r3, [r5, #4]
.L166e0:
	mov	r3, #0xc0
	lsl	r3, #2
	strh	r3, [r5, #0x1e]
	ldr	r3, [r5]
	mov	r2, #0
	strh	r2, [r3, #0x14]
	mov	r3, #0xf
	strh	r3, [r5, #0x16]
	mov	r3, #0xa
	strh	r3, [r5, #0x1a]
	strh	r6, [r5, #0x12]
	mov	r3, r5
	strh	r2, [r5, #0x14]
	strh	r2, [r5, #0x18]
	strh	r2, [r5, #0x10]
	strh	r2, [r5, #0x20]
	mov	r4, #0
	add	r3, #8
.L16704:
	add	r4, #1
	strh	r2, [r3]
	add	r3, #2
	cmp	r4, #3
	bls	.L16704
.L1670e:
	mov	r0, r5
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_16670

@ ClearTextBuffer
@ Takes no arguments. Fills the 0xF00-byte text scratch at 0x6002500 with 0
@ (transparent). Func_16738 below is the same fill with colour 4.
.thumb_func_start Func_1671c
	push	{lr}
	mov	r1, #0xf0
	ldr	r3, =Func_8d8
	lsl	r1, #4
	mov	r2, #0
	ldr	r0, =0x6002500
	bl	_call_via_r3
	pop	{r1}
	bx	r1
.func_end Func_1671c

@ FillTextBuffer
@ Takes no arguments. Fills the 0xF00-byte text scratch at 0x6002500 with
@ 0x44444444 -- palette index 4 in every 4bpp pixel. The opaque counterpart to
@ Func_1671c.
.thumb_func_start Func_16738
	push	{lr}
	mov	r1, #0xf0
	ldr	r3, =Func_8d8
	lsl	r1, #4
	ldr	r2, =0x44444444
	ldr	r0, =0x6002500
	bl	_call_via_r3
	pop	{r1}
	bx	r1
.func_end Func_16738

@ FindActiveMessageBox
@ Takes no arguments. Returns the first of the three message-box slots that is
@ empty or whose record has +0x14 == 0, or 0 if all three are still running.
@ Same scan as Func_17364, which reduces the result to a yes/no.
.thumb_func_start Func_16758
	push	{r5, lr}
	ldr	r3, =iwram_1e8c
	mov	r1, #0xc4
	ldr	r3, [r3]
	lsl	r1, #3
	add	r2, r3, r1
	mov	r5, #0
	mov	r1, #0
	b	.L1676e
.L1676a:
	add	r2, #0x28
	add	r1, #1
.L1676e:
	cmp	r1, #3
	beq	.L16780
	ldr	r3, [r2]
	cmp	r3, #0
	beq	.L1677e
	ldrh	r3, [r3, #0x14]
	cmp	r3, #0
	beq	.L1676a
.L1677e:
	mov	r5, r2
.L16780:
	cmp	r5, #0
	beq	.L167a2
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.L16792
	bl	Func_1671c
	mov	r3, #0
	strh	r3, [r5, #6]
.L16792:
	mov	r3, #0
	strh	r3, [r5, #4]
	strh	r3, [r5, #0x14]
	mov	r2, #0xf
	strh	r3, [r5, #0x18]
	mov	r3, #0xa
	strh	r2, [r5, #0x16]
	strh	r3, [r5, #0x1a]
.L167a2:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_16758

@ ApplyStyleFromRecord
@ r0 = record. Copies three saved style values out of the record into the
@ global text-style fields:
@     [r0+0x16] -> +0xEAE      [r0+0x18] -> +0xEAC      [r0+0x1A] -> +0xEA8
@ These are exactly the three fields Func_173ac resets to 0x0F, 0 and 0x0A, so
@ this is "restore the style this record was created with".
.thumb_func_start Func_167ac
	ldr	r3, =iwram_1e8c
	ldr	r4, =0xeae
	ldr	r2, [r3]
	ldrh	r1, [r0, #0x16]
	add	r3, r2, r4
	strh	r1, [r3]
	sub	r4, #2
	ldrh	r1, [r0, #0x18]
	add	r3, r2, r4
	strh	r1, [r3]
	ldr	r1, =0xea8
	ldrh	r3, [r0, #0x1a]
	add	r2, r1
	strh	r3, [r2]
	bx	lr
.func_end Func_167ac
