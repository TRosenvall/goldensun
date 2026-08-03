	.include "macros.inc"

@ Leaf helper, 23 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_959_2008a34
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xa1
	cmp	r2, r3
	bne	.La4c
	ldr	r0, =.L6910
	b	.La5e
.La4c:
	ldr	r3, =0xa2
	cmp	r2, r3
	beq	.La58
	ldr	r3, =0xa3
	cmp	r2, r3
	bne	.La5c
.La58:
	ldr	r0, =.L697c
	b	.La5e
.La5c:
	ldr	r0, =.L68a4
.La5e:
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2008a34

@ Leaf helper, 35 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: ewram_240
.thumb_func_start OvlFunc_959_2008a80
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x6a
	cmp	r2, r3
	bne	.La98
	ldr	r0, =.L69d0
	b	.Lac2
.La98:
	ldr	r3, =0xa2
	cmp	r2, r3
	bne	.Laa2
	ldr	r0, =.L6e08
	b	.Lac2
.Laa2:
	ldr	r3, =0xa1
	cmp	r2, r3
	bne	.Laac
	ldr	r0, =.L6c28
	b	.Lac2
.Laac:
	ldr	r3, =0xa0
	cmp	r2, r3
	bne	.Lab6
	ldr	r0, =.L6ac0
	b	.Lac2
.Lab6:
	ldr	r3, =0xa3
	cmp	r2, r3
	bne	.Lac0
	ldr	r0, =.L6e98
	b	.Lac2
.Lac0:
	ldr	r0, =.L69b8
.Lac2:
	pop	{r1}
	bx	r1
.func_end OvlFunc_959_2008a80
