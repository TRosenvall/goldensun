	.include "macros.inc"

@ ShareActorTiles
@ r0=destination slot, r1=source slot. Copies the source actor's tile
@ allocation size code (+0x1C) and the low 10 bits of its OAM attr2 (+0x08) --
@ the tile index -- onto the destination actor, so both draw from the same VRAM
@ tiles. Lets a second entity mirror a sprite without a second allocation.
.thumb_func_start Func_8092b54  @ 0x08092b54
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	mov	r8, r0
	mov	r0, r1
	bl	MapActor_GetActor
	ldr	r0, [r0, #0x50]
	ldrb	r6, [r0, #0x1c]
	ldrh	r5, [r0, #8]
	mov	r0, r8
	bl	MapActor_GetActor
	ldr	r0, [r0, #0x50]
	ldr	r3, =0xfffffc00
	ldrh	r2, [r0, #8]
	lsl	r5, #22
	lsr	r5, #22
	and	r3, r2
	orr	r3, r5
	strb	r6, [r0, #0x1c]
	strh	r3, [r0, #8]
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_8092b54
