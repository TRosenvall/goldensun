	.include "macros.inc"
	.include "gba.inc"

@ CountInventory
@ r0 = character id. Returns how many of the fifteen inventory halfwords at
@ [record]+0xD8 have a non-zero item id in bits 0..8.
@
@ The inventory slot format, established here and in Func_a3d9c, Func_a3ddc and
@ Func_a40ac, is one halfword per slot:
@
@     bits 0..8    the item id -- the same 0x1FF _Func_78414 masks with
@     bit 9        set means the slot is locked (equipped, or a key item)
@     bits 11..15  the quantity, less one
@
@ which is why rom_77000's _Func_788c4 decrements by 0x800: that is one unit.
.thumb_func_start Func_80a3d6c  @ 0x080a3d6c
	push	{r5, lr}
	bl	_GetUnit
	ldr	r4, =0x1ff
	mov	r5, #0
	add	r0, #0xd8
	mov	r1, #0xe
.La3d7a:
	ldrh	r2, [r0]
	mov	r3, r4
	and	r3, r2
	add	r0, #2
	cmp	r3, #0
	beq	.La3d88
	add	r5, #1
.La3d88:
	sub	r1, #1
	cmp	r1, #0
	bge	.La3d7a
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_80a3d6c

@ FindInventoryItem
@ r0 = character id, r1 = item id. Scans the fifteen inventory slots and returns
@ the held quantity -- (slot >> 11) + 1 -- for the first match, or 0 when the
@ character does not have it. Stops at the first empty slot.
.thumb_func_start Func_80a3d9c  @ 0x080a3d9c
	push	{r5, r6, lr}
	mov	r6, r1
	bl	_GetUnit
	ldr	r4, =0x1ff
	mov	r5, #0
	mov	r1, #0
	add	r0, #0xd8
.La3dac:
	ldrh	r2, [r0]
	mov	r3, r2
	add	r0, #2
	cmp	r3, #0
	beq	.La3dca
	mov	r3, r4
	and	r3, r2
	cmp	r3, r6
	bne	.La3dca
	mov	r3, #0xf8
	lsl	r3, #8
	and	r3, r2
	lsr	r5, r3, #11
	add	r5, #1
	b	.La3dd0
.La3dca:
	add	r1, #1
	cmp	r1, #0xe
	ble	.La3dac
.La3dd0:
	mov	r0, r5
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80a3d9c

@ CompactInventory
@ r0 = character record, r1 = destination, r2 = unused. Zeroes 16 destination
@ halfwords, then copies the non-empty inventory slots down into the front of
@ it, preserving order. Returns the count. This is what turns the sparse record
@ into the dense list every menu draws.
.thumb_func_start Func_80a3ddc  @ 0x080a3ddc
	push	{r5, r6, r7, lr}
	mov	r5, r1
	mov	r3, r5
	ldr	r2, =0
	add	r3, #0x3e
	mov	r12, r5
.La3de8:
	strh	r2, [r3]
	sub	r3, #2
	cmp	r3, r12
	bge	.La3de8
	ldr	r3, =0
	mov	r7, #0
	mov	r12, r3
	add	r0, #0xd8
	mov	r6, #0
	mov	r4, r5
	mov	r1, #0xe
.La3dfe:
	mov	r3, r12
	strh	r3, [r6, r5]
	ldrh	r2, [r0]
	mov	r3, r2
	add	r0, #2
	cmp	r3, #0
	beq	.La3e18
	strh	r2, [r4]
	add	r7, #1
	add	r4, #2
	b	.La3e18

	.pool_aligned

.La3e18:
	sub	r1, #1
	add	r6, #2
	cmp	r1, #0
	bge	.La3dfe
	mov	r0, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80a3ddc
