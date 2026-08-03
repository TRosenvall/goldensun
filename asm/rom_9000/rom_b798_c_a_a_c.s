	.include "macros.inc"
	.include "gba.inc"

@ AddActorPart
@ r0=actor, r1=resource id. Finds the first empty entry in the actor's 4-slot
@ part array (+0x28..+0x34), allocates a part for the id with CreateSpriteLayer and
@ stores it there. Returns the new part, 0 if allocation failed, or -1 if all
@ four slots are already occupied. When this is the actor's first part, the
@ actor-level metrics (+0x20..+0x23, +0x18) are seeded from the resource header
@ exactly as InitSprite does. The part count at +0x27 grows only when the slot
@ filled was the one just past the end.
.thumb_func_start Sprite_AddLayer  @ 0x0800b8ac
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r6, r0
	ldr	r5, [r6, #0x28]
	mov	r8, r1
	mov	r7, #0
	cmp	r5, #0
	beq	.Lb8d0
	mov	r3, r6
	add	r3, #0x28
.Lb8c2:
	add	r7, #1
	cmp	r7, #3
	bgt	.Lb8d0
	add	r3, #4
	ldr	r5, [r3]
	cmp	r5, #0
	bne	.Lb8c2
.Lb8d0:
	cmp	r7, #4
	bne	.Lb8da
	mov	r0, #1
	neg	r0, r0
	b	.Lb932
.Lb8da:
	mov	r0, r8
	bl	CreateSpriteLayer
	mov	r5, r0
	mov	r0, #0
	cmp	r5, #0
	beq	.Lb932
	lsl	r3, r7, #2
	add	r3, #0x28
	mov	r0, r8
	str	r5, [r6, r3]
	bl	_GetSpriteInfo
	mov	r2, #0x27
	add	r2, r6
	ldrb	r3, [r2]
	mov	r12, r2
	cmp	r3, #0
	bne	.Lb924
	ldrb	r2, [r0]
	mov	r3, r6
	add	r3, #0x20
	strb	r2, [r3]
	ldrb	r3, [r0, #1]
	mov	r2, r6
	add	r2, #0x21
	strb	r3, [r2]
	ldrh	r3, [r0, #2]
	lsl	r3, #8
	str	r3, [r6, #0x18]
	ldrb	r3, [r0, #7]
	add	r2, #2
	strb	r3, [r2]
	mov	r1, r6
	ldrb	r3, [r0, #6]
	add	r1, #0x22
	strb	r3, [r1]
.Lb924:
	mov	r2, r12
	ldrb	r3, [r2]
	cmp	r7, r3
	bne	.Lb930
	add	r3, r7, #1
	strb	r3, [r2]
.Lb930:
	mov	r0, r5
.Lb932:
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Sprite_AddLayer

@ RemoveActorPart
@ r0=actor, r1=part pointer. Frees the part with DeleteSpriteLayer, locates it in the
@ actor's 4-slot array and clears that slot. If every slot after the removed
@ one is empty, the part count at +0x27 shrinks to the removed index -- so
@ trailing holes are reclaimed but interior ones are left in place.
.thumb_func_start Sprite_DeleteLayer  @ 0x0800b93c
	push	{r5, r6, lr}
	mov	r6, r0
	mov	r5, r1
	cmp	r6, #0
	beq	.Lb99e
	cmp	r5, #0
	beq	.Lb99e
	mov	r0, r5
	bl	DeleteSpriteLayer
	ldr	r3, [r6, #0x28]
	mov	r0, #0
	cmp	r5, r3
	beq	.Lb96a
	mov	r2, r6
	add	r2, #0x28
.Lb95c:
	add	r0, #1
	cmp	r0, #3
	bhi	.Lb96a
	add	r2, #4
	ldr	r3, [r2]
	cmp	r5, r3
	bne	.Lb95c
.Lb96a:
	cmp	r0, #4
	beq	.Lb99e
	lsl	r3, r0, #2
	mov	r2, #0
	add	r3, #0x28
	str	r2, [r6, r3]
	add	r2, r0, #1
	mov	r4, #0
	cmp	r2, #3
	bhi	.Lb994
	lsl	r3, r2, #2
	add	r3, r6
	mov	r1, r3
	add	r1, #0x28
.Lb986:
	ldmia	r1!, {r3}
	cmp	r3, #0
	beq	.Lb98e
	add	r4, #1
.Lb98e:
	add	r2, #1
	cmp	r2, #3
	bls	.Lb986
.Lb994:
	cmp	r4, #0
	bne	.Lb99e
	mov	r3, r6
	add	r3, #0x27
	strb	r0, [r3]
.Lb99e:
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Sprite_DeleteLayer

@ RemoveActorPartAtIndex
@ r0=actor, r1=slot index (0-3). Same as Sprite_DeleteLayer but addressed by index
@ instead of by pointer: frees the occupant with DeleteSpriteLayer, clears the slot,
@ and shrinks the count at +0x27 when no later slot is still occupied.
.thumb_func_start Sprite_DeleteLayerIndex  @ 0x0800b9a4
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r7, r1
	cmp	r5, #0
	beq	.Lb9ee
	cmp	r7, #3
	bhi	.Lb9ee
	lsl	r3, r7, #2
	mov	r6, r3
	add	r6, #0x28
	ldr	r0, [r5, r6]
	cmp	r0, #0
	beq	.Lb9ee
	bl	DeleteSpriteLayer
	mov	r3, #0
	add	r2, r7, #1
	str	r3, [r5, r6]
	mov	r0, #0
	cmp	r2, #3
	bhi	.Lb9e4
	lsl	r3, r2, #2
	add	r3, r5
	mov	r1, r3
	add	r1, #0x28
.Lb9d6:
	ldmia	r1!, {r3}
	cmp	r3, #0
	beq	.Lb9de
	add	r0, #1
.Lb9de:
	add	r2, #1
	cmp	r2, #3
	bls	.Lb9d6
.Lb9e4:
	cmp	r0, #0
	bne	.Lb9ee
	mov	r3, r5
	add	r3, #0x27
	strb	r7, [r3]
.Lb9ee:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Sprite_DeleteLayerIndex
