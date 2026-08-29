	.include "macros.inc"
	.include "gba.inc"

@ SpawnEffectActors
@ r0=count, r1=actor resource id, r2=OBJ priority (only bits 0-1 are used).
@ Creates `count` actors from one resource and fills the effect-actor array at
@ [iwram_1eec]+0x77D8 with them, one word each. Each actor gets:
@     +0x26 = 0                       (u8, cleared)
@     animation index i               via _Func_ba30, so actor i plays frame i
@     +0x09 bits 3:2 = r2 & 3         (OBJ priority), other bits preserved
@ A count of 0 does nothing. A failed _Func_bc70 leaves 0 in the slot and the
@ loop continues, so callers must null-check the array.
@
@ This is the standard way every animation class stocks its particle/effect
@ actors -- `CreateSummonSprite(16, resId, 2)` means "sixteen copies of resId, each
@ showing its own animation frame, at priority 2".
.thumb_func_start CreateSummonSprite  @ 0x080dbb24
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_3001eec
	mov	r8, r0
	mov	r9, r1
	ldr	r7, [r3]
	mov	r6, #0
	cmp	r0, #0
	beq	.Ldbb80
	mov	r3, #3
	and	r2, r3
	mov	r1, #0xd
	lsl	r2, #2
	neg	r1, r1
	ldr	r5, =0x77d8
	mov	r10, r2
	mov	r11, r1
.Ldbb50:
	mov	r0, r9
	bl	_CreateSprite
	str	r0, [r5, r7]
	cmp	r0, #0
	beq	.Ldbb78
	mov	r2, r0
	add	r2, #0x26
	mov	r3, #0
	strb	r3, [r2]
	mov	r1, r6
	bl	_Sprite_SetAnim
	ldr	r2, [r5, r7]
	ldrb	r3, [r2, #9]
	mov	r1, r11
	and	r3, r1
	mov	r1, r10
	orr	r3, r1
	strb	r3, [r2, #9]
.Ldbb78:
	add	r6, #1
	add	r5, #4
	cmp	r6, r8
	bne	.Ldbb50
.Ldbb80:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end CreateSummonSprite

