	.include "macros.inc"

@ ComputeHitChance
@ r0.. = parameters. Pure arithmetic over the attacker and defender stats; no
@ calls out. 114 lines, traced structurally.
.thumb_func_start Func_8079d7c  @ 0x08079d7c
	push	{lr}
	sub	r0, #8
	cmp	r0, #0x31
	bls	.L79d86
	b	.L79e90
.L79d86:
	ldr	r2, =.L79d90
	lsl	r3, r0, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.L79d90:
	.word	.L79e8c
	.word	.L79e8c
	.word	.L79e90
	.word	.L79e90
	.word	.L79e58
	.word	.L79e58
	.word	.L79e90
	.word	.L79e90
	.word	.L79e5c
	.word	.L79e5c
	.word	.L79e6c
	.word	.L79e6c
	.word	.L79e78
	.word	.L79e7c
	.word	.L79e60
	.word	.L79e64
	.word	.L79e68
	.word	.L79e6c
	.word	.L79e70
	.word	.L79e74
	.word	.L79e8c
	.word	.L79e90
	.word	.L79e90
	.word	.L79e78
	.word	.L79e8c
	.word	.L79e90
	.word	.L79e7c
	.word	.L79e80
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e90
	.word	.L79e84
	.word	.L79e88
.L79e58:
	mov	r0, #0x46
	b	.L79e94
.L79e5c:
	mov	r0, #0x4b
	b	.L79e94
.L79e60:
	mov	r0, #0x1e
	b	.L79e94
.L79e64:
	mov	r0, #0x28
	b	.L79e94
.L79e68:
	mov	r0, #0x2d
	b	.L79e94
.L79e6c:
	mov	r0, #0x37
	b	.L79e94
.L79e70:
	mov	r0, #0x19
	b	.L79e94
.L79e74:
	mov	r0, #0x14
	b	.L79e94
.L79e78:
	mov	r0, #0x41
	b	.L79e94
.L79e7c:
	mov	r0, #0x23
	b	.L79e94
.L79e80:
	mov	r0, #0x32
	b	.L79e94
.L79e84:
	mov	r0, #0x3c
	b	.L79e92
.L79e88:
	mov	r0, #0x5a
	b	.L79e92
.L79e8c:
	mov	r0, #0x3c
	b	.L79e94
.L79e90:
	mov	r0, #0x64
.L79e92:
	neg	r0, r0
.L79e94:
	pop	{r1}
	bx	r1
.func_end Func_8079d7c

@ GetEffectForItem
@ r0 = item id. Resolves the item record (Func_773d8) to its effect record
@ (Func_79ad8).
.thumb_func_start Func_8079e9c  @ 0x08079e9c
	push	{r5, lr}
	mov	r5, r1
	ldr	r1, =0x129
	mov	r2, r0
	add	r3, r2, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	bne	.L79eca
	sub	r1, #1
	add	r3, r2, r1
	ldrb	r0, [r3]
	bl	GetEnemyInfo
	mov	r2, #0
	add	r0, #0x48
.L79eba:
	ldrb	r3, [r0]
	cmp	r3, r5
	beq	.L79ee0
	add	r2, #1
	add	r0, #1
	cmp	r2, #2
	ble	.L79eba
	b	.L79eea
.L79eca:
	ldr	r1, =0x129
	add	r3, r2, r1
	ldrb	r0, [r3]
	bl	GetClassInfo
	mov	r2, #0
	add	r0, #0x50
.L79ed8:
	ldrb	r3, [r0]
	add	r0, #1
	cmp	r3, r5
	bne	.L79ee4
.L79ee0:
	mov	r0, #1
	b	.L79eec
.L79ee4:
	add	r2, #1
	cmp	r2, #2
	ble	.L79ed8
.L79eea:
	mov	r0, #0
.L79eec:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_8079e9c
