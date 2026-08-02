	.include "macros.inc"

@ FindEntityAtPosition
@ r0 = an {x, y, z} triple, r1 = the entity to skip (the caller itself).
@ Scans entity slots 8..0x41 -- the map-object range, above the party slots --
@ from the table at [iwram_1ebc]+0x34, and returns the first whose position
@ matches r0. Returns 0 if nothing is there.
@
@ The comparison is deliberately mismatched between axes: x and z are compared
@ at whole-tile resolution (`asr #20`) but y at 1/16 (`asr #16`, with 0xFFFF
@ added first to round negatives toward zero). So an object counts as "here" if
@ it shares the tile, but only if it is on the same height step -- which is what
@ keeps a log on a ledge from blocking one on the floor below.
.thumb_func_start OvlFunc_883_200806c
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_3001ebc
	mov	r4, r0
	ldr	r2, [r3]
	ldr	r3, [r4]
	mov	r1, r2
	ldr	r6, =0xffff
	mov	r5, #8
	asr	r7, r3, #20
	add	r1, #0x34
.L80:
	ldmia	r1!, {r0}
	ldr	r3, [r0, #8]
	asr	r3, #20
	cmp	r7, r3
	bne	.Lae
	ldr	r3, [r4, #4]
	cmp	r3, #0
	bge	.L92
	add	r3, r6
.L92:
	asr	r2, r3, #16
	ldr	r3, [r0, #0xc]
	cmp	r3, #0
	bge	.L9c
	add	r3, r6
.L9c:
	asr	r3, #16
	cmp	r2, r3
	bne	.Lae
	ldr	r2, [r4, #8]
	ldr	r3, [r0, #0x10]
	asr	r2, #20
	asr	r3, #20
	cmp	r2, r3
	beq	.Lb6
.Lae:
	add	r5, #1
	cmp	r5, #0x41
	bls	.L80
	mov	r0, #0
.Lb6:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end OvlFunc_883_200806c

@ TryPushBlockOneTile
@ No arguments. The interaction handler for a single-tile pushable block, as
@ opposed to the multi-tile logs the rest of this file deals with.
@
@ Takes the player (slot 0) and steps one tile along the facing from .L6190 to
@ find the block. Then it rejects the push unless all three of these hold:
@   - the tile beyond the block is empty (a second OvlFunc_6c one step further,
@     and if something is there, bit 0 of its byte at +0x59 must be clear),
@   - the tile directly above the block is clear too -- the same lookup with
@     y raised by 0x100000, one height step, so a block cannot be shoved under
@     an overhang,
@   - TestCollision accepts the destination terrain (it returns > 0 to reject) and
@     the block's byte at +0x62 is zero.
@ +0x22 is set to 2 first because TestCollision reads it to choose the collision
@ layer to sample.
@
@ Once committed: animation 8 on the player, a fifteen-frame beat, sound 0xB9,
@ then both the block and the player are given speed 0x3333 in +0x30/+0x34 and
@ sent to the new tile with Actor_TravelTo. Func_ca6c waits out the slide and
@ .gcc2_compiled. ends the looping push cue. The final position is written straight
@ into the block, its velocities at +0x24/+0x2C are cleared, and the player's
@ move targets are reset to the 0x80000000 sentinel before animation 1 returns
@ it to idle.
.thumb_func_start OvlFunc_883_20080c4
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r0, #0
	sub	sp, #0xc
	bl	__MapActor_GetActor
	ldrh	r3, [r0, #6]
	mov	r8, r0
	lsr	r3, #12
	ldr	r0, =.L6190
	lsl	r5, r3, #2
	ldr	r2, =0xffff0000
	ldr	r1, [r0, r5]
	mov	r10, r2
	mov	r3, r10
	mov	r2, r1
	mov	r9, r0
	mov	r0, r8
	and	r2, r3
	ldr	r3, [r0, #8]
	mov	r7, sp
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r0, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r0, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r7
	mov	r1, r8
	bl	OvlFunc_883_200806c
	mov	r6, r0
	cmp	r6, #0
	bne	.L114
	b	.L226
.L114:
	mov	r2, r9
	ldr	r1, [r2, r5]
	mov	r3, r10
	mov	r2, r1
	and	r2, r3
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r7
	mov	r1, r6
	bl	OvlFunc_883_200806c
	cmp	r0, #0
	beq	.L14a
	mov	r3, r0
	add	r3, #0x59
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L226
.L14a:
	ldr	r3, [r6, #8]
	str	r3, [r7]
	mov	r0, #0x80
	ldr	r3, [r6, #0xc]
	lsl	r0, #13
	add	r3, r0
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	mov	r0, r7
	str	r3, [r7, #8]
	mov	r1, r6
	bl	OvlFunc_883_200806c
	cmp	r0, #0
	beq	.L176
	mov	r3, r0
	add	r3, #0x59
	ldrb	r2, [r3]
	mov	r3, #1
	and	r3, r2
	cmp	r3, #0
	bne	.L226
.L176:
	mov	r2, r6
	add	r2, #0x22
	mov	r3, #2
	strb	r3, [r2]
	mov	r2, r9
	ldr	r1, [r2, r5]
	mov	r3, r10
	mov	r2, r1
	and	r2, r3
	ldr	r3, [r6, #8]
	add	r3, r2
	str	r3, [r7]
	ldr	r3, [r6, #0xc]
	str	r3, [r7, #4]
	ldr	r3, [r6, #0x10]
	lsl	r1, #16
	add	r3, r1
	str	r3, [r7, #8]
	mov	r0, r6
	mov	r1, r7
	bl	__TestCollision
	cmp	r0, #0
	bgt	.L226
	mov	r3, r6
	add	r3, #0x62
	ldrb	r3, [r3]
	mov	r10, r3
	cmp	r3, #0
	bne	.L226
	mov	r1, #8
	mov	r0, r8
	bl	__Actor_SetAnim
	ldr	r5, =0x3333
	mov	r0, #0xf
	bl	__WaitFrames
	mov	r0, #0xb9
	bl	__PlaySound
	str	r5, [r6, #0x30]
	str	r5, [r6, #0x34]
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	mov	r0, r6
	bl	__Actor_TravelTo
	mov	r0, r8
	str	r5, [r0, #0x30]
	str	r5, [r0, #0x34]
	ldr	r1, [r7]
	ldr	r2, [r7, #4]
	ldr	r3, [r7, #8]
	bl	__Actor_TravelTo
	mov	r0, r6
	bl	__Actor_WaitMovement
	bl	__Func_809202c
	ldr	r3, [r7]
	str	r3, [r6, #8]
	ldr	r3, [r7, #8]
	mov	r1, r10
	str	r3, [r6, #0x10]
	str	r1, [r6, #0x24]
	str	r1, [r6, #0x2c]
	mov	r3, #0x80
	mov	r2, r8
	lsl	r3, #24
	str	r3, [r2, #0x38]
	str	r3, [r2, #0x40]
	mov	r0, #0xa
	ldrsh	r3, [r2, r0]
	lsl	r3, #16
	str	r1, [r2, #0x24]
	str	r1, [r2, #0x2c]
	str	r3, [r2, #8]
	mov	r1, #0x12
	ldrsh	r3, [r2, r1]
	lsl	r3, #16
	str	r3, [r2, #0x10]
	mov	r0, r8
	mov	r1, #1
	bl	__Actor_SetAnim
.L226:
	add	sp, #0xc
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end OvlFunc_883_20080c4

@ FillMapRectCollisionByte
@ r0 = layer (0..2, anything higher means ewram_10000), r1 = x, r2 = z,
@ r3 = width, [sp+0x0C] = height, [sp+0x10] = the byte to store.
@ Always returns 0.
@
@ Writes one byte into every cell of a rectangle of the map's cell array: the
@ layer base comes from [iwram_1e70] + 0x130 + layer*0x30, rows are 0x200 bytes
@ apart (`lsl #9`) and cells 4 bytes, and the byte written is at offset +2 of
@ the cell -- the collision/attribute byte, not the metatile index.
@
@ This is how a log occupies space. OvlFunc_608 calls it with 0xFF to stamp the
@ footprint solid at the destination and with 0 to clear it at the origin, on
@ both layer 0 and layer 2. A null [iwram_1e70] makes it a no-op.
.thumb_func_start OvlFunc_883_2008244
	push	{r5, r6, lr}
	mov	r4, r3
	ldr	r3, [sp, #0xc]
	mov	r12, r3
	ldr	r3, =iwram_3001e70
	mov	r6, r1
	mov	r1, r2
	ldr	r2, [r3]
	ldr	r5, [sp, #0x10]
	cmp	r2, #0
	beq	.L298
	cmp	r0, #2
	bhi	.L26e
	lsl	r3, r0, #1
	add	r3, r0
	mov	r0, #0x98
	lsl	r0, #1
	lsl	r3, #4
	add	r3, r0
	ldr	r0, [r2, r3]
	b	.L270
.L26e:
	ldr	r0, =gBuffer
.L270:
	lsl	r3, r1, #7
	add	r3, r6, r3
	lsl	r3, #2
	mov	r1, #0
	add	r0, r3
	cmp	r1, r12
	bcs	.L298
.L27e:
	lsl	r3, r1, #9
	mov	r2, #0
	add	r3, r0, r3
	cmp	r2, r4
	bcs	.L292
.L288:
	add	r2, #1
	strb	r5, [r3, #2]
	add	r3, #4
	cmp	r2, r4
	bcc	.L288
.L292:
	add	r1, #1
	cmp	r1, r12
	bcc	.L27e
.L298:
	mov	r0, #0
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end OvlFunc_883_2008244

