	.include "macros.inc"
	.include "gba.inc"

@ InitEntitySystem
@ r0=mode. Brings up the entity layer on top of the sprite layer:
@   - allocates the 0x5C-byte system header (tag 6) and the 0x1C00-byte entity
@     table (tag 5) with Func_48f4, then DMA zero-fills both
@   - Func_bb20 initialises the underlying actor/part pools
@   - registers the per-frame update task at priority 0xC8A via Func_41d8:
@     Func_d340 (the 14-slot x/z loop) in mode 4, otherwise Func_cacc
@   - registers the draw task at priority 0xC80: Func_c880 (projected 3D) for
@     modes 3 and 4, otherwise Func_c62c (2D map-relative), which also clears
@     iwram_1d1c and iwram_1cc0
@   - seeds the header's default palette (+0x06 = 0x0F) and flags (+0x07 = 0)
.thumb_func_start Func_c004
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r1, #0x5c
	mov	r0, #6
	sub	sp, #4
	bl	Func_48f4
	mov	r1, #0xe0
	lsl	r1, #5
	mov	r8, r0
	mov	r0, #5
	bl	Func_48f4
	mov	r6, r0
	mov	r0, r7
	bl	Func_bb20
	mov	r5, #0
	mov	r4, sp
	str	r5, [r4]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r4
	mov	r1, r6
	ldr	r2, =0x85000700
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r5, [r4]
	mov	r0, r4
	mov	r1, r8
	ldr	r2, =0x85000017
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	cmp	r7, #4
	bne	.Lc056
	ldr	r0, =Func_d340
	ldr	r1, =0xc8a
	bl	Func_41d8
	b	.Lc05e
.Lc056:
	ldr	r0, =Func_cacc
	ldr	r1, =0xc8a
	bl	Func_41d8
.Lc05e:
	sub	r3, r7, #3
	cmp	r3, #1
	bhi	.Lc070
	mov	r1, #0xc8
	ldr	r0, =Func_c880
	lsl	r1, #4
	bl	Func_41d8
	b	.Lc084
.Lc070:
	mov	r1, #0xc8
	ldr	r0, =Func_c62c
	lsl	r1, #4
	bl	Func_41d8
	ldr	r3, =iwram_1d1c
	mov	r2, #0
	str	r2, [r3]
	ldr	r3, =iwram_1cc0
	str	r2, [r3]
