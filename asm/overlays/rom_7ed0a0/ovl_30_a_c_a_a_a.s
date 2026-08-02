	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L38f4, .L3a74
@ on the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_964_20092e0
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xac
	cmp	r2, r3
	bne	.L12f8
	ldr	r5, =gScript_925__0200b8f4
	b	.L12fa
.L12f8:
	ldr	r5, =.L3a74
.L12fa:
	mov	r0, r5
	bl	__Func_808b868
	mov	r0, r5
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_964_20092e0

