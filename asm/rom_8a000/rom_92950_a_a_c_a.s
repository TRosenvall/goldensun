	.include "macros.inc"

@ PaletteBlinkHook
@ r0=entity. Per-frame hook installed by .gcc2_compiled.. Picks a palette byte from
@ the 4-entry table .L9ed80 indexed by (iwram_1e40 >> 1) & 3 -- the global frame
@ counter, so the cycle runs at half speed over four steps -- and writes it to
@ the palette field (+0x05) of every part of the actor that has an animation
@ table. Sets the actor's dirty flag at +0x25 so the change is picked up.
@ Ignores entities that are not draw kind 1.
.thumb_func_start Func_8092980  @ 0x08092980
	push	{lr}
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L929cc
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	ldr	r0, [r0, #0x50]
	ldr	r1, =.L9ed80
	lsr	r3, #1
	mov	r2, #3
	and	r3, r2
	mov	r12, r0
	ldrb	r4, [r1, r3]
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L929c4
	add	r0, #0x28
	mov	r1, r3
.L929b0:
	ldmia	r0!, {r2}
	cmp	r2, #0
	beq	.L929be
	ldr	r3, [r2, #0x10]
	cmp	r3, #0
	beq	.L929be
	strb	r4, [r2, #5]
.L929be:
	sub	r1, #1
	cmp	r1, #0
	bne	.L929b0
.L929c4:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.L929cc:
	pop	{r0}
	bx	r0
.func_end Func_8092980

@ SetActorPartsPalette
@ r0=entity, r1=palette index. Writes r1 to the palette field (+0x05) of every
@ part of the actor that has an animation table, then sets the dirty flag at
@ +0x25. The static counterpart to Func_92980, and what .gcc2_compiled. calls when
@ no cycling was requested. Ignores entities that are not draw kind 1.
.thumb_func_start Func_80929d8  @ 0x080929d8
	push	{lr}
	mov	r3, r0
	add	r3, #0x54
	ldrb	r2, [r3]
	mov	r3, #0xf
	and	r3, r2
	cmp	r3, #1
	bne	.L92a18
	ldr	r0, [r0, #0x50]
	mov	r12, r0
	mov	r3, r12
	add	r3, #0x27
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.L92a10
	mov	r4, r12
	add	r4, #0x28
	mov	r0, r3
.L929fc:
	ldmia	r4!, {r2}
	cmp	r2, #0
	beq	.L92a0a
	ldr	r3, [r2, #0x10]
	cmp	r3, #0
	beq	.L92a0a
	strb	r1, [r2, #5]
.L92a0a:
	sub	r0, #1
	cmp	r0, #0
	bne	.L929fc
.L92a10:
	mov	r2, r12
	add	r2, #0x25
	mov	r3, #1
	strb	r3, [r2]
.L92a18:
	pop	{r0}
	bx	r0
.func_end Func_80929d8

@ SetSlotChaseTarget
@ r0=slot, r1=packed target -- slot index in the low byte, bit 12 a
@ "keep current speed" flag -- r2=script.
@ Stores the target entity in the chaser's script argument slot at +0x68 and
@ installs the script with _Func_c2d8. Unless bit 12 is set it also matches the
@ chaser to its quarry: the turn rate at +0x64 becomes 0x28, the acceleration
@ at +0x34 is twice the target's, the max speed at +0x30 is copied outright, and
@ the collision flags at +0x59 are cleared so the chaser passes through others.
@ Both slots must resolve or nothing happens.
.thumb_func_start Func_8092a1c  @ 0x08092a1c
	push	{r5, r6, r7, lr}
	mov	r6, r1
	mov	r7, r2
	bl	GetFieldActor
	mov	r5, r0
	mov	r0, #0xff
	and	r0, r6
	bl	GetFieldActor
	cmp	r5, #0
	beq	.L92a6c
	cmp	r0, #0
	beq	.L92a6c
	mov	r3, #0x80
	lsl	r3, #9
	and	r3, r6
	str	r0, [r5, #0x68]
	cmp	r3, #0
	bne	.L92a5e
	mov	r2, r5
	mov	r3, #0x28
	add	r2, #0x64
	strh	r3, [r2]
	ldr	r3, [r0, #0x34]
	lsl	r3, #1
	str	r3, [r5, #0x34]
	ldr	r3, [r0, #0x30]
	ldr	r1, =0
	str	r3, [r5, #0x30]
	mov	r3, r5
	add	r3, #0x59
	strb	r1, [r3]
.L92a5e:
	mov	r0, r5
	mov	r1, r7
	bl	_Actor_SetScript
	b	.L92a6c

	.pool_aligned

.L92a6c:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_8092a1c