.Lc084:
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0xf
	strb	r3, [r1, #6]
	strb	r2, [r1, #7]
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c004

@ EntityHook_Nop -- do-nothing hook, used where a callback slot must be filled.
.thumb_func_start Func_c0c4
	bx	lr
.func_end Func_c0c4

@ EntityHook_Nop2 -- second do-nothing hook, distinct address from Func_c0c4 so
@ the two slots can be told apart by pointer comparison.
.thumb_func_start Func_c0c8
	bx	lr
.func_end Func_c0c8

@ FindFreeEntitySlot
@ Returns the first unused entity in the 0x40-slot table at [iwram_1e64]
@ (stride 0x70), or 0 when the table is full. A slot counts as free when its
@ script pointer at +0x00 is null.
.thumb_func_start Func_c0cc
	push	{lr}
	ldr	r3, =iwram_1e64
	ldr	r2, [r3]
	ldr	r3, [r2]
	mov	r0, #0
	mov	r1, #0
	b	.Lc0e4
.Lc0da:
	add	r1, #1
	add	r2, #0x70
	cmp	r1, #0x3f
	bgt	.Lc0ea
	ldr	r3, [r2]
.Lc0e4:
	cmp	r3, #0
	bne	.Lc0da
	mov	r0, r2
.Lc0ea:
	pop	{r1}
	bx	r1
.func_end Func_c0cc

@ DestroyEntity
@ r0=entity. Destroys the entity's actors according to the draw kind in the low
@ nibble of +0x54 -- kind 1 is the single actor at +0x50, kind 2 is an array of
@ up to four -- each released with Func_bdd4, then DMA zero-fills the entity's
@ 0x70 bytes to return the slot to the table. No-op on a null pointer.
.thumb_func_start Func_c0f4
	push	{r5, r6, r7, lr}
	mov	r7, r0
	sub	sp, #4
	cmp	r7, #0
	beq	.Lc13e
	mov	r3, r7
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, #0xf
	and	r2, r3
	cmp	r2, #1
	beq	.Lc112
	cmp	r2, #2
	beq	.Lc11a
	b	.Lc12e
.Lc112:
	ldr	r0, [r7, #0x50]
	bl	Func_bdd4
	b	.Lc12e
.Lc11a:
	ldr	r5, [r7, #0x50]
	mov	r6, #3
.Lc11e:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.Lc128
	bl	Func_bdd4
.Lc128:
	sub	r6, #1
	cmp	r6, #0
	bge	.Lc11e
.Lc12e:
	mov	r0, sp
	mov	r3, #0
	str	r3, [r0]
	mov	r1, r7
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x8500001c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.Lc13e:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c0f4

@ SpawnEntity
@ r0=packed descriptor (bits 12+ = draw kind, bits 0-11 = resource id),
@ r1=x, r2=y, r3=z (16.16). Returns the new entity, or 0 if the table is full.
@ Claims a slot with Func_c0cc, then builds the actors:
@   kind 0 -- one actor from Func_bc70; +0x54 = 1 and the collision radius at
@             +0x20 is header[9] >> 1. If Func_bc70 fails, +0x54 = 0 so the
@             entity exists but is never drawn.
@   kind 2 -- bump-allocates a 4-pointer array from [iwram_1e68]+0x18, zeroes
@             it, and fills the first two entries with actors for id and id+1;
@             +0x54 = 2.
@ Func_d130 writes the position, then the entity defaults are applied: script
@ .L1358c, max speed +0x30 = 0x20000, +0x18/+0x1C/+0x34 = 1.0, mode +0x55 = 3,
@ +0x59 = 0, turn-to-face +0x5A = 1, restitution +0x44 = 0x4000, facing
@ +0x06 = 0x4000, and the spawn tile is cached as (x >> 16, z >> 16) in the
@ halfwords at +0x64 and +0x66.
.thumb_func_start Func_c150
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #8
	mov	r7, r0
	mov	r11, r3
	mov	r9, r1
	str	r2, [sp]
	bl	Func_c0cc
	mov	r3, r7
	cmp	r7, #0
	bge	.Lc176
	ldr	r2, .Lc1c8	@ 0xfff
	add	r3, r7, r2
.Lc176:
	asr	r5, r3, #12
	ldr	r3, .Lc1c8	@ 0xfff
	and	r7, r3
	bl	Func_c0cc
	mov	r6, r0
	cmp	r6, #0
	bne	.Lc188
	b	.Lc2be
.Lc188:
	mov	r3, #0x10
	strh	r3, [r6, #0x20]
	cmp	r5, #0
	beq	.Lc196
	cmp	r5, #2
	beq	.Lc1cc
	b	.Lc230
.Lc196:
	mov	r0, r7
	bl	Func_bc70
	mov	r5, r0
	cmp	r5, #0
	beq	.Lc1ba
	mov	r2, r6
	mov	r3, #1
	add	r2, #0x54
	strb	r3, [r2]
	mov	r0, r7
	str	r5, [r6, #0x50]
	bl	_Func_185008
	ldrb	r3, [r0, #9]
	lsr	r3, #1
	strh	r3, [r6, #0x20]
	b	.Lc230
.Lc1ba:
	mov	r2, r6
	ldr	r3, .Lc1c4	@ 0
	add	r2, #0x54
	strb	r3, [r2]
	b	.Lc230

	.align	2, 0
.Lc1c4:
	.word	0
.Lc1c8:
	.word	0xfff

.Lc1cc:
	ldr	r3, =iwram_1e68
	ldr	r1, [r3]
	mov	r0, r1
	add	r0, #0x18
	ldr	r3, [r0]
	lsl	r2, r3, #2
	add	r1, r2
	add	r3, #1
	str	r3, [r0]
	mov	r10, r1
	mov	r3, r6
	mov	r4, #8
	add	r4, r10
	add	r3, #0x54
	strb	r5, [r3]
	mov	r8, r4
	add	r0, sp, #4
	mov	r3, #0
	str	r3, [r0]
	str	r4, [r6, #0x50]
	ldr	r3, =REG_DMA3SAD
	mov	r1, r8
	ldr	r2, =0x85000004
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r7
	bl	Func_bc70
	mov	r5, r0
	cmp	r5, #0
	beq	.Lc220
	mov	r0, r7
	bl	_Func_185008
	ldrb	r3, [r0, #9]
	lsr	r3, #1
	mov	r2, r8
	strh	r3, [r6, #0x20]
	mov	r3, #0xc
	add	r3, r10
	str	r5, [r2]
	mov	r8, r3
.Lc220:
	add	r0, r7, #1
	bl	Func_bc70
	mov	r5, r0
	cmp	r5, #0
	beq	.Lc230
	mov	r4, r8
	str	r5, [r4]
.Lc230:
	cmp	r6, #0
	beq	.Lc2be
	mov	r0, r6
	mov	r1, r9
	ldr	r2, [sp]
	mov	r3, r11
	bl	Func_d130
	ldr	r3, =.L1358c
	str	r3, [r6]
	mov	r3, #0x80
	lsl	r3, #10
	str	r3, [r6, #0x30]
	mov	r3, #0x55
	add	r3, r6
	mov	r12, r3
	mov	r2, #0x80
	lsl	r2, #9
	mov	r1, #0
	mov	r3, #3
	mov	r4, r12
	strh	r1, [r6, #4]
	str	r2, [r6, #0x18]
	str	r2, [r6, #0x1c]
	str	r2, [r6, #0x34]
	ldr	r0, .Lc28c	@ 0
	strb	r3, [r4]
	str	r2, [r6, #0x48]
	mov	r3, r6
	mov	r2, #0x80
	lsl	r2, #7
	add	r3, #0x59
	str	r2, [r6, #0x44]
	strb	r0, [r3]
	mov	r0, r6
	add	r0, #0x5a
	mov	r3, #1
	strb	r3, [r0]
	str	r1, [r6, #0x4c]
	mov	r1, r9
	strh	r2, [r6, #6]
	cmp	r1, #0
	bge	.Lc2a4
	ldr	r2, =0xffff
	add	r1, r2
	b	.Lc2a4

	.align	2, 0
.Lc28c:
	.word	0
	.pool

.Lc2a4:
	mov	r3, r6
	add	r3, #0x64
	asr	r2, r1, #16
	strh	r2, [r3]
	mov	r3, r11
	cmp	r3, #0
	bge	.Lc2b6
	ldr	r4, =0xffff
	add	r3, r4
.Lc2b6:
	mov	r2, r6
	asr	r3, #16
	add	r2, #0x66
	strh	r3, [r2]
.Lc2be:
	mov	r0, r6
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_c150

@ SetEntityScript
@ r0=entity, r1=script. Points the entity's VM at a new script: +0x00 = script,
@ cursor +0x04 = 0, and the state bytes at +0x57, +0x5B and +0x5D are cleared so
@ the freeze flag and any pending sub-state are dropped. No-op on a null entity.
.thumb_func_start Func_c2d8
	push	{lr}
	cmp	r0, #0
	beq	.Lc2fc
	mov	r2, #0
	strh	r2, [r0, #4]
	ldr	r3, =0
	mov	r2, r0
	add	r2, #0x5b
	str	r1, [r0]
	strb	r3, [r2]
	add	r2, #2
	strb	r3, [r2]
	sub	r2, #6
	strb	r3, [r2]
	b	.Lc2fc

	.pool_aligned

.Lc2fc:
	pop	{r0}
	bx	r0
.func_end Func_c2d8

@ SetEntityAnimation
@ r0=entity, r1=animation index. Forwards to Func_ba30 for the entity's actor
@ (draw kind 1) or for each of the up to four actors in the array (kind 2).
.thumb_func_start Func_c300
	push	{r5, r6, r7, lr}
	mov	r7, r1
	cmp	r0, #0
	beq	.Lc33c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, #0xf
	and	r2, r3
	cmp	r2, #1
	beq	.Lc31c
	cmp	r2, #2
	beq	.Lc326
	b	.Lc33c
.Lc31c:
	ldr	r0, [r0, #0x50]
	mov	r1, r7
	bl	Func_ba30
	b	.Lc33c
.Lc326:
	ldr	r5, [r0, #0x50]
	mov	r6, #3
.Lc32a:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.Lc336
	mov	r1, r7
	bl	Func_ba30
.Lc336:
	sub	r6, #1
	cmp	r6, #0
	bge	.Lc32a
.Lc33c:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c300

@ SetEntityAnimSpeed
@ r0=entity, r1=speed. Forwards to Func_baf8 for the entity's actor (draw
@ kind 1) or for each actor in the array (kind 2). 0x10 is normal speed.
.thumb_func_start Func_c344
	push	{r5, r6, r7, lr}
	mov	r7, r1
	cmp	r0, #0
	beq	.Lc380
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, #0xf
	and	r2, r3
	cmp	r2, #1
	beq	.Lc360
	cmp	r2, #2
	beq	.Lc36a
	b	.Lc380
.Lc360:
	ldr	r0, [r0, #0x50]
	mov	r1, r7
	bl	Func_baf8
	b	.Lc380
.Lc36a:
	ldr	r5, [r0, #0x50]
	mov	r6, #3
.Lc36e:
	ldmia	r5!, {r0}
	cmp	r0, #0
	beq	.Lc37a
	mov	r1, r7
	bl	Func_baf8
.Lc37a:
	sub	r6, #1
	cmp	r6, #0
	bge	.Lc36e
.Lc380:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c344

@ SetEntityAnimationAndSpeed
@ r0=entity, r1=animation index, r2=speed. Func_c300 and Func_c344 in one call:
@ applies Func_ba30 then Func_baf8 to the entity's actor or actor array.
.thumb_func_start Func_c388
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r5, r0
	mov	r8, r1
	mov	r10, r2
	cmp	r5, #0
	beq	.Lc3e0
	mov	r3, r5
	add	r3, #0x54
	ldrb	r3, [r3]
	mov	r2, #0xf
	and	r2, r3
	cmp	r2, #1
	beq	.Lc3ae
	cmp	r2, #2
	beq	.Lc3c0
	b	.Lc3e0
.Lc3ae:
	mov	r1, r8
	ldr	r0, [r5, #0x50]
	bl	Func_ba30
	ldr	r0, [r5, #0x50]
	mov	r1, r10
	bl	Func_baf8
	b	.Lc3e0
.Lc3c0:
	ldr	r6, [r5, #0x50]
	mov	r7, #3
.Lc3c4:
	ldmia	r6!, {r5}
	cmp	r5, #0
	beq	.Lc3da
	mov	r1, r8
	mov	r0, r5
	bl	Func_ba30
	mov	r0, r5
	mov	r1, r10
	bl	Func_baf8
.Lc3da:
	sub	r7, #1
	cmp	r7, #0
	bge	.Lc3c4
.Lc3e0:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c388

@ SetEntityActorRotation
@ r0=entity, r1=angle. Writes the rotation halfword at actor+0x1E, which
@ Func_b168/Func_b388 fold into the affine matrix. Draw kind 1 only.
.thumb_func_start Func_c3ec
	push	{lr}
	cmp	r0, #0
	beq	.Lc404
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.Lc404
	ldr	r3, [r0, #0x50]
	strh	r1, [r3, #0x1e]
.Lc404:
	pop	{r0}
	bx	r0
.func_end Func_c3ec

@ SetEntityPartResource
@ r0=entity, r1=resource id (negative is ignored). Rewrites the id halfword of
@ the actor's first part and re-runs Func_b7c0 to rebind every part to the new
@ resource -- i.e. swaps the sprite an entity is wearing. Draw kind 1 only.
.thumb_func_start Func_c408
	push	{lr}
	cmp	r0, #0
	beq	.Lc42a
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.Lc42a
	ldr	r0, [r0, #0x50]
	cmp	r1, #0
	blt	.Lc42a
	ldr	r3, [r0, #0x28]
	strh	r1, [r3]
	bl	Func_b7c0
.Lc42a:
	pop	{r0}
	bx	r0
.func_end Func_c408

@ AddEntityPart
@ r0=entity, r1=resource id (negative is ignored). Attaches an extra part to
@ the entity's actor via Func_b8ac, e.g. an overlay or held item. Draw kind 1
@ only.
.thumb_func_start Func_c430
	push	{lr}
	cmp	r0, #0
	beq	.Lc44e
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.Lc44e
	ldr	r0, [r0, #0x50]
	cmp	r1, #0
	blt	.Lc44e
	bl	Func_b8ac
.Lc44e:
	pop	{r0}
	bx	r0
.func_end Func_c430

@ StartBehaviour_13608
@ r0=entity, r1=parameter. Installs the canned behaviour script .L13608 via
@ Func_c2d8 and stores the parameter in the script argument slot at +0x68.
.thumb_func_start Func_c454
	push	{r5, r6, lr}
	mov	r6, r1
	ldr	r1, =.L13608
	mov	r5, r0
	bl	Func_c2d8
	str	r6, [r5, #0x68]
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c454

@ StartBehaviour_13590 -- r0=entity. Installs behaviour script .L13590.
.thumb_func_start Func_c46c
	push	{lr}
	ldr	r1, =.L13590
	bl	Func_c2d8
	pop	{r0}
	bx	r0
.func_end Func_c46c

@ StartBehaviour_135a8 -- r0=entity. Installs behaviour script .L135a8.
.thumb_func_start Func_c47c
	push	{lr}
	ldr	r1, =.L135a8
	bl	Func_c2d8
	pop	{r0}
	bx	r0
.func_end Func_c47c

@ StartBehaviour_135c0 -- r0=entity. Installs behaviour script .L135c0.
.thumb_func_start Func_c48c
	push	{lr}
	ldr	r1, =.L135c0
	bl	Func_c2d8
	pop	{r0}
	bx	r0
.func_end Func_c48c

@ StartBehaviour_135d8 -- r0=entity. Installs behaviour script .L135d8.
.thumb_func_start Func_c49c
	push	{lr}
	ldr	r1, =.L135d8
	bl	Func_c2d8
	pop	{r0}
	bx	r0
.func_end Func_c49c

@ StartBehaviour_13620 -- r0=entity. Installs behaviour script .L13620, the
@ 4-byte (single word) script immediately before the Data_13624 opcode table.
.thumb_func_start Func_c4ac
	push	{lr}
	ldr	r1, =.L13620
	bl	Func_c2d8
	pop	{r0}
	bx	r0
.func_end Func_c4ac

@ StartBehaviour_135f0
@ r0=entity, r1=parameter. Installs behaviour script .L135f0. When the
@ parameter is non-zero it also overrides the movement tuning for this script:
@ acceleration +0x34 = 0x8000 (half speed), max speed +0x30 = 0x40000, the
@ script argument at +0x68 = r1, and the spawn-tile halfword at +0x64 is reset.
.thumb_func_start Func_c4bc
	push	{r5, r6, lr}
	mov	r6, r1
	ldr	r1, =.L135f0
	mov	r5, r0
	bl	Func_c2d8
	cmp	r6, #0
	beq	.Lc4e2
	mov	r3, #0x80
	lsl	r3, #8
	str	r3, [r5, #0x34]
	mov	r3, #0x80
	lsl	r3, #11
	mov	r2, r5
	str	r3, [r5, #0x30]
	add	r2, #0x64
	mov	r3, #0
	str	r6, [r5, #0x68]
	strh	r3, [r2]
.Lc4e2:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c4bc

@ WaitForScriptOpcode10
@ r0=entity. Blocks until the entity's script VM reaches opcode 0x10 -- the
@ conventional "idle / ready" instruction -- polling the word at
@ script[cursor] once per frame with Func_30f8(1). Gives up after 0x12B (299)
@ frames. Returns immediately if the entity is already sitting on opcode 0x10.
.thumb_func_start Func_c4ec
	push	{r5, r6, lr}
	mov	r5, r0
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	r6, #0
	cmp	r3, #0x10
	beq	.Lc51c
.Lc500:
	mov	r0, #1
	bl	Func_30f8
	ldr	r3, =0x12b
	add	r6, #1
	cmp	r6, r3
	bgt	.Lc51c
	mov	r2, #4
	ldrsh	r3, [r5, r2]
	ldr	r2, [r5]
	lsl	r3, #2
	ldr	r3, [r3, r2]
	cmp	r3, #0x10
	bne	.Lc500
.Lc51c:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_c4ec

@ SetEntityActorOptions
@ r0=entity, r1=option bits. Writes the actor's option byte at +0x26, whose
@ bit 0 enables the companion/shadow sprite and bit 1 the text outline pass
@ (see Func_b388 and Func_aa0c). Draw kind 1 only.
.thumb_func_start Func_c528
	push	{lr}
	cmp	r0, #0
	beq	.Lc542
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.Lc542
	ldr	r3, [r0, #0x50]
	add	r3, #0x26
	strb	r1, [r3]
.Lc542:
	pop	{r0}
	bx	r0
.func_end Func_c528

@ SetEntityActorPriority
@ r0=entity, r1=priority 0-3. Replaces bits 2-3 of the actor's flag byte at
@ +0x05 -- the OAM priority field -- leaving the affine mode in bits 0-1 alone.
@ Requires an exact draw kind of 1 (not just the low nibble).
.thumb_func_start Func_c548
	push	{lr}
	cmp	r0, #0
	beq	.Lc56c
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.Lc56c
	ldr	r0, [r0, #0x50]
	mov	r3, #3
	and	r1, r3
	ldrb	r2, [r0, #5]
	mov	r3, #0xd
	neg	r3, r3
	lsl	r1, #2
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #5]
.Lc56c:
	pop	{r0}
	bx	r0
.func_end Func_c548

@ SetEntityUseCallerScale
@ r0=entity, r1=0 or 1. Replaces bit 1 of the actor's flag byte at +0x1D, the
@ flag Func_b388 tests to decide whether to compute the perspective scale from
@ the depth or take the caller-supplied value. Requires draw kind exactly 1.
.thumb_func_start Func_c570
	push	{lr}
	cmp	r0, #0
	beq	.Lc594
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.Lc594
	ldr	r0, [r0, #0x50]
	mov	r3, #1
	and	r1, r3
	ldrb	r2, [r0, #0x1d]
	mov	r3, #3
	neg	r3, r3
	lsl	r1, #1
	and	r3, r2
	orr	r3, r1
	strb	r3, [r0, #0x1d]
.Lc594:
	pop	{r0}
	bx	r0
.func_end Func_c570

@ SetEntityPalette
@ r0=entity, r1=palette index. Forwards to Func_b684, which stamps the palette
@ onto every line of the actor's label (skipping ones pinned to 0x0F) and marks
@ it dirty. Requires draw kind exactly 1.
.thumb_func_start Func_c598
	push	{lr}
	cmp	r0, #0
	beq	.Lc5ae
	mov	r3, r0
	add	r3, #0x54
	ldrb	r3, [r3]
	cmp	r3, #1
	bne	.Lc5ae
	ldr	r0, [r0, #0x50]
	bl	Func_b684
.Lc5ae:
	pop	{r0}
	bx	r0
.func_end Func_c598

@ SuspendEntityDrawTasks
@ Takes no arguments. Suspends both draw tasks (Func_c62c and Func_c880) with
@ Func_42c8, so OAM stops being rewritten and the sprites freeze as they are.
@ Then _Func_91200(0x10000, 1) and _Func_91254(1) run, one frame is allowed to
@ pass with Func_30f8(1), and DISPCNT is rewritten to disable BG1/BG2/BG3
@ (mask 0xF1FF) while forcing OBJ on (bit 12) -- leaving a sprite-only display.
@ Func_c5fc is the counterpart.
.thumb_func_start Func_c5b4
	push	{lr}
	ldr	r0, =Func_c62c
	bl	Func_42c8
	ldr	r0, =Func_c880
	bl	Func_42c8
	mov	r0, #0x80
	mov	r1, #1
	lsl	r0, #9
	bl	_Func_91200
	mov	r0, #1
	bl	_Func_91254
	mov	r0, #1
	bl	Func_30f8
	mov	r1, #0x80
	lsl	r1, #19
	ldrh	r2, [r1]
	ldr	r3, =0xf1ff
	and	r3, r2
	ldr	r2, .Lc5ec	@ 0x1000
	orr	r3, r2
	strh	r3, [r1]
	pop	{r0}
	bx	r0

	.align	2, 0
.Lc5ec:
	.word	0x1000
.func_end Func_c5b4

@ ResumeEntityDrawTasks
@ Takes no arguments. Undoes Func_c5b4: re-enables both draw tasks (Func_c62c
@ and Func_c880) with Func_439c and clears BG1/BG2/BG3 *and* OBJ from DISPCNT
@ (mask 0xE1FF), leaving the layers dark until the resumed tasks repopulate OAM.
.thumb_func_start Func_c5fc
	push	{lr}
	ldr	r0, =Func_c62c
	bl	Func_439c
	ldr	r0, =Func_c880
	bl	Func_439c
	mov	r1, #0x80
	lsl	r1, #19
	ldrh	r2, [r1]
	ldr	r3, =0xe1ff
	and	r3, r2
	strh	r3, [r1]
	pop	{r0}
	bx	r0
.func_end Func_c5fc

@ ScriptOp_Continue -- opcode handler that does nothing and returns 1, keeping
@ the VM running. The no-op entry in the Data_13624 dispatch table.
.thumb_func_start Func_c628
	mov	r0, #1
	bx	lr
.func_end Func_c628

@ DrawWorldEntities2D
@ Per-frame sprite pass for the map (non-projected) view; the counterpart to
@ Func_c880, registered by Func_c004 for modes other than 3 and 4. Takes no
@ arguments.
@ Setup: reads the map state from [iwram_1e70], caches the camera origin at
@ +0xE4/+0xE8 truncated to whole pixels, and allocates the usual 0x2C4 scratch
@ arena with a RAM copy of Func_9bb8.
@ Loop: walks the 0x40 entities (stride 0x70) and, for each active slot whose
@ draw kind (+0x54 low nibble) is 1:
@   - culls against the same screen window as Func_bfa4 -- x within
@     [-32, 272), y within [-32, 224). A culled entity releases its tile
@     allocation with Func_3f78 and sets the dirty flag at actor+0x25, unless
@     the keep-loaded flag at +0x5C is set.
@   - looks up the map attribute word for the entity's tile: the layer table at
@     [map + 0x130 + layer * 0x30] indexed by (z >> 20) * 128 + (x >> 20).
@     Bits 16-17 set the OAM priority in actor+0x09 and +0x15 (only when bit 0
@     of +0x1B enables it); bits 18-19 set the tile-type byte at entity+0x22.
@   - scales the entity's two size fields by the actor depth at +0x18 through
@     Func_888, builds the position triple on the stack, and applies the two
@     display offsets from +0x23: bit 1 shifts by -0x140.0000 and bit 2 by
@     +0x140.0000.
@   - submits with Func_b168, passing the entity facing at +0x06.
@ Releases the scratch arena with Func_2dd8 before returning.
.thumb_func_start Func_c62c
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r6, =iwram_1e70
	ldr	r0, [r6]
	sub	sp, #0x50
	str	r0, [sp, #0xc]
	mov	r2, r0
	add	r2, #0xe4
	ldr	r1, [r2]
	ldr	r3, =0xffff0000
	and	r1, r3
	str	r1, [sp, #8]
	ldr	r2, [r2, #4]
	and	r2, r3
	str	r2, [sp, #4]
	mov	r3, r6
	sub	r3, #8
	ldr	r3, [r3]
	str	r3, [sp]
	ldr	r5, =0x2c4
	mov	r0, #0x34
	mov	r1, r5
	bl	Func_48b0
	mov	r2, #0x84
	lsr	r5, #2
	lsl	r2, #24
	mov	r1, r0
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =Func_9bb8
	orr	r2, r5
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0
	sub	r6, #0xc
	ldr	r6, [r6]
	ldr	r2, [sp]
	mov	r10, r6
	strh	r3, [r2]
	mov	r4, #0x3f
	ldr	r3, =Func_888
	mov	r0, #0x54
	add	r0, r10
	mov	r7, r10
	str	r4, [sp, #0x10]
	mov	r11, r3
	mov	r8, r0
	add	r7, #8
.Lc696:
	mov	r1, r10
	ldr	r3, [r1]
	cmp	r3, #0
	bne	.Lc6a0
	b	.Lc822
.Lc6a0:
	ldr	r1, [r7]
	cmp	r1, #0
	bne	.Lc6ae
	ldr	r3, [r7, #8]
	cmp	r3, #0
	bne	.Lc6ae
	b	.Lc7f8
.Lc6ae:
	mov	r2, r8
	ldrb	r3, [r2]
	mov	r6, #0xf
	and	r6, r3
	cmp	r6, #0
	bne	.Lc6bc
	b	.Lc822
.Lc6bc:
	cmp	r6, #1
	beq	.Lc6c2
	b	.Lc822
.Lc6c2:
	ldr	r0, [sp]
	mov	r4, #4
	ldrsh	r3, [r0, r4]
	cmp	r3, #0
	beq	.Lc6e0
	ldrb	r3, [r2, #8]
	cmp	r3, #0
	bne	.Lc6e0
	ldr	r5, [r7, #0x48]
	ldrb	r0, [r5, #0x1c]
	add	r5, #0x25
	bl	Func_3f78
	strb	r6, [r5]
	b	.Lc822
.Lc6e0:
	ldr	r3, [sp, #4]
	ldr	r0, [r7, #8]
	ldr	r2, [sp, #8]
	sub	r6, r0, r3
	ldr	r3, [r7, #4]
	sub	r2, r1, r2
	mov	r9, r2
	sub	r2, r6, r3
	ldr	r3, =0x1fffff
	ldr	r4, =0x12ffffe
	add	r3, r9
	ldr	r5, [r7, #0x48]
	cmp	r3, r4
	bls	.Lc6fe
	b	.Lc7ea
.Lc6fe:
	ldr	r3, =0xffe00000
	cmp	r2, r3
	ble	.Lc7ea
	ldr	r4, =0xdfffff
	cmp	r2, r4
	bgt	.Lc7ea
	ldrb	r3, [r7, #0x1a]
	mov	r2, #0x22
	add	r2, r10
	mov	r12, r2
	lsl	r2, r3, #1
	add	r2, r3
	mov	r3, #0x98
	lsl	r3, #1
	lsl	r2, #4
	add	r2, r3
	ldr	r4, [sp, #0xc]
	asr	r3, r0, #20
	lsl	r3, #7
	asr	r1, #20
	add	r1, r3
	ldr	r3, [r4, r2]
	lsl	r1, #2
	ldrb	r2, [r7, #0x1b]
	add	r1, r3, r1
	mov	r0, #0x23
	mov	r3, #1
	add	r0, r10
	and	r3, r2
	mov	r14, r0
	cmp	r3, #0
	beq	.Lc762
	ldr	r4, [r1]
	lsl	r3, r4, #16
	lsr	r0, r3, #30
	cmp	r0, #0
	beq	.Lc764
	mov	r2, #0xd
	ldrb	r1, [r5, #9]
	neg	r2, r2
	mov	r3, r2
	lsl	r0, #2
	and	r3, r1
	orr	r3, r0
	strb	r3, [r5, #9]
	ldrb	r3, [r5, #0x15]
	and	r2, r3
	orr	r2, r0
	strb	r2, [r5, #0x15]
	b	.Lc764
.Lc762:
	ldr	r4, [r1]
.Lc764:
	lsl	r3, r4, #18
	lsr	r1, r3, #30
	cmp	r1, #0
	beq	.Lc774
	mov	r3, r1
	add	r3, #0xff
	mov	r1, r12
	strb	r3, [r1]
.Lc774:
	ldr	r0, [r7, #0x10]
	ldr	r1, [r5, #0x18]
	.call_via r11
	str	r0, [sp, #0x14]
	ldr	r0, [r7, #0x14]
	ldr	r1, [r5, #0x18]
	.call_via r11
	mov	r2, #0x14
	add	r1, sp, #0x1c
	add	r2, sp
	mov	r3, r9
	str	r0, [r2, #4]
	str	r3, [r1]
	mov	r12, r2
	ldr	r2, [r7, #4]
	str	r6, [r1, #8]
	str	r2, [r1, #4]
	ldr	r4, [r7, #0xc]
	str	r4, [r1, #0xc]
	mov	r3, r14
	ldrb	r0, [r3]
	mov	r3, #2
	and	r3, r0
	cmp	r3, #0
	beq	.Lc7be
	ldr	r3, =0xfec00000
	add	r2, r3
	str	r2, [r1, #4]
	add	r2, r6, r3
	add	r3, r4, r3
	str	r2, [r1, #8]
	str	r3, [r1, #0xc]
	mov	r4, r14
	ldrb	r0, [r4]
.Lc7be:
	mov	r3, #4
	and	r3, r0
	cmp	r3, #0
	beq	.Lc7dc
	ldr	r3, [r1, #4]
	mov	r2, #0xa0
	lsl	r2, #17
	add	r3, r2
	str	r3, [r1, #4]
	ldr	r3, [r1, #8]
	add	r3, r2
	str	r3, [r1, #8]
	ldr	r3, [r1, #0xc]
	add	r3, r2
	str	r3, [r1, #0xc]
.Lc7dc:
	mov	r0, r10
	ldrh	r3, [r0, #6]
	mov	r2, r12
	mov	r0, r5
	bl	Func_b168
	b	.Lc822
.Lc7ea:
	mov	r1, r8
	ldrb	r3, [r1, #8]
	cmp	r3, #0
	bne	.Lc822
	ldrb	r2, [r5, #0x1d]
	mov	r6, #1
	b	.Lc80e
.Lc7f8:
	mov	r2, r8
	ldrb	r3, [r2]
	mov	r6, #0xf
	and	r6, r3
	cmp	r6, #1
	bne	.Lc822
	ldrb	r3, [r2, #8]
	ldr	r5, [r7, #0x48]
	cmp	r3, #0
	bne	.Lc822
	ldrb	r2, [r5, #0x1d]
.Lc80e:
	mov	r3, r6
	and	r3, r2
	cmp	r3, #0
	bne	.Lc822
	ldrb	r0, [r5, #0x1c]
	bl	Func_3f78
	mov	r3, r5
	add	r3, #0x25
	strb	r6, [r3]
.Lc822:
	ldr	r3, [sp, #0x10]
	mov	r4, #0x70
	sub	r3, #1
	str	r3, [sp, #0x10]
	add	r8, r4
	add	r7, #0x70
	add	r10, r4
	cmp	r3, #0
	blt	.Lc836
	b	.Lc696
.Lc836:
	mov	r0, #0x34
	bl	Func_2dd8
	add	sp, #0x50
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_c62c

@ ScriptOp_Continue2 -- second no-op opcode handler returning 1, at a distinct
@ address from Func_c628 so the two table entries stay distinguishable.
.thumb_func_start Func_c87c
	mov	r0, #1
	bx	lr
.func_end Func_c87c

	.section .rodata

@ Entity behaviour scripts. Each is a word array of opcodes consumed by the VM
@ in Func_cacc / Func_a494 and dispatched through Data_13624. .L1358c is the
@ default installed by Func_c150 at spawn; the rest are selected by the
@ Func_c46c..Func_c4bc family. They sit immediately before Data_13624 in rodata.
.L1358c:
	.incrom 0x1358c, 0x13590
.L13590:
	.incrom 0x13590, 0x135a8
.L135a8:
	.incrom 0x135a8, 0x135c0
.L135c0:
	.incrom 0x135c0, 0x135d8
.L135d8:
	.incrom 0x135d8, 0x135f0
.L135f0:
	.incrom 0x135f0, 0x13608
.L13608:
	.incrom 0x13608, 0x13620
.L13620:
	.incrom 0x13620, 0x13624
