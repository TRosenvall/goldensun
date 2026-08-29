	.include "macros.inc"

@ SetFollowerFormationScript
@ r0=party slot, r1=follower count (clamped to 1..3). Resolves the slot to an
@ entity with Func_8ba1c and installs one of four canned follower scripts from
@ .L9ebfc via _Func_c2d8 (the rom_9000 entity API). The script chosen is
@ .L9ebfc + (3 - count) * 0x80, so a smaller party gets a tighter formation.
@ Does nothing if the slot has no entity or the count is not positive.
.thumb_func_start Func_9259c
	push	{r5, lr}
	mov	r5, r1
	bl	Func_8ba1c
	cmp	r0, #0
	beq	.L925c0
	cmp	r5, #0
	ble	.L925c0
	cmp	r5, #3
	ble	.L925b2
	mov	r5, #3
.L925b2:
	mov	r1, #3
	sub	r1, r5
	ldr	r3, =.L9ebfc
	lsl	r1, #7
	add	r1, r3
	bl	_Func_c2d8
.L925c0:
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_9259c

@ SetFollowerFormationAndRefresh
@ r0=party slot, r1=follower count. Func_9259c followed by Func_920e8 on the
@ same slot, so the new formation script takes effect immediately rather than
@ at the follower's next update.
.thumb_func_start Func_925cc
	push	{r5, lr}
	mov	r5, r0
	bl	Func_9259c
	mov	r0, r5
	bl	Func_920e8
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_925cc

	.section .rodata

@ .L9ebfc -- four 0x80-byte follower formation scripts, indexed by
@ (3 - follower count) in Func_9259c.
.L9ebfc:
	.incrom 0x9ebfc, 0x9ed80
