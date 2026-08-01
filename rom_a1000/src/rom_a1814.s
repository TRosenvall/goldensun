	.include "macros.inc"
	.include "gba.inc"

@ BuildPartyHeader
@ r0 = state block. Opens the party header window at (0, 0, 0xD, 5) into
@ state+0x10, attaches the menu cursor sprite through Func_a1778(-8, 0xB) and
@ stores it at state+0x14, starting it hidden (+0x05 = 0x0D). Also seeds
@ +0x1C = 0xFF and +0x1D = 0 -- no row and no column selected yet -- and sets
@ the cursor's +0x0F to 0xFE and the sprite at state+0x18's to 0xFF, which is
@ the OBJ sort order that keeps the cursor over the portraits.
.thumb_func_start Func_a1814
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r5, r0
	mov	r3, #0
	sub	sp, #8
	mov	r8, r3
	str	r3, [r5, #0x10]
	mov	r6, r5
	mov	r3, #5
	str	r3, [sp]
	add	r6, #0x10
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, r6
	mov	r3, #0xd
	mov	r1, #0
	mov	r2, #0
	bl	Func_a10d0
	ldr	r6, [r6]
	mov	r1, #8
	neg	r1, r1
	mov	r0, r6
	mov	r2, #0xb
	bl	Func_a1778
	mov	r3, #0xd
	strb	r3, [r0, #5]
	mov	r3, #0xff
	strb	r3, [r5, #0x1c]
	mov	r3, r8
	strb	r3, [r5, #0x1d]
	mov	r3, #0xfe
	str	r0, [r5, #0x14]
	strb	r3, [r0, #0xf]
	ldr	r2, [r5, #0x18]
	sub	r3, #0xff
	mov	r0, r6
	strb	r3, [r2, #0xf]
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_a1814

@ SpawnPartyActors
@ r0 = window, r1 = x offset, r2 = y offset, r3 = spacing.
@ Creates one animated actor per party member and lays them out along the top
@ of the screen. For each roster id from _Func_796c4 it remaps the resource
@ through _Func_8b398 (which is what makes the sprites follow story progress),
@ creates the actor with _Func_bc70, and files it in three parallel arrays:
@
@     state+0x114 + i*4   the actor pointer
@     state+0x134 + i*2   its x, from the window's tile column * 8 plus
@                         i * (spacing + 0x10)
@     state+0x144 + i*2   its y, from the window's tile row * 8 plus 0x10
@
@ It then sets +0x40 to 0x10000 (unit scale), clears bits 0 and 2 of the
@ actor's +0x09, zeroes +0x26 and starts animation 1 with _Func_ba30. Slots
@ past the party size are zeroed out to 8. Finally it registers Func_a19a0 at
@ sort key 0xC80 so the actors get drawn every frame.
@
@ The roster count also goes to state+0x1E.
.thumb_func_start Func_a1870
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x2c
	str	r1, [sp, #0xc]
	str	r2, [sp, #8]
	str	r3, [sp, #4]
	ldr	r3, =iwram_1f2c
	mov	r9, r0
	ldr	r3, [r3]
	mov	r0, sp
	add	r0, #0x10
	mov	r10, r3
	str	r0, [sp]
	bl	_Func_796c4
	lsl	r0, #16
	lsr	r0, #16
	mov	r8, r0
	mov	r1, r8
	mov	r2, r10
	mov	r5, #0
	strb	r1, [r2, #0x1e]
	cmp	r5, r8
	bge	.La191c
	mov	r7, #0x9a
	mov	r6, #0x8a
	lsl	r7, #1
	lsl	r6, #1
	add	r7, r10
	add	r6, r10
	mov	r11, r5
.La18b8:
	ldr	r1, [sp]
	mov	r3, r11
	ldrh	r0, [r3, r1]
	bl	_Func_8b398
	bl	_Func_bc70
	cmp	r0, #0
	beq	.La190e
	str	r0, [r6]
	mov	r2, r9
	ldrh	r3, [r2, #0xc]
	ldr	r2, [sp, #4]
	add	r2, #0x10
	mul	r2, r5
	ldr	r1, [sp, #0xc]
	add	r3, r1, r3
	lsl	r3, #3
	add	r3, r2
	strh	r3, [r7]
	mov	r2, r9
	ldr	r1, [sp, #8]
	ldrh	r3, [r2, #0xe]
	add	r3, r1, r3
	lsl	r3, #3
	add	r3, #0x10
	strh	r3, [r7, #0x10]
	mov	r3, #0x80
	lsl	r3, #9
	str	r3, [r6, #0x40]
	mov	r1, #0xd
	ldrb	r3, [r0, #9]
	neg	r1, r1
	mov	r2, r1
	and	r3, r2
	mov	r2, r0
	strb	r3, [r0, #9]
	add	r2, #0x26
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, #1
	bl	_Func_ba30
.La190e:
	mov	r2, #2
	add	r5, #1
	add	r7, #2
	add	r6, #4
	add	r11, r2
	cmp	r5, r8
	blt	.La18b8
.La191c:
	cmp	r5, #7
	bgt	.La1938
	lsl	r3, r5, #2
	mov	r0, #0x8a
	add	r3, r10
	lsl	r0, #1
	add	r2, r3, r0
	mov	r3, #8
	mov	r1, #0
	sub	r5, r3, r5
.La1930:
	sub	r5, #1
	stmia	r2!, {r1}
	cmp	r5, #0
	bne	.La1930
.La1938:
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_a19a0
	bl	Func_41d8
	add	sp, #0x2c
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1870

@ DestroyPartyActors
@ Takes no arguments. Walks the same state+0x114 array for as many entries as
@ _Func_796c4 reports, destroying each with _Func_bdd4, then unregisters
@ Func_a19a0. Null slots are skipped, so calling it twice is safe.
.thumb_func_start Func_a195c
	push	{r5, r6, lr}
	sub	sp, #0x1c
	ldr	r3, =iwram_1f2c
	mov	r0, sp
	ldr	r5, [r3]
	bl	_Func_796c4
	lsl	r0, #16
	lsr	r0, #16
	cmp	r0, #0
	beq	.La198a
	mov	r3, #0x8a
	lsl	r3, #1
	add	r6, r5, r3
	mov	r5, r0
.La197a:
	ldmia	r6!, {r0}
	cmp	r0, #0
	beq	.La1984
	bl	_Func_bdd4
.La1984:
	sub	r5, #1
	cmp	r5, #0
	bne	.La197a
.La198a:
	ldr	r0, =Func_a19a0
	bl	Func_4278
	add	sp, #0x1c
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_a195c

@ DrawPartyActors
@ The per-frame task Func_a1870 registers. For each of the _Func_795fc party
@ members it builds a 16.16 position from the halfwords at state+0x134 and
@ +0x144 -- y is inverted as 0x1E20000 minus the stored value, which is the
@ screen-to-world flip this engine uses everywhere -- clears bits 0 and 2 of
@ the actor's +0x09, and submits the sprite with _Func_b168 at scale
@ [actor+0x40].
.thumb_func_start Func_a19a0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1f2c
	sub	sp, #0x1c
	ldr	r5, [r3]
	bl	_Func_795fc
	lsl	r0, #16
	lsr	r0, #16
	mov	r10, r0
	mov	r4, #0
	cmp	r4, r10
	bge	.La1a2e
	add	r2, sp, #4
	mov	r8, r2
	mov	r3, #0x9a
	mov	r2, #0x8a
	lsl	r3, #1
	lsl	r2, #1
	add	r7, r5, r3
	add	r6, sp, #0xc
	add	r5, r2
.La19d0:
	mov	r2, #0x10
	ldrsh	r3, [r7, r2]
	ldr	r0, [r5]
	mov	r2, #0xf1
	lsl	r3, #16
	lsl	r2, #17
	sub	r1, r2, r3
	cmp	r0, #0
	beq	.La1a24
	ldrb	r3, [r0, #9]
	mov	r12, r3
	mov	r3, #0xd
	neg	r3, r3
	mov	r2, r3
	mov	r3, r12
	and	r3, r2
	strb	r3, [r0, #9]
	ldr	r3, [r5, #0x40]
	str	r3, [sp, #4]
	ldr	r3, [r5, #0x40]
	mov	r2, r8
	str	r3, [r2, #4]
	mov	r2, #0
	ldrsh	r3, [r7, r2]
	lsl	r3, #16
	str	r1, [r6, #4]
	str	r3, [r6]
	mov	r2, #0x10
	ldrsh	r3, [r7, r2]
	lsl	r3, #16
	add	r3, r1
	str	r3, [r6, #8]
	mov	r3, #0
	str	r3, [r6, #0xc]
	mov	r3, #0x80
	mov	r1, r6
	mov	r2, r8
	lsl	r3, #7
	str	r4, [sp]
	bl	_Func_b168
	ldr	r4, [sp]
.La1a24:
	add	r4, #1
	add	r7, #2
	add	r5, #4
	cmp	r4, r10
	blt	.La19d0
.La1a2e:
	add	sp, #0x1c
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a19a0

@ PlaceCursor
@ r0 = x offset in pixels, r1 = y offset. Positions the cursor sprite at
@ state+0x14 inside the window at state+0x10: the window's tile column and row
@ (+0x0C and +0x0E) are scaled by 8, the caller's offset added, and 8 more for
@ the border. A small per-axis wobble comes from the byte tables .Laf294 and
@ .Laf29d indexed by (iwram_1e40 >> 1) & 7 -- iwram_1e40 is the free-running
@ frame counter, so the cursor bobs on an eight-frame cycle.
@ The x also goes into the low 9 bits of +0x16, which is the OBJ attribute the
@ hardware reads.
.thumb_func_start Func_a1a40
	push	{r5, r6, lr}
	ldr	r3, =iwram_1f2c
	ldr	r5, [r3]
	ldr	r3, =iwram_1e40
	mov	r14, r3
	ldr	r3, [r3]
	mov	r6, #7
	lsr	r3, #1
	mov	r12, r6
	and	r3, r6
	ldr	r2, =Laf294
	ldr	r6, [r5, #0x10]
	ldrb	r2, [r2, r3]
	ldrh	r3, [r6, #0xc]
	add	r2, r0
	lsl	r3, #3
	ldr	r4, [r5, #0x14]
	add	r2, r3
	ldr	r5, .La1a9c	@ 0xffff
	add	r2, #8
	ldr	r3, .La1aa0	@ 0x1ff
	strh	r2, [r4, #6]
	and	r2, r5
	ldrh	r0, [r4, #0x16]
	and	r2, r3
	ldr	r3, =0xfffffe00
	and	r3, r0
	orr	r3, r2
	mov	r0, r14
	strh	r3, [r4, #0x16]
	ldr	r3, [r0]
	ldr	r2, =Laf29d
	mov	r0, r12
	lsr	r3, #1
	and	r3, r0
	ldrb	r3, [r2, r3]
	ldrh	r2, [r6, #0xe]
	add	r3, r1
	lsl	r2, #3
	add	r3, r2
	add	r3, #8
	strh	r3, [r4, #8]
	and	r3, r5
	strb	r3, [r4, #0x14]
	b	.La1ab8

	.align	2, 0
.La1a9c:
	.word	0xffff
.La1aa0:
	.word	0x1ff
	.pool

.La1ab8:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_a1a40

@ GlideCursorTo
@ r0 = target x, r1 = target y, both in the same offsets Func_a1a40 uses.
@ Moves the cursor there over two frames rather than snapping: the per-frame
@ step is (target*16 - current*16 + 1) / 2 in 1/16-pixel units, applied twice
@ with a Func_30f8(1) between. The 0x40 biases and the 0x38 subtraction cancel
@ out; they exist so the intermediate arithmetic stays positive.
@
@ When state+0x222 is non-zero it clears that flag and returns immediately --
@ that is the "snap, do not glide" request a screen raises when it has just
@ redrawn everything and an animation would look wrong.
.thumb_func_start Func_a1ac0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	ldr	r2, =0x222
	mov	r10, r3
	add	r2, r10
	ldrh	r3, [r2]
	mov	r8, r1
	mov	r1, #2
	sub	sp, #4
	mov	r9, r1
	cmp	r3, #0
	beq	.La1af4
	mov	r3, #0
	strh	r3, [r2]
	b	.La1bb6

	.pool_aligned

.La1af4:
	mov	r2, r10
	ldr	r7, [r2, #0x14]
	ldrh	r3, [r7, #0x16]
	lsl	r3, #23
	lsr	r3, #23
	add	r3, #0x40
	ldrb	r2, [r7, #0x14]
	strh	r3, [r7, #6]
	add	r2, #0x40
	strh	r2, [r7, #8]
	ldrh	r2, [r7, #6]
	mov	r3, #0x40
	add	r8, r3
	mov	r3, r2
	sub	r3, #8
	add	r0, #0x40
	cmp	r3, #0
	ble	.La1b1e
	ldr	r1, =0xfff8
	add	r3, r2, r1
	strh	r3, [r7, #6]
.La1b1e:
	ldrh	r6, [r7, #8]
	mov	r3, r6
	sub	r3, #8
	cmp	r3, #0
	ble	.La1b30
	ldr	r2, =0xfff8
	add	r3, r6, r2
	strh	r3, [r7, #8]
	ldrh	r6, [r7, #8]
.La1b30:
	ldrh	r5, [r7, #6]
	lsl	r0, #4
	lsl	r5, #4
	sub	r0, r5
	mov	r1, #2
	add	r0, #1
	bl	Func_af0_from_thumb
	mov	r3, r8
	mov	r11, r0
	lsl	r6, #4
	lsl	r0, r3, #4
	sub	r0, r6
	add	r0, #1
	mov	r1, #2
	bl	Func_af0_from_thumb
	ldr	r4, .La1b88	@ 0xffff
	mov	r8, r0
.La1b56:
	mov	r2, r10
	ldr	r0, [r2, #0x10]
	ldrh	r3, [r0, #0xc]
	add	r5, r11
	lsl	r3, #3
	asr	r1, r5, #4
	add	r1, r3
	sub	r1, #0x38
	ldr	r3, .La1b8c	@ 0x1ff
	strh	r1, [r7, #6]
	and	r1, r4
	and	r1, r3
	ldr	r2, .La1b90	@ 0xfffffe00
	ldrh	r3, [r7, #0x16]
	and	r3, r2
	orr	r3, r1
	strh	r3, [r7, #0x16]
	ldrh	r3, [r0, #0xe]
	add	r6, r8
	lsl	r3, #3
	asr	r2, r6, #4
	add	r2, r3
	mov	r3, #1
	b	.La1b98

	.align	2, 0
.La1b88:
	.word	0xffff
.La1b8c:
	.word	0x1ff
.La1b90:
	.word	0xfffffe00
	.pool

.La1b98:
	neg	r3, r3
	sub	r2, #0x38
	add	r9, r3
	strh	r2, [r7, #8]
	mov	r1, r9
	and	r2, r4
	strb	r2, [r7, #0x14]
	cmp	r1, #0
	beq	.La1bb6
	mov	r0, #1
	str	r4, [sp]
	bl	Func_30f8
	ldr	r4, [sp]
	b	.La1b56
.La1bb6:
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1ac0

@ NullHandler
@ An empty `bx lr`. Used where a table needs an entry that does nothing.
.thumb_func_start Func_a1bc8
	bx	lr
.func_end Func_a1bc8

@ LayOutGrid8
@ Takes no arguments. Func_a1bdc(0x6C, 0x28, 8) -- the eight-column grid the
@ item and Psynergy lists use.
.thumb_func_start Func_a1bcc
	push	{lr}
	mov	r0, #0x6c
	mov	r1, #0x28
	mov	r2, #8
	bl	Func_a1bdc
	pop	{r0}
	bx	r0
.func_end Func_a1bcc

@ LayOutGrid
@ r0 = x origin, r1 = y origin, r2 = columns. Walks all 32 sprite nodes at
@ state+0x48 and hands each live one to Func_a1c2c with its index, so they land
@ on a grid. Null slots are skipped, not compacted -- position follows the slot
@ index, not the occupied count.
.thumb_func_start Func_a1bdc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r5, r3
	add	r5, #0x48
	sub	sp, #4
	mov	r9, r0
	mov	r10, r1
	mov	r8, r2
	mov	r6, #0
	mov	r7, r5
.La1bfa:
	ldmia	r7!, {r3}
	cmp	r3, #0
	beq	.La1c10
	mov	r3, r8
	str	r3, [sp]
	mov	r0, r5
	mov	r1, r6
	mov	r2, r9
	mov	r3, r10
	bl	Func_a1c2c
.La1c10:
	add	r6, #1
	add	r5, #4
	cmp	r6, #0x1f
	ble	.La1bfa
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1bdc

@ PlaceGridNode
@ r0 = node array, r1 = index (wrapped to 0 above 31), r2 = x origin,
@ r3 = y origin, arg5 = columns. Row is index / columns and column is
@ index % columns -- Func_af0 and Func_b1c on the same pair -- each scaled by
@ 16, so the cells are 16 by 16. Finishes with Func_a17c4 to rewind the sprite.
.thumb_func_start Func_a1c2c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r1
	mov	r8, r2
	mov	r7, r3
	cmp	r6, #0x1f
	ble	.La1c3e
	mov	r6, #0
.La1c3e:
	ldr	r1, [sp, #0x14]
	ldr	r5, [r0]
	mov	r0, r6
	bl	Func_af0_from_thumb
	lsl	r0, #4
	add	r0, r7
	strh	r0, [r5, #8]
	ldr	r1, [sp, #0x14]
	mov	r0, r6
	bl	Func_b1c_from_thumb
	lsl	r0, #4
	add	r0, r8
	strh	r0, [r5, #6]
	mov	r0, r5
	bl	Func_a17c4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1c2c

@ PlaceGridNodeWide
@ r0 = node array, r1 = index (wrapped above 15), r2 = x origin, r3 = y origin,
@ arg5 = columns. As Func_a1c2c but the columns are 24 pixels apart rather than
@ 16 -- the `(q*2 + q) << 3` is a multiply by 24, not a shift.
@
@ NOTE: this one is declared `.thumb_func_Start` with a capital S. GAS accepts
@ it, so it assembles identically, but a case-sensitive scan of the sources will
@ miss the function entirely. Func_942e0 in rom_8a000 has the same typo.
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
	bl	Func_af0_from_thumb
	lsl	r0, #4
	add	r0, r7
	strh	r0, [r5, #8]
	ldr	r1, [sp, #0x14]
	mov	r0, r6
	bl	Func_b1c_from_thumb
	lsl	r3, r0, #1
	add	r3, r0
	lsl	r3, #3
	add	r3, r8
	strh	r3, [r5, #6]
	mov	r0, r5
	bl	Func_a17c4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a1c6c

@ LayOutGridWide
@ r0 = 1 to start at row 0x38, anything else 0x28. Lays the first 15 nodes out
@ five to a row at x 0x74 through Func_a1c6c.
.thumb_func_start Func_a1cb0
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1f2c
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
.func_end Func_a1cb0

@ ShowMessage
@ r0 = string id, r1 = -1 to return immediately, r2 = -1 to reuse the window
@ already at state+0x2C rather than opening one.
@
@ Hides the cursor, and unless r2 is -1 measures the string with _Func_187ac
@ and opens a window of exactly that size at state+0x3C (resizing an existing
@ one through Func_a23f4 rather than reopening it). Renders with _Func_1e7c0
@ when reusing the shared window and _Func_1e74c otherwise.
@
@ When r1 is -1 it sets save bit 0x151 and returns with the box still up --
@ that is how a screen leaves a message on screen while it does something else.
@ Otherwise it spins on Func_30f8(1) until A, B or Start is newly pressed, then
@ tears the window down.
@
@ Either way it raises state+0x222 before returning, so the next Func_a1ac0
@ snaps the cursor instead of gliding it across a screen that has just changed.
.thumb_func_start Func_a1d08
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1f2c
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
	bl	_Func_187ac
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
	bl	Func_a10d0
	cmp	r0, #0
	bne	.La1d68
	ldr	r2, [sp, #8]
	ldr	r0, [r5]
	ldr	r3, [sp, #0xc]
	str	r2, [sp]
	mov	r1, r10
	mov	r2, r8
	bl	Func_a23f4
.La1d68:
	ldr	r5, [r5]
	b	.La1d6e
.La1d6c:
	ldr	r5, [r7, #0x2c]
.La1d6e:
	mov	r0, r5
	bl	_Func_16498
	mov	r0, r5
	bl	_Func_164ac
	mov	r3, #1
	neg	r3, r3
	cmp	r8, r3
	bne	.La1d90
	mov	r0, r6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
	b	.La1d9c
.La1d90:
	mov	r0, r6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e74c
.La1d9c:
	mov	r2, #1
	neg	r2, r2
	cmp	r10, r2
	beq	.La1df4
	mov	r0, #1
	bl	Func_30f8
	mov	r3, #1
	ldr	r6, =iwram_1c94
	mov	r10, r3
.La1db0:
	mov	r0, #1
	bl	Func_30f8
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
	bl	_Func_16498
.La1de2:
	mov	r0, r5
	bl	_Func_164ac
	b	.La1dfa

	.pool_aligned

.La1df4:
	ldr	r0, =0x151
	bl	_Func_79358
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
	bl	Func_a1114
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
.func_end Func_a1d08

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
.thumb_func_start Func_a1e38
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
	bl	_Func_78414
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
.func_end Func_a1e38

@ GetCategoryOrder
@ r0 = which order, r1 = destination. Copies a 0xFF-terminated byte list, at
@ most 32 entries, from one of four tables: .Laf2d0 for 0, .Laf2bc for 1,
@ .Laf2b1 for 2 and .Laf2a6 for anything else. Those bytes are the category
@ sequence Func_a1e38 sorts into.
@
@ Declared `.thumb_Func_start` with a capital F -- see the note on Func_a1c6c.
.thumb_Func_start Func_a1f74
	push	{lr}
	ldr	r2, =Laf2a6
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
	ldr	r2, =Laf2d0
	b	.La1f96
.La1f90:
	ldr	r2, =Laf2bc
	b	.La1f96
.La1f94:
	ldr	r2, =Laf2b1
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
.thumb_func_start Func_a1fd4
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
	bl	_Func_219c8
	mov	r1, r7
	mov	r0, r8
	bl	Func_af0_from_thumb
	mov	r1, r7
	mov	r9, r0
	mov	r0, r8
	bl	Func_b1c_from_thumb
	cmp	r0, #0
	beq	.La2016
	mov	r3, #1
	add	r9, r3
.La2016:
	cmp	r5, #0
	beq	.La2034
	ldr	r2, =iwram_1b04
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
	ldr	r2, =iwram_1b04
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
	bl	_Func_f9080
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
	bl	_Func_f9080
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
	bl	Func_352c
	mov	r0, #1
	b	.La212e
.La20ce:
	cmp	r1, #0
	beq	.La20fc
	mov	r0, #0x6f
	bl	_Func_f9080
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
	bl	_Func_f9080
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
.func_end Func_a1fd4

@ LoadMenuPalette
@ r0 = OBJ palette bank. DMA3-copies the 16 colours at 0x50001E0 into that
@ bank, queues a second eight-halfword copy for the next VBlank, and then
@ brightens the bank's colour 4 by nine steps on each of red, green and blue,
@ clamped at 31. That brightened entry is the highlight the cursor and the
@ selected row are drawn in.
.thumb_func_start Func_a2144
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
.func_end Func_a2144

@ DrawPageIndicator
@ r0 = window, r1 = total entries, r2 = per page, r3 = current page,
@ arg5 = right-hand column. Draws the segmented page bar along the window's
@ first interior row when there is more than one page.
@
@ Pages are total / perPage rounded up. The bar occupies the columns ending at
@ arg5: cap tile 0xF128, then one tile per page taken from 0x31 upward, then
@ cap 0xF129. The segment matching r3 is written in palette bank 2 and the rest
@ in bank 3, which is the whole highlight.
.thumb_func_start Func_a21b0
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
	bl	Func_af0_from_thumb
	mov	r1, r6
	mov	r8, r0
	mov	r0, r5
	bl	Func_b1c_from_thumb
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
	bl	_Func_19000
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
	bl	_Func_19000
	b	.La222e
.La221e:
	mov	r3, #3
	str	r3, [sp]
	mov	r0, r9
	mov	r1, r10
	mov	r2, r7
	sub	r3, #4
	bl	_Func_19000
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
	bl	_Func_19000
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
.func_end Func_a21b0

@ TintTilemapRect
@ r0 = window, r1 = x, r2 = y, r3 = width, arg5 = height, arg6 = palette bank.
@ ORs the bank into bits 12..15 of every tilemap entry in the rectangle, which
@ recolours what is already drawn without touching the tile indices. Coordinates
@ are relative to the window's own origin and clipped to the 30x20 map, and the
@ dirty byte at [iwram_1e8c]+0xEA3 is raised so the next frame uploads it.
.thumb_func_start Func_a2268
	push	{r5, r6, r7, lr}
	mov	r6, r3
	ldr	r3, =iwram_1e8c
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
.func_end Func_a2268

@ SyncObjPaletteToBg
@ Takes no arguments. DMA3-copies OBJ palette bank 0 (0x5000200) down into BG
@ bank 14 (0x50001C0), plus one further colour. Keeps the text drawn into the
@ tilemap the same colours as the sprites drawn over it.
.thumb_func_start Func_a22f4
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
.func_end Func_a22f4

@ ShowNodeRun
@ r0 = count, r1 = first index, r2 = unused, r3 = x, arg5 = y.
@ Hides all 32 nodes at state+0x48 by setting each one's +0x05 to 0x0D, then
@ walks `count` of them from `first`, placing each at x and a y that steps down
@ by 0x10, rewinding it with Func_a17c4 and marking it live. Stops early on a
@ null slot or once the index passes the visible-row count at state+0x218.
.thumb_func_start Func_a2324
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r9, r3
	ldr	r3, =iwram_1f2c
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
	bl	Func_a17c4
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
.func_end Func_a2324

@ DrawCoinTotal
@ r0 = window. Draws the party's money -- the word at ewram_240+0x10 -- as a
@ seven-digit number at x 8, then label 0xB0B at x 0x40.
.thumb_func_start Func_a23c0
	push	{r5, lr}
	ldr	r3, =ewram_240
	sub	sp, #4
	mov	r5, r0
	ldr	r0, [r3, #0x10]
	mov	r3, #0
	str	r3, [sp]
	mov	r2, r5
	mov	r1, #7
	mov	r3, #8
	bl	_Func_1e9d4
	ldr	r0, =0xb0b
	mov	r1, r5
	mov	r2, #0x40
	mov	r3, #0
	bl	_Func_1e7c0
	add	sp, #4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_a23c0

@ SetWindowGeometry
@ r0 = window record or 0, r1 = x column, r2 = y row, r3 = width, arg5 = height,
@ all in tiles. Writes the four fields rom_15000 documents: +0x08 width, +0x0A
@ height, +0x0C x, +0x0E y. Note the argument order does NOT match the record
@ order.
@
@ This is the one function in the module with no `.func_end`, so it gets no ELF
@ `.size`. Harmless -- the next `.thumb_func_start` realigns -- but it is why a
@ size-based tool will report it as zero length.
.thumb_func_start Func_a23f4
	push	{lr}
	cmp	r0, #0
	beq	.La2404
	strh	r3, [r0, #8]
	ldr	r3, [sp, #4]
	strh	r1, [r0, #0xc]
	strh	r3, [r0, #0xa]
	strh	r2, [r0, #0xe]
.La2404:
	pop	{r0}
	bx	r0

@ SuppressUiRedraw
@ Takes no arguments. Sets the byte at [iwram_1e8c]+0xEA6. The screens raise it
@ around a teardown so rom_15000 does not repaint over a fade.
.thumb_func_start Func_a2408
	ldr	r3, =iwram_1e8c
	ldr	r2, =0xea6
	ldr	r3, [r3]
	add	r3, r2
	mov	r2, #1
	strb	r2, [r3]
	bx	lr
.func_end Func_a2408

@ AllowUiRedraw
@ Takes no arguments. Clears [iwram_1e8c]+0xEA6.
.thumb_func_start Func_a2420
	ldr	r3, =iwram_1e8c
	ldr	r2, =0xea6
	ldr	r3, [r3]
	add	r3, r2
	mov	r2, #0
	strb	r2, [r3]
	bx	lr
.func_end Func_a2420

@ PlaySoundAndAccept
@ r0 = sound id. _Func_f9080 then return 1. The shape every "this selection was
@ valid" path in the module ends with.
.thumb_func_start Func_a2438
	push	{lr}
	bl	_Func_f9080
	mov	r0, #1
	pop	{r1}
	bx	r1
.func_end Func_a2438

@ WatchForStart
@ The per-frame task Func_a2474 registers. When Start is newly pressed
@ (iwram_1c94 bit 3) it plays sound 0x71, sets save bit 0x150 and unregisters
@ itself. Every screen loop in this module polls bit 0x150 and unwinds when it
@ goes up, so this is how Start closes the menu from any depth.
.thumb_func_start Func_a2444
	push	{lr}
	ldr	r3, =iwram_1c94
	ldr	r3, [r3]
	mov	r2, #8
	and	r3, r2
	cmp	r3, #0
	beq	.La2466
	mov	r0, #0x71
	bl	_Func_f9080
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79358
	ldr	r0, =Func_a2444
	bl	Func_4278
.La2466:
	pop	{r0}
	bx	r0
.func_end Func_a2444

@ ArmStartWatcher
@ Takes no arguments. Clears save bit 0x150 and registers Func_a2444 at sort
@ key 0xC80.
.thumb_func_start Func_a2474
	push	{lr}
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79374
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_a2444
	bl	Func_41d8
	pop	{r0}
	bx	r0
.func_end Func_a2474

@ DisarmStartWatcher
@ Takes no arguments. Unregisters Func_a2444, unless bit 0x150 is already set --
@ in which case the task removed itself and calling Func_4278 again would strip
@ an unrelated slot.
.thumb_func_start Func_a2490
	push	{lr}
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La24a4
	ldr	r0, =Func_a2444
	bl	Func_4278
.La24a4:
	pop	{r0}
	bx	r0
.func_end Func_a2490
