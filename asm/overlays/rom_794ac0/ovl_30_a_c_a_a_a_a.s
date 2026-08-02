	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L5cc8, .L5ab8
@ on the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_899_2008048
	push	{r5, lr}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	cmp	r3, #0x11
	bgt	.L62
	cmp	r3, #0xf
	blt	.L62
	ldr	r5, =.L5cc8
	b	.L64
.L62:
	ldr	r5, =.L5ab8
.L64:
	mov	r0, r5
	bl	__Func_808b868
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_899_2008048

@ 54 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   GetSlotEntityChecked x2, AdvanceDialogue, CopyMapRectAttributes, GetSlotEntityChecked x2
@   AdvanceDialogue, CopyMapRectAttributes
.thumb_func_start OvlFunc_899_2008080
	push	{lr}
	mov	r0, #0
	sub	sp, #8
	bl	__MapActor_GetActor
	mov	r2, #0xa0
	ldrh	r3, [r0, #6]
	lsl	r2, #8
	cmp	r3, r2
	bcc	.Lbe
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xe0
	ldrh	r3, [r0, #6]
	lsl	r2, #8
	cmp	r3, r2
	bhi	.Lbe
	bl	__Func_8093c00
	mov	r3, #0x2a
	mov	r2, #0x55
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x29
	mov	r1, #0x55
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
	b	.Lf6
.Lbe:
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0x80
	ldrh	r3, [r0, #6]
	lsl	r2, #6
	cmp	r3, r2
	bcc	.Lf6
	mov	r0, #0
	bl	__MapActor_GetActor
	mov	r2, #0xc0
	ldrh	r3, [r0, #6]
	lsl	r2, #7
	cmp	r3, r2
	bhi	.Lf6
	bl	__Func_8093c00
	mov	r3, #0x2a
	mov	r2, #0x55
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2b
	mov	r1, #0x55
	mov	r2, #1
	mov	r3, #1
	bl	__Func_8010704
.Lf6:
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_899_2008080

