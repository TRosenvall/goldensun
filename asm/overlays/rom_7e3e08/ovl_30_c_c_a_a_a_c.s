	.include "macros.inc"
	.include "gba.inc"

@ Leaf helper, 44 instructions, calls nothing.
@ Described by what it touches, not by what it means.
@ Globals: REG_BLDALPHA, REG_BLDCNT, ewram_1004
.thumb_func_start OvlFunc_957_2008a54
	push	{lr}
	ldr	r3, =ewram_2001004
	ldrb	r2, [r3]
	ldr	r1, =0x3f42
	ldr	r3, =REG_BLDCNT
	strh	r1, [r3]
	lsl	r2, #24
	asr	r2, #24
	cmp	r2, #0
	bne	.La70
	mov	r2, #0x80
	lsl	r2, #5
	add	r3, #2
	b	.Laa6
.La70:
	cmp	r2, #1
	bne	.La7c
	mov	r2, #0xe0
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #4
	b	.Laa6
.La7c:
	cmp	r2, #2
	bne	.La88
	mov	r2, #0xc0
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #4
	b	.Laa6
.La88:
	cmp	r2, #3
	bne	.La94
	mov	r2, #0xa0
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #4
	b	.Laa6
.La94:
	cmp	r2, #4
	bne	.Laa0
	mov	r2, #0x80
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #4
	b	.Laa6
.Laa0:
	mov	r2, #0xc0
	ldr	r3, =REG_BLDALPHA
	lsl	r2, #3
.Laa6:
	strh	r2, [r3]
	pop	{r0}
	bx	r0
.func_end OvlFunc_957_2008a54
