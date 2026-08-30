	.include "macros.inc"

@ GetSpriteTableFlag
@ r0 = index. Returns bit 0 of the byte at entry+2, or 0 for an out-of-range
@ index.
.thumb_func_start Func_80c23c0  @ 0x080c23c0
	push	{lr}
	cmp	r0, #0xab
	bls	.Lc23ca
	mov	r0, #0
	b	.Lc23de
.Lc23ca:
	ldr	r3, =.Lc7420
	lsl	r2, r0, #3
	add	r2, r3
	ldrb	r3, [r2, #2]
	lsl	r3, #31
	mov	r1, #0
	cmp	r3, #0
	beq	.Lc23dc
	mov	r1, #1
.Lc23dc:
	mov	r0, r1
.Lc23de:
	pop	{r1}
	bx	r1
.func_end Func_80c23c0

@ GetSpriteTableField3
@ r0 = index. Returns a further field of the entry.
.thumb_func_start Func_80c23e8  @ 0x080c23e8
	push	{lr}
	cmp	r0, #0xab
	bls	.Lc23f2
	mov	r0, #1
	b	.Lc2406
.Lc23f2:
	ldr	r3, =.Lc7420
	lsl	r2, r0, #3
	add	r2, r3
	ldrb	r3, [r2, #2]
	lsl	r3, #27
	lsr	r3, #28
	mov	r0, r3
	cmp	r3, #0
	bne	.Lc2406
	mov	r0, #1
.Lc2406:
	pop	{r1}
	bx	r1
.func_end Func_80c23e8

@ GetSpriteTableField4
@ r0 = index. Another entry field. Exported.
.thumb_func_start GetEnemyAttackAnimParam  @ 0x080c2410
	push	{lr}
	cmp	r0, #0xab
	bls	.Lc241a
	mov	r0, #0
	b	.Lc242c
.Lc241a:
	ldr	r3, =.Lc7420
	lsl	r2, r0, #3
	add	r2, r3
	ldrb	r3, [r2, #2]
	lsr	r3, #5
	mov	r0, r3
	cmp	r3, #0
	bne	.Lc242c
	mov	r0, #0
.Lc242c:
	pop	{r1}
	bx	r1
.func_end GetEnemyAttackAnimParam
