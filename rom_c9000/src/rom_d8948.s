	.include "macros.inc"
	.include "gba.inc"

@ PlayD89acImpl_Variant3or4
@ r0=action descriptor. Unlike its sibling wrappers this one chooses at run
@ time: variant 3 when the descriptor's field at +0x18 is zero, variant 4
@ otherwise. That field is set by Func_d6660 when an action has no handler
@ class, so this picks the fallback presentation.
.thumb_func_start Func_d896c
	push	{lr}
	ldr	r3, [r0, #0x18]
	cmp	r3, #0
	bne	.Ld897c
	mov	r1, #3
	bl	Func_d89ac
	b	.Ld8982
.Ld897c:
	mov	r1, #4
	bl	Func_d89ac
.Ld8982:
	pop	{r0}
	bx	r0
.func_end Func_d896c
