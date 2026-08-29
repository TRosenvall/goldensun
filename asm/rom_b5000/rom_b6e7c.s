	.include "macros.inc"

@ LookupSpriteVariant
@ r0 = combatant/resource id. Searches the halfword table at .Lc593c for an
@ entry whose LOW 9 BITS match, and returns that entry's high 7 bits (>> 9) as
@ the variant. The table ends at a halfword of -1; an id that is not listed
@ returns the default variant 6.
.thumb_func_start GetWeaponType  @ 0x080b6e7c
	push	{r5, lr}
	mov	r5, r0
	mov	r4, #0
	ldr	r0, =.Lc593c
.Lb6e84:
	lsl	r1, r4, #1
	ldrh	r2, [r0, r1]
	ldr	r3, =0x1ff
	and	r3, r2
	cmp	r5, r3
	bne	.Lb6e96
	ldrh	r3, [r0, r1]
	lsr	r0, r3, #9
	b	.Lb6ea6
.Lb6e96:
	lsl	r3, r2, #16
	mov	r2, #1
	asr	r3, #16
	neg	r2, r2
	add	r4, #1
	cmp	r3, r2
	bne	.Lb6e84
	mov	r0, #6
.Lb6ea6:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end GetWeaponType

	.section .rodata

.Lc593c:
	.incrom 0xc593c, 0xc59a4
