	.include "macros.inc"
	.include "gba.inc"

@ Slot 3: the read after slot 4.
@ Chooses among .L6904, .L69c4, .L6b74, .L6c04, .L6c64, .L6cf4, .L68ec
@ on the area/entrance id at ewram_240+0x1C0 or +0x1C2.
@ The result is passed through Func_8b868 first, which tags the records
@ whose position falls inside the active bounds.
.thumb_func_start OvlFunc_968_2008e88
	push	{r5, lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xb5
	cmp	r2, r3
	bne	.Lea0
	ldr	r0, =gScript_945__0200e904
	b	.Ledc
.Lea0:
	ldr	r3, =0xb6
	cmp	r2, r3
	bne	.Leaa
	ldr	r5, =.L69c4
	b	.Led0
.Leaa:
	ldr	r3, =0xb7
	cmp	r2, r3
	bne	.Leb4
	ldr	r5, =.L6b74
	b	.Led0
.Leb4:
	ldr	r3, =0xb8
	cmp	r2, r3
	bne	.Lebe
	ldr	r5, =.L6c04
	b	.Led0
.Lebe:
	ldr	r3, =0xb9
	cmp	r2, r3
	bne	.Lec8
	ldr	r5, =.L6c64
	b	.Led0
.Lec8:
	ldr	r3, =0xba
	cmp	r2, r3
	bne	.Leda
	ldr	r5, =.L6cf4
.Led0:
	mov	r0, r5
	bl	__Func_808b868
	mov	r0, r5
	b	.Ledc
.Leda:
	ldr	r0, =.L68ec
.Ledc:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end OvlFunc_968_2008e88

