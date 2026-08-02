	.include "macros.inc"

@ Map edit: 1 attribute copy.
@ Attributes only, so the artwork is already correct and only
@ collision or priority changes.
@ Records it with save bit 0x210.
.thumb_func_start OvlFunc_882_200810c
	push	{lr}
	mov	r0, #0x84
	lsl	r0, #2
	sub	sp, #8
	bl	__SetFlag
	mov	r3, #0xa
	mov	r2, #0x54
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x28
	mov	r1, #0x54
	mov	r2, #7
	mov	r3, #4
	bl	__Func_8010704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_200810c

@ Map edit: 1 attribute copy.
@ Attributes only, so the artwork is already correct and only
@ collision or priority changes.
@ Clears save bit 0x210.
.thumb_func_start OvlFunc_882_2008134
	push	{lr}
	mov	r0, #0x84
	lsl	r0, #2
	sub	sp, #8
	bl	__ClearFlag
	mov	r3, #0xa
	mov	r2, #0x54
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x28
	mov	r1, #0x59
	mov	r2, #7
	mov	r3, #4
	bl	__Func_8010704
	add	sp, #8
	pop	{r0}
	bx	r0
.func_end OvlFunc_882_2008134

