	.include "macros.inc"
	.include "gba.inc"

@ SumPartyField
@ Takes no arguments. Returns the sum of byte +0x0F across every active party
@ member, then divides by the party size with Func_af0 -- so this is a PARTY
@ AVERAGE, not a total.
@ The member ids come from the byte list at ewram_240+0x1F8 and the count from
@ Func_795fc.
.thumb_func_start Func_8077348  @ 0x08077348
	push	{r5, r6, r7, lr}
	sub	sp, #4
	bl	GetPartySize
	mov	r7, r0
	mov	r6, #0
	mov	r0, #0
	cmp	r7, #0
	beq	.L77388
	cmp	r6, r7
	bge	.L7737e
	ldr	r3, =gState
	mov	r1, #0xfc
	lsl	r1, #1
	add	r2, r3, r1
	mov	r5, r7
.L77368:
	ldrb	r0, [r2]
	add	r2, #1
	str	r2, [sp]
	bl	GetUnit
	ldrb	r3, [r0, #0xf]
	sub	r5, #1
	add	r6, r3
	ldr	r2, [sp]
	cmp	r5, #0
	bne	.L77368
.L7737e:
	mov	r0, r6
	mov	r1, r7
	bl	__divsi3
	mov	r6, r0
.L77388:
	add	sp, #4
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_8077348

@ GetCombatantRecord
@ r0 = combatant id. Returns its 0x14C-byte record, or 0 for an unknown id.
@     0x00..0x07  ewram_500 + id * 0x14C
@     0x80..0x85  [iwram_1f28] + id * 0x14C - 0xA600, so 0x80 lands at offset 0
@ Enemy ids also return 0 when iwram_1f28 is null, i.e. outside battle.
@ The most-called cross-module function in the ROM -- 226 sites.
.thumb_func_start GetUnit  @ 0x08077394
	push	{lr}
	mov	r3, r14
	ldr	r2, =gPartyStatus
	cmp	r0, #7
	bhi	.L773a8
	mov	r3, #0xa6
	lsl	r3, #1
	mul	r3, r0
	add	r0, r3, r2
	b	.L773c8
.L773a8:
	mov	r3, r0
	sub	r3, #0x80
	cmp	r3, #5
	bhi	.L773c6
	ldr	r3, =iwram_3001f28
	ldr	r2, [r3]
	cmp	r2, #0
	beq	.L773c6
	mov	r3, #0xa6
	lsl	r3, #1
	mul	r3, r0
	add	r3, r2, r3
	ldr	r2, =0xffff5a00
	add	r0, r3, r2
	b	.L773c8
.L773c6:
	mov	r0, #0
.L773c8:
	pop	{r1}
	bx	r1
.func_end GetUnit

