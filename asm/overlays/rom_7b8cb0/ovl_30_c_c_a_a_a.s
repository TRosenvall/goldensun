	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L140c, .L15bc, .L13f4
@ on save bits 0x8fd, 0x8fe, 0x907, 0x909 and the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_931_200807c
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x4b
	cmp	r2, r3
	bne	.Lac
	ldr	r0, =0x909
	bl	__GetFlag
	cmp	r0, #0
	beq	.La8
	ldr	r3, =.L140c
	mov	r1, r3
	mov	r2, #0
	add	r1, #0x8e
	add	r3, #0xa6
	strb	r2, [r1]
	strb	r2, [r3]
.La8:
	ldr	r0, =.L140c
	b	.Lee
.Lac:
	ldr	r3, =0x4c
	cmp	r2, r3
	bne	.Lec
	ldr	r0, =0x8fd
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lc4
	ldr	r3, =.L15bc
	mov	r2, #1
	add	r3, #0x2e
	strb	r2, [r3]
.Lc4:
	ldr	r0, =0x8fe
	bl	__GetFlag
	cmp	r0, #0
	bne	.Ld8
	ldr	r0, =0x907
	bl	__GetFlag
	cmp	r0, #0
	beq	.Le0
.Ld8:
	ldr	r3, =.L15bc
	mov	r2, #1
	add	r3, #0x5e
	strb	r2, [r3]
.Le0:
	ldr	r5, =.L15bc
	mov	r0, r5
	bl	__Func_808b868
	mov	r0, r5
	b	.Lee
.Lec:
	ldr	r0, =.L13f4
.Lee:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_931_200807c
