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
