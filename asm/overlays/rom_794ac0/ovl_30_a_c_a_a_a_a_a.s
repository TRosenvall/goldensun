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
