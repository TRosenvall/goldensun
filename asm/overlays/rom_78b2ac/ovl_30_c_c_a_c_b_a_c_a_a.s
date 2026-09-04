	.include "macros.inc"

@ 154 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
@   SetMapTransition x2
.thumb_func_start OvlFunc_890_2008d9c
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.Ldae
	b	.Leaa
.Ldae:
	ldr	r6, =.L2de4
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.Le0c
	cmp	r5, #2
	bgt	.Ldc4
	cmp	r5, #0
	beq	.Ldd2
	cmp	r5, #1
	beq	.Ldee
	b	.Le8c
.Ldc4:
	cmp	r5, #4
	beq	.Le4c
	cmp	r5, #4
	blt	.Le2c
	cmp	r5, #0x50
	beq	.Le78
	b	.Le8c
.Ldd2:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x21
	bl	__CopyMapTiles
	b	.Le8c
.Ldee:
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x21
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x22
	b	.Le70
.Le0c:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x22
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x23
	b	.Le70
.Le2c:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x23
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x24
	b	.Le70
.Le4c:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x1e
	mov	r3, #0x25
.Le70:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.Le8c
.Le78:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x31
	mov	r2, #0x1e
	mov	r3, #0x21
	bl	__CopyMapTiles
.Le8c:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x5a
	cmp	r5, r3
	bls	.Leaa
	ldr	r3, .Lec8	@ 0
	strh	r3, [r6]
.Leaa:
	ldr	r5, =.L2de8
	ldr	r3, [r5]
	cmp	r3, #0
	beq	.Leec
	cmp	r3, #2
	bne	.Led4
	mov	r0, #0x80
	mov	r1, #0x80
	mov	r2, #0x80
	lsl	r0, #9
	lsl	r1, #9
	lsl	r2, #9
	bl	__Func_8012330
	b	.Lee6

	.align	2, 0
.Lec8:
	.word	0
	.pool

.Led4:
	cmp	r3, #1
	bne	.Lee6
	mov	r0, #1
	mov	r1, #1
	neg	r0, r0
	neg	r1, r1
	ldr	r2, =0xe666
	bl	__Func_8012330
.Lee6:
	ldr	r3, [r5]
	sub	r3, #1
	str	r3, [r5]
.Leec:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end OvlFunc_890_2008d9c

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_2008ef8
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.Lf0a
	b	.L1006
.Lf0a:
	ldr	r6, =.L2ddc
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.Lf68
	cmp	r5, #2
	bgt	.Lf20
	cmp	r5, #0
	beq	.Lf2e
	cmp	r5, #1
	beq	.Lf4a
	b	.Lfe8
.Lf20:
	cmp	r5, #4
	beq	.Lfa8
	cmp	r5, #4
	blt	.Lf88
	cmp	r5, #0x5a
	beq	.Lfd4
	b	.Lfe8
.Lf2e:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x21
	bl	__CopyMapTiles
	b	.Lfe8
.Lf4a:
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x21
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x22
	b	.Lfcc
.Lf68:
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x22
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x23
	b	.Lfcc
.Lf88:
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x23
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x24
	b	.Lfcc
.Lfa8:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x3b
	mov	r2, #0x2a
	mov	r3, #0x25
.Lfcc:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.Lfe8
.Lfd4:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2f
	mov	r1, #0x31
	mov	r2, #0x2a
	mov	r3, #0x21
	bl	__CopyMapTiles
.Lfe8:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x64
	cmp	r5, r3
	bls	.L1006
	ldr	r3, .L1010	@ 0
	strh	r3, [r6]
.L1006:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1010:
	.word	0
.func_end OvlFunc_890_2008ef8

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_200901c
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.L102e
	b	.L112a
.L102e:
	ldr	r6, =.L2de0
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.L108c
	cmp	r5, #2
	bgt	.L1044
	cmp	r5, #0
	beq	.L1052
	cmp	r5, #1
	beq	.L106e
	b	.L110c
.L1044:
	cmp	r5, #4
	beq	.L10cc
	cmp	r5, #4
	blt	.L10ac
	cmp	r5, #0x5f
	beq	.L10f8
	b	.L110c
.L1052:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x24
	bl	__CopyMapTiles
	b	.L110c
.L106e:
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x25
	b	.L10f0
.L108c:
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x25
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x26
	b	.L10f0
.L10ac:
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x26
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x27
	b	.L10f0
.L10cc:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x27
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x3b
	mov	r2, #0x1f
	mov	r3, #0x28
.L10f0:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.L110c
.L10f8:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x30
	mov	r1, #0x31
	mov	r2, #0x1f
	mov	r3, #0x24
	bl	__CopyMapTiles
.L110c:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x69
	cmp	r5, r3
	bls	.L112a
	ldr	r3, .L1134	@ 0
	strh	r3, [r6]
.L112a:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1134:
	.word	0
.func_end OvlFunc_890_200901c

@ 129 instructions. Not one of the recognised overlay shapes,
@ so this is a CALL TRACE rather than a description -- what it does with
@ these is not characterised here.
@
@   Random, PlaySound, CopyMapRectIndicesU x7, Random
.thumb_func_start OvlFunc_890_2009140
	push	{r5, r6, lr}
	sub	sp, #8
	bl	__Random
	mov	r3, #3
	and	r0, r3
	cmp	r0, #0
	bne	.L1152
	b	.L124e
.L1152:
	ldr	r6, =.L2dec
	ldrh	r5, [r6]
	cmp	r5, #2
	beq	.L11b0
	cmp	r5, #2
	bgt	.L1168
	cmp	r5, #0
	beq	.L1176
	cmp	r5, #1
	beq	.L1192
	b	.L1230
.L1168:
	cmp	r5, #4
	beq	.L11f0
	cmp	r5, #4
	blt	.L11d0
	cmp	r5, #0x55
	beq	.L121c
	b	.L1230
.L1176:
	mov	r0, #0xbb
	bl	__PlaySound
	mov	r3, #1
	mov	r2, #5
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x24
	bl	__CopyMapTiles
	b	.L1230
.L1192:
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x24
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x25
	b	.L1214
.L11b0:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x25
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x26
	b	.L1214
.L11d0:
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x26
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x27
	b	.L1214
.L11f0:
	ldr	r2, =.L2de8
	mov	r3, #2
	str	r3, [r2]
	mov	r5, #1
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x27
	str	r5, [sp]
	str	r5, [sp, #4]
	bl	__CopyMapTiles
	mov	r3, #5
	str	r3, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x3b
	mov	r2, #0x29
	mov	r3, #0x28
.L1214:
	str	r5, [sp]
	bl	__CopyMapTiles
	b	.L1230
.L121c:
	mov	r3, #1
	mov	r2, #0xa
	str	r3, [sp]
	str	r2, [sp, #4]
	mov	r0, #0x2e
	mov	r1, #0x31
	mov	r2, #0x29
	mov	r3, #0x24
	bl	__CopyMapTiles
.L1230:
	ldrh	r3, [r6]
	add	r3, #1
	strh	r3, [r6]
	ldrh	r5, [r6]
	bl	__Random
	lsl	r3, r0, #2
	add	r3, r0
	lsl	r3, #3
	lsr	r3, #16
	add	r3, #0x5f
	cmp	r5, r3
	bls	.L124e
	ldr	r3, .L1258	@ 0
	strh	r3, [r6]
.L124e:
	add	sp, #8
	pop	{r5, r6}
	pop	{r0}
	bx	r0

	.align	2, 0
.L1258:
	.word	0
	.pool
.func_end OvlFunc_890_2009140
