	.include "macros.inc"

@ Slot 4: the map object table.
@ Chooses among .L168, .L3e70, .L3ec4, .L3f0c, .L40ec, .L4038, .L4080, .L3fd8, .L3f78, .L3e34
@ on save bits 0x815, 0x87a and the area/entrance id at ewram_240+0x1C0 or +0x1C2.
.thumb_func_start OvlFunc_888_200814c
	push	{lr}
	ldr	r3, =gState
	mov	r2, #0xe1
	lsl	r2, #1
	add	r3, r2
	mov	r2, #0
	ldrsh	r3, [r3, r2]
	sub	r3, #0xa
	cmp	r3, #0x28
	bhi	.L224
	ldr	r2, =.L168
	lsl	r3, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L168:
	.word	.L20c
	.word	.L210
	.word	.L20c
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L214
	.word	.L214
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L21c
	.word	.L224
	.word	.L224
	.word	.L218
	.word	.L224
	.word	.L224
	.word	.L220
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L224
	.word	.L214
.L20c:
	ldr	r0, =.L3e70
	b	.L242
.L210:
	ldr	r0, =.L3ec4
	b	.L242
.L214:
	ldr	r0, =.L3f0c
	b	.L242
.L218:
	ldr	r0, =.L40ec
	b	.L242
.L21c:
	ldr	r0, =.L4038
	b	.L242
.L220:
	ldr	r0, =.L4080
	b	.L242
.L224:
	ldr	r0, =0x87a
	bl	__GetFlag
	cmp	r0, #0
	beq	.L232
	ldr	r0, =.L3fd8
	b	.L242
.L232:
	ldr	r0, =0x815
	bl	__GetFlag
	cmp	r0, #0
	beq	.L240
	ldr	r0, =.L3f78
	b	.L242
.L240:
	ldr	r0, =.L3e34
.L242:
	pop	{r1}
	bx	r1
.func_end OvlFunc_888_200814c
