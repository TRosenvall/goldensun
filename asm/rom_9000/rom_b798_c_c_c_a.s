	.include "macros.inc"
	.include "gba.inc"

@ CreateActor
@ r0=resource id. Allocates and initialises a new actor, returning it (0 on
@ failure). Steps:
@   - fetch the resource header; AllocSpriteSlot derives the tile-allocation size code
@     (0x60 means "too large", which aborts)
@   - claim the first free entry in the actor pool at [iwram_1e60] (stride 0x38,
@     up to 0x40 entries; free means +0x20 is 0)
@   - UploadSpriteGFX reserves OBJ tiles; the size code is kept at +0x1C, +0x1E is
@     cleared and the visible flag at +0x26 is set
@   - map the sprite's (header[0] << 8 | header[1]) pixel dimensions onto the
@     OAM shape/size bits via the comparison chain at .Lbd12..: 0x0808, 0x0810,
@     0x1008, 0x1010, 0x1020, 0x2020, 0x2040, 0x4020, 0x4040. An unrecognised
@     pair falls through to 0 (8x8).
@   - write the OAM attribute template into +0x00..+0x14: attr0 = 0,
@     attr1 = shape/size | 0x2000, attr2 = tile | 0x800, then the priority word
@     0x6000 and a palette field taken from iwram_1b10[0xBB] >> 5
@   - Sprite_AddLayer attaches the first part for the same resource id
.thumb_func_start CreateSprite  @ 0x0800bc70
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r1, #0
	mov	r8, r1
	mov	r10, r0
	bl	_GetSpriteInfo
	mov	r7, r0
	bl	AllocSpriteSlot
	ldr	r3, =iwram_3001e60
	ldr	r5, [r3]
	ldrb	r3, [r7]
	mov	r6, r0
	mov	r0, #0
	cmp	r3, #0
	bne	.Lbc98
	b	.Lbd9c
.Lbc98:
	mov	r3, r5
	add	r3, #0x20
	ldrb	r3, [r3]
	mov	r2, #0
	b	.Lbcb0
.Lbca2:
	add	r2, #1
	add	r5, #0x38
	cmp	r2, #0x3f
	bgt	.Lbcb6
	mov	r3, r5
	add	r3, #0x20
	ldrb	r3, [r3]
.Lbcb0:
	cmp	r3, #0
	bne	.Lbca2
	mov	r8, r5
.Lbcb6:
	mov	r2, r8
	mov	r0, #0
	cmp	r2, #0
	beq	.Lbd9c
	cmp	r6, #0x60
	beq	.Lbd9c
	mov	r0, r6
	mov	r1, #0
	mov	r2, #0
	bl	UploadSpriteGFX
	mov	r12, r0
	cmp	r0, #0
	bne	.Lbcd6
	mov	r0, #0
	b	.Lbd9c
.Lbcd6:
	mov	r3, #0
	mov	r2, r8
	mov	r1, r8
	strb	r6, [r1, #0x1c]
	strh	r3, [r2, #0x1e]
	add	r2, #0x26
	mov	r3, #1
	strb	r3, [r2]
	ldrb	r3, [r7]
	ldrb	r2, [r7, #1]
	lsl	r3, #8
	add	r0, r3, r2
	mov	r3, #0x81
	lsl	r3, #5
	ldr	r4, =0x80008000
	cmp	r0, r3
	beq	.Lbd5e
	cmp	r0, r3
	bhi	.Lbd28
	mov	r3, #0x81
	mov	r4, #0x80
	lsl	r3, #4
	lsl	r4, #8
	cmp	r0, r3
	beq	.Lbd5e
	cmp	r0, r3
	bhi	.Lbd12
	sub	r3, #8
	mov	r4, #0
	b	.Lbd58
.Lbd12:
	ldr	r1, =0x1008
	mov	r4, #0x80
	lsl	r4, #7
	cmp	r0, r1
	beq	.Lbd5e
	ldr	r2, =0x1010
	mov	r4, #0x80
	lsl	r4, #23
	cmp	r0, r2
	beq	.Lbd5e
	b	.Lbd5c
.Lbd28:
	mov	r3, #0x81
	lsl	r3, #6
	ldr	r4, =0xc0008000
	cmp	r0, r3
	beq	.Lbd5e
	cmp	r0, r3
	bhi	.Lbd4a
	sub	r3, #0x30
	ldr	r4, =0x80004000
	cmp	r0, r3
	beq	.Lbd5e
	ldr	r1, =0x2020
	mov	r4, #0x80
	lsl	r4, #24
	cmp	r0, r1
	beq	.Lbd5e
	b	.Lbd5c
.Lbd4a:
	ldr	r2, =0x4020
	ldr	r4, =0xc0004000
	cmp	r0, r2
	beq	.Lbd5e
	ldr	r3, =0x4040
	mov	r4, #0xc0
	lsl	r4, #24
.Lbd58:
	cmp	r0, r3
	beq	.Lbd5e
.Lbd5c:
	mov	r4, #0
.Lbd5e:
	mov	r2, r8
	mov	r1, #0
	mov	r3, #0x80
	stmia	r2!, {r1}
	lsl	r3, #6
	orr	r4, r3
	mov	r0, #0x80
	stmia	r2!, {r4}
	lsl	r0, #4
	mov	r3, r12
	orr	r3, r0
	stmia	r2!, {r3}
	mov	r3, #0xc0
	stmia	r2!, {r1}
	lsl	r3, #7
	stmia	r2!, {r3}
	mov	r1, #0xbb
	ldr	r3, =gSpriteSlots
	lsl	r1, #1
	add	r3, r1
	ldrh	r3, [r3]
	lsr	r3, #5
	orr	r3, r0
	str	r3, [r2]
	mov	r0, r5
	mov	r1, r10
	bl	Sprite_AddLayer
	mov	r2, #1
	neg	r2, r2
	mov	r0, r8
.Lbd9c:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end CreateSprite

@ DestroyActor
@ r0=actor. Releases the actor's OBJ tile allocation with Func_3f3c (skipped
@ when bit 0 of +0x1D marks the tiles as externally owned), frees all four part
@ slots with DeleteSpriteLayer, then DMA3 zero-fills the actor's 0x38 bytes (0xE words)
@ to return it to the pool. No-op on a null pointer.
.thumb_func_start DeleteSprite  @ 0x0800bdd4
	push	{r5, r6, r7, lr}
	mov	r7, r0
	sub	sp, #4
	cmp	r7, #0
	beq	.Lbe10
	ldrb	r2, [r7, #0x1d]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.Lbdee
	ldrb	r0, [r7, #0x1c]
	bl	Func_8003f3c
.Lbdee:
	mov	r5, r7
	add	r5, #0x28
	mov	r6, #3
.Lbdf4:
	ldmia	r5!, {r0}
	sub	r6, #1
	bl	DeleteSpriteLayer
	cmp	r6, #0
	bge	.Lbdf4
	mov	r0, sp
	mov	r3, #0
	str	r3, [r0]
	mov	r1, r7
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x8500000e
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.Lbe10:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end DeleteSprite
