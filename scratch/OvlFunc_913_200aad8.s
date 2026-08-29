	.include "macros.inc"

.thumb_func_start OvlFunc_913_200aad8
	push	{lr}
	mov	r0, #0xd
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2b02
	mov	r2, r0
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	ldr	r3, =iwram_3001e40
	ldr	r2, [r3]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	bne	.L2afc
	str	r2, [r0, #0xc]
	b	.L2b02
.L2afc:
	mov	r3, #0xfa
	lsl	r3, #17
	str	r3, [r0, #0xc]
.L2b02:
	mov	r0, #0xe
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2b2a
	mov	r3, r0
	add	r3, #0x55
	mov	r1, #0
	strb	r1, [r3]
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L2b24
	str	r1, [r0, #0xc]
	b	.L2b2a
.L2b24:
	mov	r3, #0xfa
	lsl	r3, #17
	str	r3, [r0, #0xc]
.L2b2a:
	mov	r0, #0xf
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2b52
	mov	r2, r0
	add	r2, #0x55
	mov	r3, #0
	strb	r3, [r2]
	ldr	r3, =iwram_3001e40
	ldr	r2, [r3]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	bne	.L2b4c
	str	r2, [r0, #0xc]
	b	.L2b52
.L2b4c:
	mov	r3, #0xfa
	lsl	r3, #17
	str	r3, [r0, #0xc]
.L2b52:
	mov	r0, #0x10
	bl	__MapActor_GetActor
	cmp	r0, #0
	beq	.L2b7a
	mov	r3, r0
	add	r3, #0x55
	mov	r1, #0
	strb	r1, [r3]
	ldr	r3, =iwram_3001e40
	ldr	r3, [r3]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.L2b74
	str	r1, [r0, #0xc]
	b	.L2b7a
.L2b74:
	mov	r3, #0xfa
	lsl	r3, #17
	str	r3, [r0, #0xc]
.L2b7a:
	pop	{r0}
	bx	r0
.func_end OvlFunc_913_200aad8
