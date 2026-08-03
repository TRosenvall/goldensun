	.include "macros.inc"

@ Slot 3: read after slot 4.
@ Area 0xAA -> .Lba8. Area 0xA9 splits on save bit 0x96F -- .Lc98 once set,
@ .Lc50 before. Anything else -> .Lb90. Note the area tested here (0xA9) is
@ NOT one of the two slot 4 distinguishes (0xAA, 0xAB), so the two slots key on
@ different areas; they are not simply parallel.
.thumb_func_start OvlFunc_963_200808c
	push	{lr}
	ldr	r3, =gState
	mov	r1, #0xe0
	lsl	r1, #1
	add	r3, r1
	mov	r1, #0
	ldrsh	r2, [r3, r1]
	ldr	r3, =0xaa
	cmp	r2, r3
	bne	.La4
	ldr	r0, =.Lba8
	b	.Lbe
.La4:
	ldr	r3, =0xa9
	cmp	r2, r3
	bne	.Lbc
	ldr	r0, =0x96f
	bl	__GetFlag
	cmp	r0, #0
	beq	.Lb8
	ldr	r0, =.Lc98
	b	.Lbe
.Lb8:
	ldr	r0, =gOvl_02008c50
	b	.Lbe
.Lbc:
	ldr	r0, =.Lb90
.Lbe:
	pop	{r1}
	bx	r1
.func_end OvlFunc_963_200808c
