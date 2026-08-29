	.include "macros.inc"

@ GetAbilityRecord
@ r0 = ability id, masked to 0x1FF. Returns .L7b6a8 + id * 0x2C.
@ Ability records are 0x2C bytes. Fields established from the callers below:
@     +0x02  category, classified by Func_78480 into 0 / 1 / 2
@     +0x03  flag byte; bit 4 gates the Func_78588 path
@     +0x04  a bitmask of which classes may use it, tested by Func_7842c
.thumb_func_start Func_78414
	ldr	r3, =0x1ff
	mov	r2, #0x2c
	and	r3, r0
	mov	r0, r3
	mul	r0, r2
	ldr	r3, =.L7b6a8
	add	r0, r3
	bx	lr
.func_end Func_78414

@ CanUseAbility
@ r0 = combatant id, r1 = ability id. Returns 1 when the character's class may
@ use the ability, 0 otherwise.
@ The class index is the byte at record+0x128; ability+4 is a bitmask, and the
@ test is `(mask >> class) & 1`. A class index above 7 always returns 0, so the
@ mask is eight bits wide.
.thumb_func_start Func_7842c
	push	{r5, r6, lr}
	mov	r5, r1
	bl	Func_77394
	mov	r6, r0
	mov	r0, r5
	bl	Func_78414
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
.func_end Func_7842c

@ GetUsableAbilityCategory
@ r0 = combatant id, r1 = ability id. Func_7842c gates the lookup and
@ Func_78480 supplies the category, so an ability the class cannot use reports
@ no category.
.thumb_func_start Func_7845c
	push	{r5, r6, lr}
	mov	r5, r1
	mov	r6, r0
	mov	r0, r5
	bl	Func_78480
	cmp	r0, #0
	bne	.L78470
	mov	r0, #1
	b	.L78478
.L78470:
	mov	r0, r6
	mov	r1, r5
	bl	Func_7842c
.L78478:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_7845c

