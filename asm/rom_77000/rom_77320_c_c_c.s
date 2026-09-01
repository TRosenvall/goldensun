	.include "macros.inc"
	.include "gba.inc"

@ ChangeHp
@ r0 = combatant id, r1 = SIGNED delta. Adds the delta to current HP at +0x38,
@ clamps to 0..maxHP (+0x34), recomputes the fraction with Func_7822c, and
@ returns the new HP.
@ This is the damage and healing entry point -- rom_b5000 calls it as
@ _Func_783a4(id, -damage). Reaching 0 is how a combatant goes down.
.thumb_func_start ModifyHP  @ 0x080783a4
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	GetUnit
	mov	r6, r0
	mov	r1, #0x38
	ldrsh	r3, [r6, r1]
	mov	r1, #0x34
	ldrsh	r2, [r6, r1]
	add	r3, r5
	mov	r1, r2
	cmp	r3, r2
	bgt	.L783c8
	mov	r1, #0
	cmp	r3, #0
	blt	.L783c8
	mov	r1, r3
.L783c8:
	mov	r0, r7
	strh	r1, [r6, #0x38]
	bl	UpdateStatBarPercent
	mov	r2, #0x38
	ldrsh	r0, [r6, r2]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end ModifyHP

@ ChangePp
@ r0 = combatant id, r1 = SIGNED delta. The ModifyHP counterpart for PP:
@ current at +0x3A, maximum at +0x36, same clamp and the same Func_7822c
@ refresh, returning the new PP.
.thumb_func_start ModifyPP  @ 0x080783dc
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	GetUnit
	mov	r6, r0
	mov	r1, #0x3a
	ldrsh	r3, [r6, r1]
	mov	r1, #0x36
	ldrsh	r2, [r6, r1]
	add	r3, r5
	mov	r1, r2
	cmp	r3, r2
	bgt	.L78400
	mov	r1, #0
	cmp	r3, #0
	blt	.L78400
	mov	r1, r3
.L78400:
	mov	r0, r7
	strh	r1, [r6, #0x3a]
	bl	UpdateStatBarPercent
	mov	r2, #0x3a
	ldrsh	r0, [r6, r2]
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end ModifyPP

	.section .rodata
	.global .L7a828

.L7a828:
	.incrom 0x7a828, 0x7a830
