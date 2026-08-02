	.include "macros.inc"

@ CanUseAbility
@ r0 = combatant id, r1 = ability id. Returns 1 when the character's class may
@ use the ability, 0 otherwise.
@ The class index is the byte at record+0x128; ability+4 is a bitmask, and the
@ test is `(mask >> class) & 1`. A class index above 7 always returns 0, so the
@ mask is eight bits wide.
.thumb_func_start CanEquipItem  @ 0x0807842c
	push	{r5, r6, lr}
	mov	r5, r1
	bl	GetUnit
	mov	r6, r0
	mov	r0, r5
	bl	GetItemInfo
	mov	r3, #0x94
	lsl	r3, #1
	add	r6, r3
	ldrb	r3, [r6]
	ldrh	r0, [r0, #4]
	cmp	r3, #7
	bls	.L7844e
	mov	r0, #0
	b	.L78456
.L7844e:
	ldrb	r3, [r6]
	asr	r0, r3
	mov	r3, #1
	and	r0, r3
.L78456:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end CanEquipItem