@ ClassifyAbility
@ r0 = ability id. Maps the record's +0x02 byte to a category:
@     1                   -> 1
@     2, 3, 4, 5 or 9     -> 2
@     anything else       -> 0
.thumb_func_start Func_78480
	push	{lr}
	bl	Func_78414
	ldrb	r3, [r0, #2]
	mov	r2, #0
	cmp	r3, #1
	bne	.L78492
	mov	r2, #1
	b	.L784a8
.L78492:
	cmp	r3, #2
	beq	.L784a6
	cmp	r3, #3
	beq	.L784a6
	cmp	r3, #4
	beq	.L784a6
	cmp	r3, #5
	beq	.L784a6
	cmp	r3, #9
	bne	.L784a8
.L784a6:
	mov	r2, #2
.L784a8:
	mov	r0, r2
	pop	{r1}
	bx	r1
.func_end Func_78480

@ GetInventorySlot
@ r0 = combatant id, r1 = slot (0..14). Reads the halfword at record+0xD8 + 2*slot.
@ THE INVENTORY HALFWORD PACKS THREE FIELDS:
@     bits 0..8   item id      (mask 0x1FF)
@     bit  10     equipped     (Func_78a34 sets it, Func_78a60 clears it)
@     bits 11..15 quantity - 1 (Func_788c4 decrements by 0x800 per unit)
@ Returns (h >> 11) + 1, the quantity, or 0 when the id field is empty.
.thumb_func_start Func_784b0
	push	{r5, lr}
	mov	r5, r1
	bl	Func_77394
	lsl	r5, #1
	add	r5, #0xd8
	ldrh	r0, [r0, r5]
	ldr	r3, =0x1ff
	and	r3, r0
	lsr	r0, #11
	add	r0, #1
	cmp	r3, #0
	bne	.L784cc
	mov	r0, #0
.L784cc:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_784b0

@ CountInventory
@ r0 = combatant id. Returns how many of the 15 inventory slots at record+0xD8
@ hold a non-zero halfword, stopping at the first empty one -- so the inventory
@ is kept COMPACTED, with no holes.
.thumb_func_start Func_784d8
	push	{lr}
	bl	Func_77394
	mov	r3, #0xd8
	ldrh	r3, [r0, r3]
	mov	r2, #0
	cmp	r3, #0
	beq	.L784f8
	add	r0, #0xd8
.L784ea:
	add	r2, #1
	cmp	r2, #0xe
	bgt	.L784f8
	add	r0, #2
	ldrh	r3, [r0]
	cmp	r3, #0
	bne	.L784ea
.L784f8:
	mov	r0, r2
	pop	{r1}
	bx	r1
.func_end Func_784d8

@ CountPartyInventory
@ r0.. = parameters. Sums Func_784d8 across the active party, whose member list
@ Func_796c4 supplies.
.thumb_func_start Func_78500
	push	{r5, r6, r7, lr}
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r3, r2
	ldr	r0, [r3]
	sub	sp, #0x14
	bl	Func_784d8
	cmp	r0, #0xf
	beq	.L7851a
.L78516:
	mov	r0, #1
	b	.L78542
.L7851a:
	mov	r5, sp
	mov	r0, r5
	bl	Func_796c4
	mov	r7, r5
	mov	r6, r0
	mov	r5, #0
	cmp	r5, r6
	bge	.L78540
.L7852c:
	mov	r3, #0
	ldrsh	r0, [r7, r3]
	add	r7, #2
	bl	Func_784d8
	cmp	r0, #0xf
	bne	.L78516
	add	r5, #1
	cmp	r5, r6
	blt	.L7852c
.L78540:
	mov	r0, #0
.L78542:
	add	sp, #0x14
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78500

@ CountPartyInventoryFiltered
@ r0.. = parameters. As Func_78500 with a per-slot filter applied.
.thumb_func_start Func_78550
	push	{r5, r6, r7, lr}
	sub	sp, #0x14
	mov	r5, sp
	mov	r0, r5
	bl	Func_796c4
	mov	r7, #0
	mov	r6, r5
	cmp	r7, r0
	bge	.L7857c
	mov	r5, r0
.L78566:
	mov	r3, #0
	ldrsh	r0, [r6, r3]
	bl	Func_784d8
	sub	r0, r7, r0
	mov	r7, r0
	sub	r5, #1
	add	r6, #2
	add	r7, #0xf
	cmp	r5, #0
	bne	.L78566
.L7857c:
	mov	r0, r7
	add	sp, #0x14
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78550

@ FindUsableItem
@ r0 = combatant id, r1 = item id. Searches the inventory for the item, but only
@ when bit 4 of the ability record's +0x03 is set. Compares against the id field
@ with `(slot ^ wanted) & 0x1FF`, so the quantity and equipped bits are ignored.
.thumb_func_start Func_78588
	push	{r5, r6, lr}
	mov	r5, r1
	bl	Func_77394
	mov	r6, r0
	mov	r0, r5
	bl	Func_78414
	ldrb	r2, [r0, #3]
	mov	r3, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.L785ee
	mov	r3, #0xd8
	ldrh	r3, [r6, r3]
	ldr	r2, =0x1ff
	eor	r3, r5
	and	r3, r2
	mov	r0, #0
	cmp	r3, #0
	beq	.L785ca
	mov	r1, r2
	mov	r2, r6
	add	r2, #0xd8
.L785b8:
	add	r0, #1
	cmp	r0, #0xe
	bgt	.L785ca
	add	r2, #2
	ldrh	r3, [r2]
	eor	r3, r5
	and	r3, r1
	cmp	r3, #0
	bne	.L785b8
.L785ca:
	cmp	r0, #0xf
	beq	.L785ee
	lsl	r3, r0, #1
	mov	r4, r3
	add	r4, #0xd8
	ldrh	r1, [r6, r4]
	lsr	r3, r1, #11
	add	r2, r3, #1
	cmp	r2, #0x1d
	bhi	.L78606
	ldr	r3, =0x7ff
	lsl	r2, #11
	and	r3, r1
	orr	r3, r2
	strh	r3, [r6, r4]
	b	.L7860a
.L785ea:
	strh	r5, [r6, r1]
	b	.L7860a
.L785ee:
	mov	r2, r6
	mov	r0, #0
	add	r2, #0xd8
	mov	r1, #0xd8
.L785f6:
	ldrh	r3, [r2]
	add	r2, #2
	cmp	r3, #0
	beq	.L785ea
	add	r0, #1
	add	r1, #2
	cmp	r0, #0xe
	ble	.L785f6
.L78606:
	mov	r0, #1
	neg	r0, r0
.L7860a:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_78588

@ FindUsableItemInParty
@ r0.. = parameters. Runs Func_78588 across the active party from Func_796c4.
.thumb_func_start Func_78618
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x18
	add	r5, sp, #4
	mov	r8, r0
	mov	r0, r5
	bl	Func_796c4
	mov	r6, #0
	mov	r7, r0
	mov	r3, r5
	cmp	r6, r7
	bge	.L78654
.L78634:
	mov	r2, #0
	ldrsh	r5, [r3, r2]
	mov	r1, r8
	add	r3, #2
	mov	r0, r5
	str	r3, [sp]
	bl	Func_78588
	ldr	r3, [sp]
	cmp	r0, #0
	blt	.L7864e
	mov	r0, r5
	b	.L78658
.L7864e:
	add	r6, #1
	cmp	r6, r7
	blt	.L78634
.L78654:
	mov	r0, #1
	neg	r0, r0
.L78658:
	add	sp, #0x18
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78618

@ FindInventorySlot
@ r0 = combatant id, r1 = item id. Returns the slot index (0..14) holding that
@ item id, or -1 when absent. Matches on the low 9 bits only.
.thumb_func_start Func_78664
	push	{r5, lr}
	mov	r5, r1
	bl	Func_77394
	ldr	r4, =0x1ff
	mov	r1, #0
	add	r0, #0xd8
.L78672:
	ldrh	r2, [r0]
	mov	r3, r4
	and	r3, r2
	add	r0, #2
	cmp	r3, r5
	bne	.L78682
	mov	r0, r1
	b	.L7868c
.L78682:
	add	r1, #1
	cmp	r1, #0xe
	ble	.L78672
	mov	r0, #1
	neg	r0, r0
.L7868c:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_78664

@ FindInventorySlotInParty
@ r0.. = parameters. Runs Func_78664 across the active party, returning both the
@ member and the slot.
.thumb_func_start Func_78698
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =ewram_240
	mov	r2, #0xfa
	lsl	r2, #1
	add	r5, r3, r2
	mov	r8, r0
	mov	r1, r8
	ldr	r0, [r5]
	sub	sp, #0x18
	bl	Func_78664
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	beq	.L786c2
	ldr	r0, [r5]
	b	.L786f8
.L786be:
	mov	r0, r5
	b	.L786f8
.L786c2:
	add	r5, sp, #4
	mov	r0, r5
	bl	Func_796c4
	mov	r6, #0
	mov	r7, r0
	mov	r3, r5
	cmp	r6, r7
	bge	.L786f4
.L786d4:
	mov	r2, #0
	ldrsh	r5, [r3, r2]
	mov	r1, r8
	add	r3, #2
	mov	r0, r5
	str	r3, [sp]
	bl	Func_78664
	mov	r2, #1
	neg	r2, r2
	ldr	r3, [sp]
	cmp	r0, r2
	bne	.L786be
	add	r6, #1
	cmp	r6, r7
	blt	.L786d4
.L786f4:
	mov	r0, #1
	neg	r0, r0
.L786f8:
	add	sp, #0x18
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78698

@ AddInventoryItem
@ r0 = combatant id, r1 = item id. Places the item in a free slot, bumping the
@ quantity field if it is already held, then rebuilds the character summary with
@ Func_77428 and the equipment state with Func_78bf0.
.thumb_func_start Func_78708
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r5, r1
	mov	r10, r0
	bl	Func_77394
	lsl	r5, #1
	mov	r11, r5
	mov	r3, r11
	mov	r7, r0
	add	r3, #0xd8
	ldrh	r5, [r7, r3]
	mov	r0, r10
	mov	r1, r5
	bl	Func_7842c
	cmp	r0, #0
	bne	.L7873c
	mov	r0, #1
	neg	r0, r0
	b	.L787cc
.L7873c:
	mov	r2, #0x80
	lsl	r2, #2
	mov	r3, r5
	and	r3, r2
	mov	r9, r2
	mov	r0, #0
	cmp	r3, #0
	bne	.L787cc
	mov	r0, r5
	bl	Func_78414
	ldrb	r0, [r0, #2]
	mov	r8, r0
	cmp	r0, #6
	beq	.L787a6
	mov	r6, #0
	mov	r5, #0xd8
	b	.L78764
.L78760:
	add	r5, #2
	add	r6, #1
.L78764:
	cmp	r6, #0xe
	bgt	.L7877e
	ldrh	r3, [r5, r7]
	mov	r2, r9
	and	r3, r2
	cmp	r3, #0
	beq	.L78760
	ldrh	r0, [r5, r7]
	bl	Func_78414
	ldrb	r3, [r0, #2]
	cmp	r3, r8
	bne	.L78760
.L7877e:
	cmp	r6, #0xf
	beq	.L787a6
	lsl	r3, r6, #1
	mov	r6, r3
	add	r6, #0xd8
	ldrh	r0, [r7, r6]
	bl	Func_78414
	ldrb	r2, [r0, #3]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L7879e
	mov	r0, #2
	neg	r0, r0
	b	.L787cc
.L7879e:
	ldrh	r2, [r7, r6]
	ldr	r3, =0xfdff
	and	r3, r2
	strh	r3, [r7, r6]
.L787a6:
	mov	r3, r11
	add	r3, #0xd8
	ldrh	r2, [r7, r3]
	ldr	r1, .L787c4	@ 0x200
	orr	r2, r1
	strh	r2, [r7, r3]
	mov	r0, r10
	bl	Func_78bf0
	mov	r0, r10
	bl	Func_77428
	mov	r0, #0
	b	.L787cc

	.align	2, 0
.L787c4:
	.word	0x200
	.pool

.L787cc:
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78708

@ RemoveInventoryItem
@ r0 = combatant id, r1 = slot. Clears the slot and compacts the remainder, so
@ Func_784d8's stop-at-first-empty scan stays correct.
.thumb_func_start Func_787dc
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r8, r1
	bl	Func_77394
	mov	r2, #0x80
	lsl	r2, #2
	mov	r7, r0
	mov	r6, #0
	mov	r5, #0xd8
	mov	r10, r2
	b	.L787fc
.L787f8:
	add	r5, #2
	add	r6, #1
.L787fc:
	cmp	r6, #0xe
	bgt	.L78816
	ldrh	r3, [r5, r7]
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L787f8
	ldrh	r0, [r5, r7]
	bl	Func_78414
	ldrb	r3, [r0, #2]
	cmp	r3, r8
	bne	.L787f8
.L78816:
	cmp	r6, #0xf
	bne	.L7881e
	mov	r6, #1
	neg	r6, r6
.L7881e:
	mov	r0, r6
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_787dc

@ GetItemCategory
@ r0 = item id. Returns the category byte from the ability/item record.
.thumb_func_start Func_7882c
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r2, #0x80
	lsl	r2, #2
	mov	r6, r0
	mov	r8, r1
	mov	r7, #0
	mov	r5, #0xd8
	mov	r10, r2
.L78842:
	ldrh	r3, [r5, r6]
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.L78858
	ldrh	r0, [r5, r6]
	bl	Func_78414
	ldrb	r3, [r0, #2]
	cmp	r3, r8
	beq	.L78862
.L78858:
	add	r7, #1
	add	r5, #2
	cmp	r7, #0xe
	ble	.L78842
	mov	r0, #0
.L78862:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_7882c

@ GetItemProperty
@ r0 = item id, r1 = which. Returns one of the record's property fields.
.thumb_func_start Func_78870
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r2, #0x80
	ldr	r3, =0x1ff
	lsl	r2, #2
	mov	r5, r0
	mov	r7, r1
	mov	r6, #0
	mov	r8, r2
	mov	r10, r3
	add	r5, #0xd8
.L7888a:
	ldrh	r3, [r5]
	mov	r2, r8
	and	r3, r2
	cmp	r3, #0
	beq	.L788a8
	ldrh	r0, [r5]
	bl	Func_78414
	ldrb	r3, [r0, #2]
	cmp	r3, r7
	bne	.L788a8
	ldrh	r3, [r5]
	mov	r0, r10
	and	r0, r3
	b	.L788b2
.L788a8:
	add	r6, #1
	add	r5, #2
	cmp	r6, #0xe
	ble	.L7888a
	mov	r0, #0
.L788b2:
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78870

@ ConsumeInventoryItem
@ r0 = combatant id, r1 = slot. Decrements the quantity field by 0x800 -- one
@ unit -- and clears the slot entirely when the count reaches zero. Returns 1 on
@ success, -1 when the slot was already empty. Rebuilds the summary with
@ Func_77428.
.thumb_func_start Func_788c4
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r7, r0
	bl	Func_77394
	lsl	r5, #1
	add	r5, #0xd8
	ldrh	r3, [r0, r5]
	mov	r6, #1
	neg	r6, r6
	cmp	r3, #0
	beq	.L7893a
	mov	r2, #0xf8
	lsl	r2, #8
	and	r2, r3
	cmp	r2, #0
	beq	.L788f4
	ldr	r2, =0xfffff800
	add	r3, r2
	strh	r3, [r0, r5]
	mov	r6, #1
	b	.L7893a

	.pool_aligned

.L788f4:
	mov	r6, r0
	add	r6, #0xd8
	strh	r2, [r0, r5]
	mov	r4, r6
	mov	r5, #0
	mov	r1, r6
	mov	r0, #0xe
.L78902:
	ldrh	r2, [r4]
	lsl	r3, r2, #16
	add	r4, #2
	cmp	r3, #0
	beq	.L78912
	strh	r2, [r1]
	add	r5, #1
	add	r1, #2
.L78912:
	sub	r0, #1
	cmp	r0, #0
	bge	.L78902
	cmp	r5, #0xe
	bgt	.L78938
	lsl	r3, r5, #1
	add	r0, r3, r6
	ldr	r2, =0
	mov	r3, #0xf
	sub	r5, r3, r5
.L78926:
	sub	r5, #1
	strh	r2, [r0]
	add	r0, #2
	cmp	r5, #0
	bne	.L78926
	b	.L78938

	.pool_aligned

.L78938:
	mov	r6, #2
.L7893a:
	mov	r0, r7
	bl	Func_77428
	mov	r0, r6
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_788c4

@ ConsumeAndNotify
@ r0 = combatant id, r1 = slot. Func_788c4 then Func_78ad0 and _Func_91858 so
@ the field layer learns the item was used.
.thumb_func_start Func_78948
	push	{r5, r6, r7, lr}
	mov	r5, r0
	mov	r6, r1
	bl	Func_77394
	lsl	r3, r6, #1
	add	r3, #0xd8
	ldrh	r7, [r0, r3]
	mov	r1, r6
	mov	r0, r5
	bl	Func_788c4
	mov	r3, #1
	mov	r5, r0
	neg	r3, r3
	cmp	r5, r3
	beq	.L78976
	mov	r1, #1
	mov	r0, r7
	bl	Func_78ad0
	bl	_Func_91858
.L78976:
	mov	r0, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78948

@ SwapInventorySlots
@ r0 = combatant id, r1, r2 = slots. Exchanges two inventory halfwords.
.thumb_func_start Func_78980
	push	{r5, r6, r7, lr}
	mov	r5, r1
	bl	Func_77394
	lsl	r5, #1
	mov	r6, r5
	mov	r7, r0
	add	r6, #0xd8
	ldrh	r3, [r7, r6]
	ldr	r5, =0x1ff
	and	r5, r3
	mov	r0, r5
	bl	Func_78414
	cmp	r5, #0
	bne	.L789a6
	mov	r0, #1
	neg	r0, r0
	b	.L789d2
.L789a6:
	ldrb	r0, [r0, #3]
	mov	r3, #8
	and	r3, r0
	cmp	r3, #0
	beq	.L789b6
	mov	r0, #4
	neg	r0, r0
	b	.L789d2
.L789b6:
	ldrh	r2, [r7, r6]
	mov	r3, #0x80
	lsl	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.L789d0
	mov	r3, #2
	and	r3, r0
	cmp	r3, #0
	beq	.L789d0
	mov	r0, #3
	neg	r0, r0
	b	.L789d2
.L789d0:
	mov	r0, #0
.L789d2:
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_78980

@ FindAndConsumeInParty
@ r0 = item id. Locates the item with Func_78664 / Func_78698 and consumes it
@ with Func_788c4.
.thumb_func_start Func_789dc
	push	{r5, r6, lr}
	mov	r6, r0
	bl	Func_78698
	mov	r3, #1
	mov	r5, r0
	neg	r3, r3
	mov	r0, #0
	cmp	r5, r3
	beq	.L78a02
	mov	r1, r6
	mov	r0, r5
	bl	Func_78664
	mov	r1, r0
	mov	r0, r5
	bl	Func_788c4
	mov	r0, #0
.L78a02:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_789dc

@ FindAndConsumeWithNotify
@ r0 = item id. As Func_789dc but going through Func_78948 so the field layer is
@ notified.
.thumb_func_start Func_78a08
	push	{r5, r6, lr}
	mov	r6, r0
	bl	Func_78698
	mov	r3, #1
	mov	r5, r0
	neg	r3, r3
	mov	r0, #0
	cmp	r5, r3
	beq	.L78a2e
	mov	r1, r6
	mov	r0, r5
	bl	Func_78664
	mov	r1, r0
	mov	r0, r5
	bl	Func_78948
	mov	r0, #0
.L78a2e:
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_78a08

@ EquipInventorySlot
@ r0 = combatant id, r1 = slot. Sets bit 10 (0x400) of the slot's halfword.
@ Returns 0 on success, -1 when the slot is empty.
.thumb_func_start Func_78a34
	push	{r5, lr}
	mov	r5, r1
	bl	Func_77394
	lsl	r5, #1
	add	r5, #0xd8
	ldrh	r2, [r0, r5]
	mov	r3, r2
	cmp	r3, #0
	bne	.L78a4e
	mov	r0, #1
	neg	r0, r0
	b	.L78a56
.L78a4e:
	ldr	r3, =0x400
	orr	r3, r2
	strh	r3, [r0, r5]
	mov	r0, #0
.L78a56:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_78a34

@ UnequipInventorySlot
@ r0 = combatant id, r1 = slot. Clears bit 10 (mask 0xFBFF). Returns 0 on
@ success, -1 when the slot is empty.
.thumb_func_start Func_78a60
	push	{r5, lr}
	mov	r5, r1
	bl	Func_77394
	lsl	r5, #1
	add	r5, #0xd8
	ldrh	r2, [r0, r5]
	mov	r3, r2
	cmp	r3, #0
	bne	.L78a7a
	mov	r0, #1
	neg	r0, r0
	b	.L78a82
.L78a7a:
	ldr	r3, =0xfbff
	and	r3, r2
	strh	r3, [r0, r5]
	mov	r0, #0
.L78a82:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_78a60

	.section .rodata

.L7b6a8:
	.incrom 0x7b6a8, 0x7ee58
