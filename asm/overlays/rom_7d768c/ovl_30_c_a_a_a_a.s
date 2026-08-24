	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L4b3c, .L4e6c, .L4d64, .L4b84
@ on save bits 0x950, 0x962 and the area/entrance id at ewram_240+0x1C0 or +0x1C2.
.thumb_func_start OvlFunc_952_2008070
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x8b
	cmp	r2, r3
	bne	.L88
	ldr	r0, =.L4b3c
	b	.La8
.L88:
	mov	r0, #0x95
	lsl	r0, #4
	bl	__GetFlag
	cmp	r0, #0
	beq	.L98
	ldr	r0, =.L4e6c
	b	.La8
.L98:
	ldr	r0, =0x962
	bl	__GetFlag
	cmp	r0, #0
	beq	.La6
	ldr	r0, =.L4d64
	b	.La8
.La6:
	ldr	r0, =.L4b84
.La8:
	pop	{r1}
	bx	r1
.func_end OvlFunc_952_2008070
