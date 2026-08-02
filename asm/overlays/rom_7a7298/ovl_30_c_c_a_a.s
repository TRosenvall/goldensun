	.include "macros.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L2ad0, .L2a58, .L29e0
@ on save bit 0x881 and the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_921_200816c
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0x33
	cmp	r2, r3
	bne	.L1b0
	ldr	r5, =.L2ad0
	mov	r0, r5
	bl	__Func_808b868
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1ac
	mov	r3, #0x83
	lsl	r3, #1
	add	r2, r5, r3
	mov	r3, #0
	strb	r3, [r2]
	mov	r3, #0xb6
	lsl	r3, #16
	str	r3, [r5, #0x50]
	mov	r3, #0x8d
	lsl	r3, #18
	str	r3, [r5, #0x58]
	mov	r3, #2
	str	r3, [r5, #0x4c]
.L1ac:
	mov	r0, r5
	b	.L1c0
.L1b0:
	ldr	r0, =0x881
	bl	__GetFlag
	cmp	r0, #0
	beq	.L1be
	ldr	r0, =gOvl_0200aa58
	b	.L1c0
.L1be:
	ldr	r0, =.L29e0
.L1c0:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_921_200816c

