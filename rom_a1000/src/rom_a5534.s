	.include "macros.inc"

@ ReserveItemArrowTiles
@ Takes no arguments. The Func_ae88c of this file: reserves two OBJ slots and
@ loads 0x80 bytes into each -- .Laebcc into state+0x392 and .Laeb4c into
@ state+0x394. Different graphics from Func_ae88c's, same two slots.
.thumb_func_start Func_a5534
	push	{r5, lr}
	ldr	r3, =iwram_1f2c
	ldr	r5, [r3]
	bl	Func_4080
	ldr	r2, =0x392
	add	r3, r5, r2
	strh	r0, [r3]
	ldr	r2, =.Laebcc
	mov	r1, #0x80
	bl	Func_3fa4
	bl	Func_4080
	mov	r3, #0xe5
	lsl	r3, #2
	add	r5, r3
	strh	r0, [r5]
	ldr	r2, =.Laeb4c
	mov	r1, #0x80
	bl	Func_3fa4
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_a5534

@ BuildAbilityScrollState
@ r0 = destination, r1 = which cursor.
@
@ Fills the seven-word scroll descriptor every list renderer in this module
@ consumes. The five-row page size is baked in as a literal divisor:
@
@     [0x00] the character record from _Func_77394
@     [0x08] index / 5          the page
@     [0x0C] total / 5 rounded up   the page count
@     [0x10] index % 5          the row inside the page
@     [0x14] total
@     [0x18] index, clamped to total - 1
@
@ Returns 1 always. Word [0x04] is left untouched.
.thumb_func_start Func_a5578
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r2, #0x86
	mov	r8, r3
	lsl	r2, #2
	mov	r5, r1
	mov	r6, r8
	add	r5, r2
	add	r6, #2
	mov	r10, r0
	ldrb	r0, [r6, r5]
	bl	Func_a3d6c
	mov	r7, r0
	ldrb	r0, [r6, r5]
	bl	_Func_77394
	ldrb	r3, [r6, r5]
	mov	r2, #0x98
	lsl	r2, #2
	add	r3, r2
	mov	r2, r8
	ldrsb	r6, [r2, r3]
	add	r3, r6, #1
	mov	r11, r0
	cmp	r3, r7
	ble	.La55be
	sub	r6, r7, #1
.La55be:
	mov	r1, #5
	mov	r0, r6
	bl	Func_af0_from_thumb
	mov	r1, #5
	mov	r9, r0
	mov	r0, r6
	bl	Func_b1c_from_thumb
	mov	r1, #5
	mov	r8, r0
	mov	r0, r7
	bl	Func_af0_from_thumb
	mov	r1, #5
	mov	r5, r0
	mov	r0, r7
	bl	Func_b1c_from_thumb
	cmp	r0, #0
	beq	.La55ea
	add	r5, #1
.La55ea:
	mov	r2, r10
	mov	r3, r11
	str	r3, [r2]
	mov	r3, r9
	str	r3, [r2, #8]
	mov	r3, r8
	str	r5, [r2, #0xc]
	str	r3, [r2, #0x10]
	str	r7, [r2, #0x14]
	str	r6, [r2, #0x18]
	mov	r0, #1
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a5578

@ DrawAbilityDetail
@ r0, r1 = unused, r2 = scroll descriptor. Recomputes the absolute index as
@ page * 5 + row and stores it back at [r2+0x18], releases the bottom window's
@ nodes and prints the highlighted ability's description -- STRING 0x75 + id,
@ the ability description base -- at its origin.
@
@ It then tints the five list rows through Func_a2268: palette 0x0E for the row
@ matching [r2+0x10] and 0x0F for the rest. That tint IS the selection
@ highlight; nothing is redrawn.
.thumb_func_start Func_a5614
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r8, r2
	ldr	r3, =iwram_1f2c
	ldr	r2, [r2, #8]
	mov	r1, r8
	ldr	r7, [r3]
	lsl	r3, r2, #2
	add	r3, r2
	ldr	r2, [r1, #0x10]
	add	r3, r2
	str	r3, [r1, #0x18]
	ldr	r0, [r7, #0x2c]
	sub	sp, #8
	bl	_Func_16498
	mov	r0, #1
	bl	Func_30f8
	mov	r2, r8
	ldr	r3, [r2, #0x18]
	mov	r1, #0xe4
	lsl	r3, #1
	lsl	r1, #1
	add	r3, r1
	ldrh	r2, [r7, r3]
	mov	r3, r2
	cmp	r3, #0
	beq	.La5664
	ldr	r0, =0x1ff
	ldr	r3, =0x75
	and	r0, r2
	add	r0, r3
	ldr	r1, [r7, #0x2c]
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
.La5664:
	mov	r2, #1
	mov	r6, #0
	mov	r10, r2
	mov	r5, #1
.La566c:
	mov	r1, r8
	ldr	r3, [r1, #0x10]
	cmp	r6, r3
	bne	.La5688
	mov	r2, r10
	ldr	r0, [r7, #0x20]
	mov	r3, #0xe
	str	r2, [sp]
	mov	r1, #1
	mov	r2, r5
	str	r3, [sp, #4]
	bl	Func_a2268
	b	.La569c
.La5688:
	mov	r3, r10
	ldr	r0, [r7, #0x20]
	str	r3, [sp]
	mov	r3, #0xf
	str	r3, [sp, #4]
	mov	r1, #1
	mov	r2, r5
	mov	r3, #0xe
	bl	Func_a2268
.La569c:
	add	r6, #1
	add	r5, #2
	cmp	r6, #4
	ble	.La566c
	mov	r0, #1
	bl	Func_30f8
	mov	r0, #1
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a5614

@ DrawAbilityPage
@ r0 = window, r1 = unused, r2 = scroll descriptor. Draws one page of five
@ abilities: clears with _Func_1e41c, shows the five sprites with
@ Func_a2324(5, page*5, ...) at x 0x74, draws the page bar with Func_a21b0, and
@ prints each name -- STRING 0x182 + (id & 0x1FF) -- 16 pixels apart down the
@ window. A short last page draws fewer rows; the count is clamped to 5.
.thumb_func_start Func_a56c8
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r5, r2
	sub	sp, #4
	mov	r6, r0
	mov	r10, r3
	bl	_Func_16498
	mov	r3, #0xb
	str	r3, [sp]
	mov	r2, #0xb
	mov	r3, #0x10
	mov	r0, r6
	mov	r1, #0
	bl	_Func_1e41c
	ldr	r2, [r5, #8]
	lsl	r3, r2, #2
	add	r7, r3, r2
	ldr	r3, [r5, #0x14]
	sub	r3, r7
	lsl	r3, #24
	lsr	r3, #24
	mov	r8, r3
	cmp	r3, #5
	bls	.La5708
	mov	r2, #5
	mov	r8, r2
.La5708:
	mov	r3, #0x22
	str	r3, [sp]
	mov	r2, r6
	mov	r0, #5
	mov	r1, r7
	mov	r3, #0x74
	bl	Func_a2324
	mov	r2, #0xf
	ldr	r3, [r5, #8]
	ldr	r1, [r5, #0x14]
	mov	r0, r6
	str	r2, [sp]
	mov	r2, #5
	bl	Func_a21b0
	mov	r3, r8
	mov	r6, #0
	cmp	r3, #0
	bls	.La5770
	lsl	r3, r7, #1
	mov	r2, #0xe4
	add	r3, r10
	lsl	r2, #1
	ldr	r7, .La5764	@ 0x1ff
	add	r5, r3, r2
.La573c:
	ldrh	r3, [r5]
	mov	r0, r7
	and	r0, r3
	ldr	r3, =0x182
	add	r0, r3
	mov	r3, r10
	ldr	r1, [r3, #0x20]
	lsl	r3, r6, #4
	add	r3, #8
	mov	r2, #0x18
	bl	_Func_1e7c0
	add	r3, r6, #1
	lsl	r3, #24
	lsr	r6, r3, #24
	add	r5, #2
	cmp	r8, r6
	bhi	.La573c
	b	.La5770

	.align	2, 0
.La5764:
	.word	0x1ff
	.pool

.La5770:
	mov	r0, #1
	add	sp, #4
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a56c8

@ AlwaysAccept
@ Returns 1. A table entry meaning "this option is always available".
.thumb_func_start Func_a5780
	mov	r0, #1
	bx	lr
.func_end Func_a5780

@ NullHandler3
@ An empty `bx lr`.
.thumb_func_start Func_a5784
	bx	lr
.func_end Func_a5784

@ RunAbilityList
@ r0 = mode. The Psynergy screen's list state: builds the scroll descriptor with
@ Func_a5578, draws with Func_a56c8 and Func_a5614, and loops on Func_a1fd4 for
@ the d-pad. _Func_7842c decides whether the highlighted ability is usable and
@ _Func_ba30 nods the party sprite accordingly; unusable choices print string
@ 0xB89 and refuse. A commits through Func_a3ef0, B returns -1.
@ 504 lines; traced structurally.
.thumb_func_start Func_a5788
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	sub	sp, #0x58
	mov	r1, #0
	ldr	r7, [r3]
	str	r1, [sp, #0x20]
	str	r1, [sp, #0x14]
	add	r4, sp, #0x20
	mov	r2, #0x97
	ldrb	r4, [r4]
	lsl	r2, #2
	add	r3, r7, r2
	strb	r4, [r3]
	mov	r5, r7
	mov	r3, #0xe
	str	r3, [sp]
	add	r5, #0x34
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r11, r0
	mov	r1, #0xd
	mov	r0, r5
	mov	r2, #3
	mov	r3, #0x11
	bl	Func_a10d0
	ldr	r5, [r5]
	mov	r1, #0
	str	r5, [sp, #0x24]
	str	r1, [sp, #0x18]
	mov	r2, #0x28
	mov	r3, r11
	add	r2, sp
	lsl	r3, #1
	add	r4, r7, #2
	mov	r8, r2
	str	r3, [sp, #8]
	str	r4, [sp, #0xc]
	b	.La5aba
.La57e2:
	mov	r0, #0xad
	bl	_Func_f9080
	mov	r1, r8
	ldr	r3, [r1, #0x18]
	mov	r2, #0xe4
	lsl	r3, #1
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	str	r3, [sp, #0x20]
	mov	r3, #1
	str	r3, [sp, #0x18]
	b	.La5aba
.La57fe:
	mov	r0, #0x71
	bl	_Func_f9080
	mov	r4, #1
	str	r5, [sp, #0x20]
	str	r4, [sp, #0x18]
	b	.La5aba

	.pool_aligned

.La5810:
	mov	r2, r8
	ldr	r1, [r2, #0x10]
	lsl	r1, #4
	add	r1, #0x24
	mov	r0, #0x62
	bl	Func_a1a40
	mov	r3, r10
	cmp	r3, #0
	beq	.La58ec
	ldr	r1, [sp, #0x14]
	lsl	r3, r1, #1
	add	r3, #0xd8
	mov	r2, r9
	ldrh	r3, [r2, r3]
	mov	r4, #0
	mov	r10, r4
	cmp	r3, #0
	beq	.La5840
	lsl	r3, r1, #2
	add	r3, #0x48
	ldr	r0, [r7, r3]
	bl	Func_a17c4
.La5840:
	ldr	r3, [sp, #0x1c]
	cmp	r3, #0
	beq	.La586c
	mov	r0, #1
	bl	Func_30f8
	mov	r1, #0
	ldr	r0, [sp, #0x24]
	mov	r2, r8
	bl	Func_a56c8
	mov	r4, r11
	cmp	r4, #0
	bne	.La5868
	ldr	r0, =0xb89
	ldr	r1, [sp, #0x24]
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_1e7c0
.La5868:
	mov	r1, #0
	str	r1, [sp, #0x1c]
.La586c:
	add	r1, sp, #0x44
	ldr	r0, [sp, #0x24]
	mov	r2, r8
	bl	Func_a5614
	mov	r4, #0xbc
	ldr	r3, [sp, #8]
	lsl	r4, #1
	mov	r1, r8
	add	r2, r3, r4
	ldr	r3, [r1, #0x18]
	mov	r5, #0xe4
	lsl	r5, #1
	lsl	r3, #1
	add	r3, r5
	ldrh	r3, [r7, r3]
	strh	r3, [r7, r2]
	mov	r3, #0x86
	ldr	r2, [sp, #0xc]
	lsl	r3, #2
	add	r3, r11
	ldrb	r3, [r2, r3]
	ldr	r1, [r1, #0x18]
	mov	r0, r3
	mov	r2, #0
	bl	Func_a3ef0
	mov	r3, r8
	ldr	r2, [r3, #0x18]
	lsl	r3, r2, #1
	add	r3, r5
	ldrh	r3, [r7, r3]
	cmp	r3, #0
	beq	.La58c2
	lsl	r3, r2, #2
	add	r3, #0x48
	ldr	r0, [r7, r3]
	mov	r3, #9
	mov	r2, #0
	strb	r3, [r0, #5]
	mov	r3, #0xfa
	strh	r2, [r0, #0xc]
	strb	r3, [r0, #0xf]
.La58c2:
	ldr	r4, =0x219
	add	r3, r7, r4
	ldrb	r3, [r3]
	mov	r5, #0
	cmp	r5, r3
	bcs	.La58ec
	add	r6, r7, r4
.La58d0:
	mov	r1, #0x8a
	lsl	r3, r5, #2
	lsl	r1, #1
	add	r3, r1
	ldr	r0, [r7, r3]
	mov	r1, #1
	bl	_Func_ba30
	add	r3, r5, #1
	lsl	r3, #24
	lsr	r5, r3, #24
	ldrb	r3, [r6]
	cmp	r5, r3
	bcc	.La58d0
.La58ec:
	ldr	r3, =iwram_1e40
	ldr	r2, [r3]
	mov	r3, #0x1f
	and	r2, r3
	cmp	r2, #0
	bne	.La595c
	ldr	r2, =0x219
	add	r3, r7, r2
	ldrb	r3, [r3]
	mov	r5, #0
	cmp	r5, r3
	bcs	.La595c
.La5904:
	mov	r4, #0x82
	lsl	r3, r5, #1
	lsl	r4, #2
	add	r3, r4
	mov	r1, r8
	ldrh	r0, [r7, r3]
	ldr	r3, [r1, #0x18]
	mov	r2, #0xe4
	lsl	r3, #1
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	ldr	r1, .La594c	@ 0x1ff
	and	r1, r3
	bl	_Func_7842c
	cmp	r0, #0
	beq	.La5938
	mov	r4, #0x8a
	lsl	r3, r5, #2
	lsl	r4, #1
	add	r3, r4
	ldr	r0, [r7, r3]
	mov	r1, #3
	bl	_Func_ba30
.La5938:
	add	r3, r5, #1
	ldr	r1, =0x219
	lsl	r3, #24
	lsr	r5, r3, #24
	add	r3, r7, r1
	ldrb	r3, [r3]
	cmp	r5, r3
	bcc	.La5904
	b	.La595c

	.align	2, 0
.La594c:
	.word	0x1ff
	.pool

.La595c:
	mov	r0, #1
	bl	Func_30f8
	mov	r2, r8
	ldr	r2, [r2, #0x18]
	str	r2, [sp, #0x14]
	mov	r3, r8
	add	r2, sp, #0x30
	ldr	r1, [r3, #0x14]
	mov	r0, #0
	str	r2, [sp]
	add	r3, sp, #0x38
	mov	r2, #5
	bl	Func_a1fd4
	cmp	r0, #1
	bne	.La5984
	mov	r4, #1
	str	r4, [sp, #0x1c]
	mov	r10, r4
.La5984:
	cmp	r0, #0
	bne	.La598c
	mov	r1, #1
	mov	r10, r1
.La598c:
	mov	r5, #1
	neg	r5, r5
	cmp	r0, r5
	bne	.La5998
	mov	r2, #0
	mov	r10, r2
.La5998:
	ldr	r1, =iwram_1c94
	ldr	r2, [r1]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La59b8
	mov	r4, r8
	ldr	r3, [r4, #0x18]
	mov	r2, #0xe4
	lsl	r3, #1
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	cmp	r3, #0
	beq	.La59b8
	b	.La57e2
.La59b8:
	ldr	r2, [r1]
	mov	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La59c4
	b	.La57fe
.La59c4:
	ldr	r1, =iwram_1b04
	ldr	r2, [r1]
	add	r3, #0xfe
	and	r2, r3
	cmp	r2, #0
	bne	.La59dc
	ldr	r2, [r1]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La5aac
.La59dc:
	mov	r3, r11
	cmp	r3, #1
	bne	.La59f0
	mov	r0, #0x72
	bl	_Func_f9080
	mov	r0, #1
	bl	Func_30f8
	b	.La5aac
.La59f0:
	mov	r0, #0x6f
	bl	_Func_f9080
	mov	r1, #0x86
	ldr	r2, [sp, #0xc]
	mov	r4, r11
	lsl	r1, #2
	add	r0, r4, r1
	ldrb	r3, [r2, r0]
	mov	r4, #0x98
	lsl	r4, #2
	add	r3, r4
	mov	r4, r8
	ldr	r2, [r4, #0x18]
	strb	r2, [r7, r3]
	mov	r2, r11
	add	r2, #0x1c
	str	r2, [sp, #0x10]
	ldr	r3, [sp, #0xc]
	ldrsb	r5, [r7, r2]
	mov	r9, r0
	mov	r10, r3
.La5a1c:
	ldr	r3, =iwram_1b04
	mov	r2, #0x80
	ldr	r3, [r3]
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La5a2e
	add	r5, #1
	b	.La5a30
.La5a2e:
	sub	r5, #1
.La5a30:
	ldr	r4, =0x219
	add	r3, r7, r4
	ldrb	r1, [r3]
	add	r0, r5, r1
	bl	Func_b1c_from_thumb
	mov	r1, #0x82
	mov	r5, r0
	lsl	r1, #2
	lsl	r6, r5, #1
	add	r2, r6, r1
	ldrh	r3, [r7, r2]
	str	r3, [r7, #8]
	ldrh	r3, [r7, r2]
	mov	r4, r9
	mov	r2, r10
	strb	r3, [r2, r4]
	ldr	r1, [sp, #0x10]
	strb	r5, [r7, r1]
	ldrb	r0, [r2, r4]
	bl	_Func_77394
	mov	r2, #0xe4
	lsl	r2, #1
	add	r1, r7, r2
	mov	r2, #0
	bl	Func_a3ddc
	mov	r3, #0x86
	lsl	r3, #2
	strb	r0, [r7, r3]
	lsl	r0, #24
	cmp	r0, #0
	beq	.La5a1c
	mov	r1, #0xa2
	ldr	r2, .La5a98	@ 0x1e
	mov	r5, #0
	lsl	r1, #1
.La5a7c:
	lsl	r3, r5, #1
	add	r3, r1
	strh	r2, [r7, r3]
	add	r3, r5, #1
	lsl	r3, #24
	lsr	r5, r3, #24
	cmp	r5, #3
	bls	.La5a7c
	mov	r4, #0xa2
	lsl	r4, #1
	ldr	r3, .La5a9c	@ 0x1a
	add	r2, r6, r4
	strh	r3, [r7, r2]
	b	.La5aba

	.align	2, 0
.La5a98:
	.word	0x1e
.La5a9c:
	.word	0x1a
	.pool

.La5aac:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La5aba
	b	.La5810
.La5aba:
	ldr	r1, [sp, #0x18]
	cmp	r1, #0
	bne	.La5b20
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La5b20
	mov	r6, #0x86
	ldr	r4, [sp, #0xc]
	mov	r2, r11
	lsl	r6, #2
	add	r3, r2, r6
	ldrb	r0, [r4, r3]
	bl	_Func_77394
	mov	r1, #0xe4
	lsl	r1, #1
	add	r5, r7, r1
	mov	r2, #0
	mov	r1, r5
	mov	r9, r0
	bl	Func_a3ddc
	mov	r1, #0
	strb	r0, [r7, r6]
	mov	r0, r5
	bl	Func_a3e28
	mov	r2, #0x87
	lsl	r2, #2
	add	r3, r7, r2
	ldr	r2, [r3]
	mov	r3, #0xd
	strb	r3, [r2, #5]
	mov	r1, r11
	mov	r0, r8
	bl	Func_a5578
	mov	r3, r8
	ldr	r1, [r3, #0x18]
	lsl	r1, #4
	add	r1, #0x24
	mov	r0, #0x62
	bl	Func_a1a40
	mov	r4, #1
	mov	r10, r4
	str	r4, [sp, #0x1c]
	b	.La5aac
.La5b20:
	mov	r3, #0x60
	str	r3, [sp]
	ldr	r0, [sp, #0x24]
	mov	r1, #0
	mov	r2, #0x58
	mov	r3, #0x78
	bl	_Func_164d4
	ldr	r0, [r7, #0x44]
	bl	Func_a17c4
	mov	r3, #0xba
	ldr	r1, [sp, #8]
	mov	r4, r8
	lsl	r3, #1
	add	r2, r1, r3
	ldr	r3, [r4, #0x18]
	strh	r3, [r7, r2]
	mov	r3, #0x86
	ldr	r1, [sp, #0xc]
	lsl	r3, #2
	add	r3, r11
	ldrb	r3, [r1, r3]
	mov	r2, #0x98
	lsl	r2, #2
	add	r3, r2
	ldr	r2, [r4, #0x18]
	strb	r2, [r7, r3]
	ldr	r4, [sp, #8]
	mov	r1, #0xbc
	add	r2, sp, #0x20
	lsl	r1, #1
	ldrh	r2, [r2]
	add	r3, r4, r1
	mov	r0, #0xa8
	strh	r2, [r7, r3]
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	beq	.La5b78
	mov	r3, #1
	neg	r3, r3
	str	r3, [sp, #0x20]
.La5b78:
	mov	r0, #1
	bl	Func_30f8
	ldr	r0, [sp, #0x20]
	add	sp, #0x58
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a5788

@ RunItemScreen -- MENU INDEX 1
@ Takes no arguments. The Items screen. Follows the standard scaffold described
@ in the module header, with Func_a5cc0 as its state machine and a party header
@ three rows tall.
@
@ On a successful selection (Func_a5cc0 returning 1) it resolves the item
@ through _Func_78b9c -- MASK 0x3FFF, the display-record id space, which is what
@ distinguishes this screen's result from Func_a24d0's 0x1FF ability id -- and
@ writes `(arg1 << 10) | arg3` to [iwram_1e68+0x54]+0x17E for the field engine
@ to act on.
@
@ Returns Func_a5cc0's answer; rom_15000's Func_1c244 loops the menu again on
@ -1 and leaves it otherwise.
.thumb_func_start Func_a5b94
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r1, #0xa7
	lsl	r1, #4
	mov	r0, #0x37
	sub	sp, #0x10
	bl	Func_48b0
	ldr	r1, =iwram_1e68
	ldr	r2, [r1]
	mov	r3, #1
	mov	r6, r0
	strh	r3, [r2, #4]
	mov	r0, #0
	mov	r3, #0x14
	mov	r2, #0x1e
	mov	r8, r1
	mov	r1, #0
	bl	_Func_170f8
	mov	r0, #1
	bl	Func_30f8
	mov	r0, #0
	bl	Func_a1090
	mov	r2, #0x82
	lsl	r2, #2
	add	r0, r6, r2
	bl	_Func_796c4
	ldr	r1, =0x219
	add	r3, r6, r1
	strb	r0, [r3]
	mov	r1, #3
	mov	r0, #0
	mov	r2, #0
	mov	r3, #7
	bl	Func_a3354
	mov	r3, #2
	str	r3, [sp]
	mov	r1, #0
	mov	r2, #0x11
	mov	r3, #3
	mov	r0, #0xd
	bl	_Func_162d4
	mov	r2, #0x86
	lsl	r2, #1
	add	r3, r6, r2
	str	r0, [r3]
	mov	r0, #0xe
	bl	Func_a2144
	ldr	r0, =0x6002500
	bl	_Func_219c8
	bl	Func_a2474
	add	r0, sp, #0xc
	add	r1, sp, #8
	add	r2, sp, #4
	bl	Func_a5cc0
	mov	r7, r0
	bl	Func_a2490
	cmp	r7, #1
	bne	.La5c46
	mov	r1, #0xbc
	lsl	r1, #1
	mov	r3, r8
	ldr	r5, [r3, #0x54]
	add	r3, r6, r1
	ldrh	r3, [r3]
	ldr	r0, =0x3fff
	and	r0, r3
	bl	_Func_78b9c
	ldr	r3, [sp, #0xc]
	ldr	r2, [sp, #4]
	mov	r1, #0xbf
	lsl	r3, #10
	lsl	r1, #1
	orr	r2, r3
	add	r3, r5, r1
	strh	r2, [r3]
.La5c46:
	ldr	r0, [r6, #0x24]
	mov	r6, r8
	add	r6, #0x24
	bl	_Func_164ac
	ldr	r5, =0xea6
	ldr	r2, [r6]
	ldr	r3, .La5c90	@ 1
	strb	r3, [r2, r5]
	bl	Func_a34c0
	mov	r1, #0
	mov	r2, #0x1e
	mov	r3, #0x14
	mov	r0, #0
	bl	_Func_170f8
	mov	r0, #0x37
	bl	Func_2dd8
	mov	r3, r8
	ldr	r2, [r3]
	mov	r3, #0
	strh	r3, [r2, #4]
	mov	r0, #1
	bl	Func_30f8
	mov	r0, #0
	mov	r1, #0
	mov	r2, #0x1e
	mov	r3, #0x14
	bl	_Func_16178
	ldr	r3, [r6]
	ldr	r2, .La5c94	@ 0
	add	r3, r5
	b	.La5cac

	.align	2, 0
.La5c90:
	.word	1
.La5c94:
	.word	0
	.pool

.La5cac:
	strb	r2, [r3]
	bl	_Func_91858
	mov	r0, r7
	add	sp, #0x10
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a5b94

@ RunItemScreenStates
@ r0, r1, r2 = out-parameters. A five-state machine (table at the head of the
@ body) covering: pick a member, pick an item, pick an action, apply it, and
@ assign a shortcut. Draws its prompts from 0xAE2, 0xAE3, 0xAE9, 0xAEA, 0xAEB,
@ 0xAF0 and 0xAF1, reports failures as 0xBEF + state+0x25A, and delegates the
@ work to Func_a602c, Func_a63e4, Func_a6ccc, Func_a9f10 and Func_aa460.
@ Func_a65e4 is what writes the field shortcut. 365 lines; traced structurally.
.thumb_func_start Func_a5cc0
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	mov	r5, #0
	str	r0, [sp, #8]
	str	r2, [sp, #4]
	str	r5, [sp]
	ldr	r3, =iwram_1f2c
	ldr	r7, [r3]
	mov	r11, r5
	b	.La5fa4
.La5ce0:
	cmp	r5, #4
	bls	.La5ce6
	b	.La5fa0
.La5ce6:
	ldr	r2, =.La5cf0
	lsl	r3, r5, #2
	ldr	r3, [r3, r2]
	mov	pc, r3
	.align	2, 0
.La5cf0:
	.word	.La5d04
	.word	.La5d34
	.word	.La5e22
	.word	.La5dfa
	.word	.La5e92
.La5d04:
	mov	r3, #0xba
	lsl	r3, #1
	add	r2, r7, r3
	mov	r3, #0
	strh	r3, [r2]
	ldr	r1, =0xae9
	mov	r0, #0
	bl	Func_a3cf8
	mov	r0, #0
	bl	Func_a602c
	mov	r3, #1
	neg	r3, r3
	cmp	r0, r3
	bne	.La5d2a
	mov	r2, #1
	str	r2, [sp]
	mov	r11, r3
.La5d2a:
	ldr	r0, [r7, #0x2c]
	bl	_Func_16498
	mov	r5, #1
	b	.La5fa4
.La5d34:
	mov	r0, #1
	bl	Func_30f8
	ldr	r2, =0x21a
	add	r3, r7, r2
	ldrb	r0, [r3]
	bl	_Func_77394
	mov	r2, #0x86
	lsl	r2, #2
	add	r3, r7, r2
	ldrb	r3, [r3]
	mov	r5, #0
	cmp	r3, #0
	bne	.La5d54
	b	.La5fa4
.La5d54:
	mov	r2, #0x9a
	lsl	r2, #2
	add	r3, r7, r2
	ldrb	r3, [r3]
	cmp	r3, #1
	beq	.La5d74
	cmp	r3, #1
	bgt	.La5d6a
	cmp	r3, #0
	beq	.La5d70
	b	.La5d86
.La5d6a:
	cmp	r3, #2
	beq	.La5d7e
	b	.La5d86
.La5d70:
	ldr	r1, =0xaea
	b	.La5d76
.La5d74:
	ldr	r1, =0xaf1
.La5d76:
	mov	r0, #0
	bl	Func_a3cf8
	b	.La5d86
.La5d7e:
	ldr	r1, =0xaf0
	mov	r0, #0
	bl	Func_a3cf8
.La5d86:
	bl	Func_a9cbc
	ldr	r3, =0x21a
	add	r6, r7, r3
	ldrb	r1, [r6]
	mov	r2, #0
	ldr	r0, [r7, #0x24]
	mov	r3, #0
	bl	Func_a112c
	mov	r0, #0
	bl	Func_a6ccc
	mov	r2, #1
	neg	r2, r2
	mov	r1, r0
	mov	r8, r2
	mov	r5, #0
	cmp	r1, r8
	bne	.La5db0
	b	.La5fa4
.La5db0:
	mov	r2, #0x9a
	lsl	r2, #2
	add	r3, r7, r2
	ldrb	r3, [r3]
	mov	r5, #2
	cmp	r3, #0
	bne	.La5dc0
	b	.La5fa4
.La5dc0:
	cmp	r3, #1
	bne	.La5dde
	mov	r2, #0
	ldrb	r0, [r6]
	bl	Func_a65e4
	ldr	r0, [r7, #0x2c]
	bl	_Func_164ac
	ldr	r0, =0xae2
	mov	r1, r8
	mov	r2, r8
	bl	Func_a1d08
	b	.La5df6
.La5dde:
	mov	r2, #1
	ldrb	r0, [r6]
	bl	Func_a65e4
	ldr	r0, [r7, #0x2c]
	bl	_Func_164ac
	ldr	r0, =0xae3
	mov	r1, r8
	mov	r2, r8
	bl	Func_a1d08
.La5df6:
	mov	r5, #0
	b	.La5fa4
.La5dfa:
	ldr	r1, =0xaeb
	mov	r0, #0
	bl	Func_a3cf8
	mov	r0, #0
	bl	Func_a63e4
	mov	r3, #1
	mov	r10, r0
	neg	r3, r3
	mov	r5, #4
	cmp	r10, r3
	beq	.La5e16
	b	.La5fa4
.La5e16:
	mov	r2, #0x88
	lsl	r2, #2
	add	r1, r7, r2
	ldrh	r2, [r1]
	ldr	r3, .La5e40	@ 1
	b	.La5f58
.La5e22:
	bl	Func_a5fe0
	cmp	r0, #1
	bne	.La5e2e
.La5e2a:
	mov	r5, #3
	b	.La5fa4
.La5e2e:
	cmp	r0, #2
	bne	.La5e70
	ldr	r3, =0x21b
	add	r2, r7, r3
	mov	r3, #9
	strb	r3, [r2]
	mov	r5, #4
	b	.La5fa4

	.align	2, 0
.La5e40:
	.word	1
	.pool

.La5e70:
	mov	r2, #1
	str	r2, [sp]
	mov	r11, r2
	ldr	r2, =0x21a
	add	r3, r7, r2
	ldrb	r3, [r3]
	ldr	r2, [sp, #8]
	str	r3, [r2]
	mov	r2, #0xbc
	lsl	r2, #1
	add	r3, r7, r2
	ldrh	r2, [r3]
	ldr	r3, =0x3fff
	and	r3, r2
	ldr	r2, [sp, #4]
	str	r3, [r2]
	b	.La5fa4
.La5e92:
	mov	r2, #0xbc
	lsl	r2, #1
	add	r2, r7
	mov	r3, #0
	ldrh	r0, [r2]
	mov	r10, r3
	mov	r8, r2
	ldr	r3, =0x21a
	ldr	r2, =0x21b
	add	r5, r7, r3
	add	r6, r7, r2
	mov	r3, #0
	ldrb	r1, [r5]
	ldrb	r2, [r6]
	bl	Func_a9f10
	ldrb	r3, [r6]
	mov	r11, r0
	cmp	r3, #9
	bne	.La5ec2
	ldrb	r3, [r5]
	strb	r3, [r6]
	mov	r3, #9
	mov	r10, r3
.La5ec2:
	mov	r2, #1
	neg	r2, r2
	mov	r9, r2
	cmp	r11, r9
	beq	.La5ee4
	mov	r2, r8
	ldrh	r3, [r2]
	ldr	r0, =0x3fff
	and	r0, r3
	bl	_Func_78b9c
	ldrb	r3, [r5]
	ldrb	r1, [r0, #9]
	mov	r0, r3
	neg	r1, r1
	bl	_Func_783dc
.La5ee4:
	ldrb	r0, [r5]
	bl	_Func_77428
	cmp	r11, r9
	beq	.La5f22
	ldrb	r1, [r6]
	ldr	r0, [r7, #0x24]
	mov	r2, #0
	mov	r3, #0
	bl	Func_a112c
	mov	r2, r8
	ldrh	r3, [r2]
	ldr	r0, =0x3fff
	and	r0, r3
	bl	Func_aa460
	ldr	r0, [r7, #0x2c]
	bl	_Func_164ac
	ldr	r2, =0x25a
	add	r3, r7, r2
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	ldr	r3, =0xbef
	mov	r1, #0
	add	r0, r3
	mov	r2, r9
	bl	Func_a1d08
	b	.La5f42
.La5f22:
	mov	r0, #0x72
	bl	_Func_f9080
	ldr	r0, [r7, #0x2c]
	bl	_Func_164ac
	ldr	r2, =0x25a
	add	r3, r7, r2
	mov	r2, #0
	ldrsh	r0, [r3, r2]
	ldr	r3, =0xbef
	mov	r1, r11
	add	r0, r3
	mov	r2, r11
	bl	Func_a1d08
.La5f42:
	mov	r3, #1
	neg	r3, r3
	cmp	r11, r3
	beq	.La5f60
	mov	r3, #0x88
	lsl	r3, #2
	add	r1, r7, r3
	mov	r2, #1
	mov	r11, r2
	ldr	r3, .La5f84	@ 1
	ldrh	r2, [r1]
.La5f58:
	orr	r3, r2
	strh	r3, [r1]
	mov	r5, #1
	b	.La5fa4
.La5f60:
	ldr	r3, =0x222
	add	r2, r7, r3
	mov	r3, #1
	strh	r3, [r2]
	mov	r2, r10
	ldr	r1, .La5f84	@ 1
	cmp	r2, #9
	beq	.La5f72
	b	.La5e2a
.La5f72:
	mov	r3, #0x88
	lsl	r3, #2
	add	r2, r7, r3
	ldrh	r3, [r2]
	orr	r3, r1
	strh	r3, [r2]
	mov	r5, #1
	b	.La5fa4

	.align	2, 0
.La5f84:
	.word	1
	.pool

.La5fa0:
	mov	r2, #1
	str	r2, [sp]
.La5fa4:
	ldr	r3, [sp]
	cmp	r3, #0
	bne	.La5fb8
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La5fb8
	b	.La5ce0
.La5fb8:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	beq	.La5fca
	mov	r2, #1
	neg	r2, r2
	mov	r11, r2
.La5fca:
	mov	r0, r11
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a5cc0

@ ClassifySelectedItem
@ Takes no arguments. Looks at the item at state+0x178, masked 0x3FFF and
@ resolved through _Func_78b9c, and returns which kind of thing it is:
@
@     0  _Func_8e96c rejects the record's +0x0C, or +0x00 is not 2
@     1  +0x00 is 2
@     2  +0x08 is 0xFF
@
@ The caller uses this to pick which action menu to show.
.thumb_func_start Func_a5fe0
	push	{r5, lr}
	ldr	r3, =iwram_1f2c
	mov	r2, #0xbc
	ldr	r3, [r3]
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r3]
	ldr	r0, =0x3fff
	and	r0, r3
	bl	_Func_78b9c
	mov	r5, r0
	ldrb	r0, [r5, #0xc]
	bl	_Func_8e96c
	cmp	r0, #0
	beq	.La6006
	mov	r0, #0
	b	.La601e
.La6006:
	ldrb	r3, [r5, #8]
	mov	r0, #2
	cmp	r3, #0xff
	beq	.La601e
	ldrb	r3, [r5]
	mov	r2, #2
	eor	r3, r2
	neg	r0, r3
	orr	r0, r3
	lsr	r0, #31
	mov	r3, #1
	sub	r0, r3, r0
.La601e:
	pop	{r5}
	pop	{r1}
	bx	r1
.func_end Func_a5fe0

@ SelectItemOwner
@ r0 = which cursor. The item screen's counterpart to Func_a355c: shows the
@ cursor sprite at state+0x14 + r0*4, hides the one at state+0x21C, glides to
@ (index * 24 - 10, 0x10), builds that character's filtered list with
@ Func_a68ec(record, state+0x1C8, 2), records the count at state+0x218 and hands
@ off to Func_a60d4. A selection of -1 resets to member 0.
.thumb_func_start Func_a602c
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	mov	r4, r0
	lsl	r0, #2
	mov	r8, r0
	ldr	r7, [r3]
	mov	r3, r8
	add	r3, #0x14
	ldr	r0, [r7, r3]
	mov	r5, #0
	mov	r3, #1
	strb	r3, [r0, #5]
	strh	r5, [r0, #0xc]
	mov	r0, #0x87
	lsl	r0, #2
	add	r3, r7, r0
	ldr	r2, [r3]
	sub	r0, #3
	mov	r3, #0xd
	strb	r3, [r2, #5]
	add	r3, r7, r0
	ldrb	r3, [r3]
	add	r4, #0x1c
	add	r2, r7, #2
	ldrsb	r1, [r7, r4]
	strb	r3, [r2, r4]
	mov	r2, #1
	neg	r2, r2
	cmp	r1, r2
	bne	.La607c
	ldr	r3, .La6074	@ 0
	mov	r6, #0
	strb	r3, [r7, r4]
	b	.La608a

	.align	2, 0
.La6074:
	.word	0
	.pool

.La607c:
	lsl	r6, r1, #1
	add	r0, r6, r1
	lsl	r0, #3
	sub	r0, #0xa
	mov	r1, #0x10
	bl	Func_a1ac0
.La608a:
	mov	r5, #0x82
	lsl	r5, #2
	add	r3, r6, r5
	ldrh	r0, [r7, r3]
	bl	_Func_77394
	mov	r3, #0xe4
	lsl	r3, #1
	add	r6, r7, r3
	mov	r1, r6
	mov	r2, #2
	bl	Func_a68ec
	mov	r2, #0x86
	lsl	r2, #2
	add	r3, r7, r2
	add	r5, r7, r5
	mov	r1, r6
	strb	r0, [r3]
	mov	r0, r5
	bl	Func_a60d4
	mov	r3, r8
	add	r3, #0x14
	mov	r5, r0
	ldr	r0, [r7, r3]
	bl	Func_a17c4
	mov	r0, #1
	bl	Func_30f8
	mov	r0, r5
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a602c

@ RunItemOwnerLoop
@ r0 = roster array, r1 = list. The loop that sits on the member row of the item
@ screen: opens the panels, creates the sprites with Func_a33d4, draws the coin
@ total with Func_a23c0 and the shortcut state with Func_a6614, and refreshes
@ through Func_a6384. Watches save bit 0x151 for the message-box handshake.
@ 318 lines; traced structurally.
.thumb_func_start Func_a60d4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x20
	str	r0, [sp, #0x1c]
	ldr	r3, =iwram_1f2c
	ldr	r7, [r3]
	mov	r1, #0x1e
	ldrsb	r1, [r7, r1]
	mov	r0, #0x1c
	ldrsb	r0, [r7, r0]
	mov	r3, #0
	mov	r2, #1
	str	r1, [sp, #0x18]
	str	r2, [sp, #0x14]
	str	r3, [sp, #0x10]
	str	r3, [sp, #8]
	mov	r10, r0
	add	r1, sp, #0x10
	mov	r0, #0x9a
	ldrb	r1, [r1]
	lsl	r0, #2
	add	r3, r7, r0
	strb	r1, [r3]
	ldr	r3, [sp, #0x1c]
	mov	r2, r10
	lsl	r2, #1
	ldrh	r0, [r2, r3]
	bl	_Func_77394
	mov	r5, r7
	mov	r3, #0xa
	add	r5, #0x20
	str	r0, [sp, #0xc]
	str	r3, [sp]
	mov	r6, #2
	mov	r0, r5
	mov	r1, #0xd
	mov	r2, #3
	mov	r3, #0x11
	str	r6, [sp, #4]
	bl	Func_a10d0
	cmp	r0, #0
	beq	.La613e
	ldr	r1, [r5]
	mov	r0, r7
	bl	Func_a33d4
.La613e:
	mov	r5, r7
	mov	r3, #4
	add	r5, #0x28
	str	r3, [sp]
	mov	r0, r5
	mov	r1, #0xd
	mov	r2, #0xd
	mov	r3, #0x11
	str	r6, [sp, #4]
	bl	Func_a10d0
	cmp	r0, #0
	beq	.La6174
	ldr	r0, [sp, #0x10]
	ldr	r2, [r5]
	mov	r1, #0
	str	r0, [sp]
	mov	r3, #0
	mov	r0, #2
	bl	_Func_1eb64
	mov	r1, #0x87
	lsl	r1, #2
	add	r3, r7, r1
	str	r0, [r3]
	mov	r3, #0xd
	strb	r3, [r0, #5]
.La6174:
	mov	r2, #0x9a
	lsl	r2, #2
	add	r2, r7
	mov	r11, r2
	b	.La6338

	.pool_aligned

.La6184:
	ldr	r3, [sp, #0x14]
	cmp	r3, #0
	beq	.La6230
	mov	r0, #0
	str	r0, [sp, #0x14]
	ldr	r0, [sp, #0x18]
	ldr	r1, [sp, #0x18]
	add	r0, r10
	bl	Func_b1c_from_thumb
	mov	r10, r0
	mov	r1, r10
	ldr	r5, [sp, #0x1c]
	lsl	r1, #1
	mov	r8, r1
	add	r5, r8
	ldrh	r0, [r5]
	ldr	r6, [r7, #0x24]
	bl	_Func_77394
	str	r0, [sp, #0xc]
	ldrh	r0, [r5]
	bl	Func_a6384
	mov	r2, #0
	mov	r3, #0
	ldrh	r1, [r5]
	mov	r0, r6
	bl	Func_a112c
	ldrh	r1, [r5]
	ldr	r0, [r7, #0x28]
	bl	Func_a6614
	ldrh	r1, [r5]
	mov	r0, r7
	bl	Func_a1804
	mov	r0, #0xa5
	lsl	r0, #1
	ldr	r1, =0x1e
	mov	r9, r8
	mov	r2, #3
	add	r3, r7, r0
.La61dc:
	sub	r2, #1
	strh	r1, [r3]
	sub	r3, #2
	cmp	r2, #0
	bge	.La61dc
	mov	r3, #0xa2
	lsl	r3, #1
	ldr	r2, =0x1a
	add	r3, r9
	strh	r2, [r7, r3]
	ldr	r0, =0x151
	bl	_Func_79338
	cmp	r0, #0
	bne	.La6228
	ldr	r1, [sp, #8]
	cmp	r1, #0
	bne	.La6228
	b	.La6210

	.pool_aligned

.La6210:
	ldr	r0, [r7, #0x2c]
	bl	_Func_164ac
	ldr	r0, [r7, #0x2c]
	bl	_Func_16498
	ldr	r0, [r7, #0x2c]
	bl	Func_a23c0
	mov	r2, #1
	str	r2, [sp, #8]
	b	.La6236
.La6228:
	ldr	r0, =0x151
	bl	_Func_79374
	b	.La6236
.La6230:
	mov	r3, r10
	lsl	r3, #1
	mov	r8, r3
.La6236:
	mov	r0, r8
	add	r0, r10
	lsl	r0, #3
	mov	r1, #0x10
	sub	r0, #0xa
	bl	Func_a1a40
	mov	r0, #1
	bl	Func_30f8
	ldr	r1, =iwram_1c94
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La627a
	mov	r0, #0x86
	lsl	r0, #2
	add	r3, r7, r0
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.La6272
	mov	r0, #0x70
	bl	_Func_f9080
	ldr	r2, [sp, #0x1c]
	mov	r1, r8
	ldrh	r1, [r1, r2]
	str	r1, [sp, #0x10]
	b	.La634c
.La6272:
	mov	r0, #0x72
	bl	_Func_f9080
	ldr	r1, =iwram_1c94
.La627a:
	ldr	r0, =iwram_1c94
	ldr	r3, [r0]
	mov	r0, #0x80
	lsl	r0, #2
	and	r3, r0
	cmp	r3, #0
	bne	.La6294
	ldr	r3, [r1]
	mov	r2, #0x80
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La62ec
.La6294:
	ldr	r3, [sp, #0x1c]
	mov	r2, r8
	ldrh	r2, [r2, r3]
	str	r2, [sp, #0x10]
	ldr	r3, [r1]
	and	r3, r0
	cmp	r3, #0
	beq	.La62ac
	mov	r3, #1
	mov	r0, r11
	strb	r3, [r0]
	b	.La62b2
.La62ac:
	mov	r3, #2
	mov	r1, r11
	strb	r3, [r1]
.La62b2:
	mov	r0, #0x40
	bl	Func_4938
	mov	r6, r0
	mov	r1, r6
	ldr	r0, [sp, #0xc]
	mov	r2, #1
	bl	Func_a68ec
	mov	r5, r0
	lsl	r5, #24
	lsr	r5, #24
	lsl	r5, #24
	mov	r0, r6
	asr	r5, #24
	bl	Func_2df0
	cmp	r5, #0
	bne	.La62e4
	mov	r2, r11
	strb	r5, [r2]
	mov	r0, #0x72
	bl	_Func_f9080
	b	.La62ec
.La62e4:
	mov	r0, #0x70
	bl	_Func_f9080
	b	.La634c
.La62ec:
	ldr	r0, =iwram_1c94
	ldr	r3, [r0]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.La6306
	mov	r0, #0x71
	bl	_Func_f9080
	mov	r1, #1
	neg	r1, r1
	str	r1, [sp, #0x10]
	b	.La634c
.La6306:
	ldr	r5, =iwram_1b04
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.La6322
	mov	r0, #0x6f
	bl	_Func_f9080
	mov	r2, #1
	mov	r3, #1
	neg	r2, r2
	str	r3, [sp, #0x14]
	add	r10, r2
.La6322:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.La6338
	mov	r0, #0x6f
	bl	_Func_f9080
	mov	r0, #1
	str	r0, [sp, #0x14]
	add	r10, r0
.La6338:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La6346
	b	.La6184
.La6346:
	mov	r1, r10
	lsl	r1, #1
	mov	r8, r1
.La634c:
	mov	r2, r10
	strb	r2, [r7, #0x1c]
	ldr	r0, [sp, #0x1c]
	mov	r3, r8
	ldr	r1, =0x21a
	ldrh	r2, [r3, r0]
	add	r3, r7, r1
	str	r2, [r7, #8]
	strb	r2, [r3]
	ldr	r0, [sp, #0x10]
	add	sp, #0x20
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a60d4

@ DrawOwnerItemList
@ r0 = character id. Builds the filtered list into state+0x1C8, records the
@ count at state+0x218, releases the window at state+0x20, lays out an eight-
@ column grid from (0x6C, 0x20) and loads the icons with Func_a68a8. Prints
@ string 0xAF2 when the character is carrying nothing.
.thumb_func_start Func_a6384
	push	{r5, r6, r7, lr}
	ldr	r3, =iwram_1f2c
	ldr	r7, [r3]
	bl	_Func_77394
	mov	r3, #0xe4
	lsl	r3, #1
	add	r6, r7, r3
	mov	r1, r6
	mov	r2, #2
	bl	Func_a68ec
	mov	r3, #0x86
	lsl	r3, #2
	add	r5, r7, r3
	strb	r0, [r5]
	ldr	r0, [r7, #0x20]
	bl	_Func_16498
	mov	r0, #0x6c
	mov	r1, #0x20
	mov	r2, #8
	bl	Func_a1bdc
	mov	r0, r6
	bl	Func_a68a8
	ldrb	r3, [r5]
	cmp	r3, #0
	bne	.La63cc
	ldr	r0, =0xaf2
	ldr	r1, [r7, #0x20]
	mov	r2, #0
	mov	r3, #0x18
	bl	_Func_1e7c0
.La63cc:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a6384

@ AlwaysAccept2
@ Returns 1, like Func_a5780.
.thumb_func_start Func_a63dc
	mov	r0, #1
	bx	lr
.func_end Func_a63dc

@ NullHandler4
@ An empty `bx lr`.
.thumb_func_start Func_a63e0
	bx	lr
.func_end Func_a63e0

@ RunItemDetailLoop
@ Takes no arguments. Shows the highlighted item's detail line -- STRING 0x53A +
@ (id & 0x1FF) -- and runs the cursor over the party with Func_a1ac0, redrawing
@ the preview through Func_a112c as it moves. Exits on save bit 0x150 or 0x151.
@ 232 lines; traced structurally.
.thumb_func_start Func_a63e4
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	str	r0, [sp, #8]
	ldr	r3, =iwram_1f2c
	ldr	r6, [r3]
	ldr	r2, =0x219
	mov	r1, #0x1d
	ldrsb	r1, [r6, r1]
	add	r3, r6, r2
	ldrb	r3, [r3]
	mov	r8, r1
	mov	r1, #0
	str	r1, [sp, #4]
	str	r1, [sp]
	mov	r11, r3
	mov	r3, #1
	mov	r9, r3
	mov	r3, #0x1c
	ldrsb	r3, [r6, r3]
	sub	r2, #0x11
	lsl	r3, #1
	add	r3, r2
	ldrh	r0, [r6, r3]
	bl	_Func_77394
	mov	r3, r8
	lsl	r3, #1
	mov	r10, r3
	mov	r0, r10
	add	r0, r8
	lsl	r0, #3
	sub	r0, #0xa
	mov	r1, #0x10
	bl	Func_a1ac0
	b	.La6582

	.pool_aligned

.La6440:
	mov	r1, r9
	cmp	r1, #0
	beq	.La6500
	mov	r0, r8
	mov	r2, #0
	mov	r1, r11
	add	r0, r11
	mov	r9, r2
	bl	Func_b1c_from_thumb
	mov	r8, r0
	mov	r3, r8
	lsl	r3, #1
	mov	r7, #0x82
	mov	r10, r3
	lsl	r7, #2
	add	r7, r10
	ldrh	r0, [r6, r7]
	bl	_Func_77394
	ldr	r3, [r6, #0x10]
	mov	r1, r10
	ldrh	r2, [r3, #0xc]
	add	r1, r8
	add	r2, r1
	ldr	r5, [r6, #0x18]
	ldr	r3, =0xffff
	lsl	r2, #3
	sub	r2, #2
	strh	r2, [r5, #6]
	and	r2, r3
	ldr	r3, =0x1ff
	ldr	r1, =0xfffffe00
	and	r2, r3
	ldrh	r3, [r5, #0x16]
	and	r3, r1
	orr	r3, r2
	strh	r3, [r5, #0x16]
	ldr	r1, [sp, #8]
	cmp	r1, #0
	bne	.La6506
	b	.La64a0

	.pool_aligned

.La64a0:
	ldr	r0, [r6, #0x24]
	ldrh	r1, [r6, r7]
	mov	r2, #0
	mov	r3, #0
	bl	Func_a112c
	mov	r0, r6
	ldrh	r1, [r6, r7]
	bl	Func_a1804
	ldr	r0, =0x151
	bl	_Func_79338
	cmp	r0, #0
	bne	.La64ea
	ldr	r2, [sp]
	cmp	r2, #0
	bne	.La64ea
	ldr	r0, [r6, #0x2c]
	bl	_Func_16498
	mov	r1, #0xbc
	lsl	r1, #1
	add	r3, r6, r1
	ldrh	r3, [r3]
	ldr	r0, .La64f4	@ 0x3fff
	and	r0, r3
	ldr	r3, =0x53a
	mov	r2, #0
	add	r0, r3
	ldr	r1, [r6, #0x2c]
	mov	r3, #0
	bl	_Func_1e7c0
	mov	r2, #1
	str	r2, [sp]
	b	.La6506
.La64ea:
	ldr	r0, =0x151
	bl	_Func_79374
	b	.La6506

	.align	2, 0
.La64f4:
	.word	0x3fff
	.pool

.La6500:
	mov	r3, r8
	lsl	r3, #1
	mov	r10, r3
.La6506:
	mov	r0, r10
	add	r0, r8
	lsl	r0, #3
	mov	r1, #0x10
	sub	r0, #0xa
	bl	Func_a1a40
	mov	r0, #1
	bl	Func_30f8
	ldr	r1, =iwram_1c94
	ldr	r3, [r1]
	mov	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La6538
	mov	r0, #0x70
	bl	_Func_f9080
	mov	r3, #0x82
	lsl	r3, #2
	add	r3, r10
	ldrh	r3, [r6, r3]
	str	r3, [sp, #4]
	b	.La6596
.La6538:
	ldr	r3, [r1]
	mov	r2, #2
	and	r3, r2
	cmp	r3, #0
	beq	.La6550
	mov	r0, #0x71
	bl	_Func_f9080
	mov	r1, #1
	neg	r1, r1
	str	r1, [sp, #4]
	b	.La6596
.La6550:
	ldr	r5, =iwram_1b04
	ldr	r3, [r5]
	mov	r2, #0x20
	and	r3, r2
	cmp	r3, #0
	beq	.La656c
	mov	r0, #0x6f
	bl	_Func_f9080
	mov	r2, #1
	neg	r2, r2
	mov	r3, #1
	add	r8, r2
	mov	r9, r3
.La656c:
	ldr	r3, [r5]
	mov	r2, #0x10
	and	r3, r2
	cmp	r3, #0
	beq	.La6582
	mov	r0, #0x6f
	bl	_Func_f9080
	mov	r1, #1
	add	r8, r1
	mov	r9, r1
.La6582:
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	bne	.La6590
	b	.La6440
.La6590:
	mov	r2, r8
	lsl	r2, #1
	mov	r10, r2
.La6596:
	ldr	r5, [r6, #0x18]
	mov	r3, r8
	strb	r3, [r6, #0x1d]
	mov	r0, r5
	bl	Func_a17c4
	mov	r3, #0xd
	strb	r3, [r5, #5]
	mov	r0, #1
	bl	Func_30f8
	mov	r1, r8
	mov	r2, #0x82
	strb	r1, [r6, #0x1d]
	lsl	r2, #2
	add	r2, r10
	ldrh	r3, [r6, r2]
	str	r3, [r6, #8]
	ldr	r1, =0x21b
	ldrh	r2, [r6, r2]
	add	r3, r6, r1
	strb	r2, [r3]
	ldr	r0, [sp, #4]
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a63e4

@ AssignFieldShortcut
@ r0 = party index, r1 = item id, r2 = which slot.
@ Packs `(index << 10) | (id & 0x3FFF)` and stores it at ewram_240+0x220 for
@ r2 == 0 and ewram_240+0x222 otherwise. Returns 1.
@
@ Those two halfwords are exactly the pair rom_8a000's _Func_91858 revalidates
@ whenever a menu closes, and rom_a1000's screen teardowns all call it -- which
@ is how a shortcut pointing at an item you no longer own gets cleared.
.thumb_func_start Func_a65e4
	push	{lr}
	ldr	r3, =0x3fff
	lsl	r0, #10
	and	r3, r1
	orr	r0, r3
	cmp	r2, #0
	bne	.La65fa
	ldr	r3, =ewram_240
	mov	r2, #0x88
	lsl	r2, #2
	b	.La65fe
.La65fa:
	ldr	r3, =ewram_240
	ldr	r2, =0x222
.La65fe:
	add	r3, r2
	strh	r0, [r3]
	mov	r0, #1
	pop	{r1}
	bx	r1
.func_end Func_a65e4

@ DrawShortcutRow
@ r0 = window. Draws the current field shortcuts. The heading is string 0xAE4
@ when both slots are occupied and 0xAE0 when either is empty. For each occupied
@ slot it prints the item name at 0x333 + (id & 0x3FF), registers the icon
@ through _Func_19908, adds label 0xAE7, and -- when the name is short enough
@ (the measured width from _Func_187ac is 10 or less) -- the owner's portrait
@ from _Func_77394(id >> 10) at x 0x50. 162 lines; traced structurally.
.thumb_func_start Func_a6614
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r2, =ewram_240
	mov	r1, #0x88
	lsl	r1, #2
	add	r3, r2, r1
	ldrh	r3, [r3]
	sub	sp, #0x14
	mov	r5, r0
	cmp	r3, #0
	beq	.La664e
	add	r1, #2
	add	r3, r2, r1
	ldrh	r3, [r3]
	cmp	r3, #0
	beq	.La664e
	mov	r3, #8
	ldr	r0, =0xae4
	neg	r3, r3
	mov	r1, r5
	mov	r2, #0
	bl	_Func_1e7c0
	b	.La665c
.La664e:
	mov	r3, #8
	ldr	r0, =0xae0
	neg	r3, r3
	mov	r1, r5
	mov	r2, #0
	bl	_Func_1e7c0
.La665c:
	ldr	r3, =ewram_240
	mov	r2, #0x88
	lsl	r2, #2
	add	r3, r2
	ldrh	r3, [r3]
	ldr	r0, =0x3ff
	and	r0, r3
	ldr	r3, =0x333
	add	r0, r3
	add	r3, sp, #0x10
	mov	r1, #0xc
	mov	r2, #8
	add	r1, sp
	add	r2, sp
	mov	r10, r3
	add	r3, sp, #4
	mov	r9, r1
	mov	r11, r2
	str	r3, [sp]
	mov	r8, r3
	mov	r1, r10
	mov	r3, r11
	mov	r2, r9
	bl	_Func_187ac
	ldr	r3, [sp, #8]
	mov	r6, #1
	cmp	r3, #0xa
	bhi	.La6698
	mov	r6, #0
.La6698:
	ldr	r3, =ewram_240
	mov	r1, #0x88
	lsl	r1, #2
	add	r7, r3, r1
	ldrh	r2, [r7]
	mov	r3, r2
	cmp	r3, #0
	beq	.La66d6
	ldr	r0, =0x3ff
	mov	r1, #4
	and	r0, r2
	bl	_Func_19908
	ldr	r0, =0xae7
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
	cmp	r6, #0
	bne	.La66e2
	ldrh	r0, [r7]
	lsr	r0, #10
	bl	_Func_77394
	mov	r1, r5
	mov	r2, #0x50
	mov	r3, #0
	bl	_Func_1e8b0
	b	.La66e2
.La66d6:
	ldr	r0, =0xae5
	mov	r1, r5
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
.La66e2:
	ldr	r3, =ewram_240
	ldr	r2, =0x222
	add	r3, r2
	ldrh	r3, [r3]
	ldr	r0, =0x3ff
	and	r0, r3
	ldr	r3, =0x333
	add	r0, r3
	mov	r3, r8
	str	r3, [sp]
	mov	r1, r10
	mov	r3, r11
	mov	r2, r9
	bl	_Func_187ac
	ldr	r3, [sp, #8]
	mov	r6, #1
	cmp	r3, #0xa
	bhi	.La670a
	mov	r6, #0
.La670a:
	ldr	r3, =ewram_240
	ldr	r1, =0x222
	add	r7, r3, r1
	ldrh	r2, [r7]
	mov	r3, r2
	cmp	r3, #0
	beq	.La674c
	ldr	r0, =0x3ff
	mov	r1, #4
	and	r0, r2
	bl	_Func_19908
	ldr	r0, =0xae8
	mov	r1, r5
	mov	r2, #0
	mov	r3, #8
	bl	_Func_1e7c0
	cmp	r6, #0
	bne	.La6744
	ldrh	r0, [r7]
	lsr	r0, #10
	bl	_Func_77394
	mov	r1, r5
	mov	r2, #0x50
	mov	r3, #8
	bl	_Func_1e8b0
.La6744:
	mov	r0, #0xf
	bl	_Func_1e71c
	b	.La6758
.La674c:
	ldr	r0, =0xae6
	mov	r1, r5
	mov	r2, #0
	mov	r3, #8
	bl	_Func_1e7c0
.La6758:
	mov	r0, #1
	add	sp, #0x14
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a6614

@ BuildItemScreenLayout
@ Takes no arguments. Builds the item screen's furniture: Func_a1814 and
@ Func_a1870 for the party strip, a (0, 5, 0x1E, 0xF) window into state+0x20,
@ a cursor sprite at state+0x44, the menu entries via _Func_1ec6c, and sixteen
@ panel sprites at y 0x60 stepping 0x10 -- the first eight at priority 8 and the
@ rest at 0x18, so the second row sorts behind the first.
.thumb_func_start Func_a6794
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r10, r3
	mov	r0, r10
	sub	sp, #8
	bl	Func_a1814
	mov	r5, #0
	mov	r1, #2
	mov	r2, #2
	mov	r3, #8
	str	r5, [sp]
	bl	Func_a1870
	mov	r6, #2
	mov	r1, #5
	mov	r2, #0x1e
	mov	r3, #0xf
	mov	r0, #0
	str	r6, [sp]
	bl	_Func_162d4
	mov	r3, #0x88
	lsl	r3, #1
	mov	r2, r10
	add	r3, r10
	str	r0, [r2, #0x20]
	strb	r5, [r3]
	ldr	r3, =0x111
	mov	r2, #0x89
	add	r3, r10
	lsl	r2, #1
	strb	r5, [r3]
	add	r2, r10
	mov	r3, #8
	strb	r3, [r2]
	ldr	r3, =0x113
	add	r3, r10
	strb	r6, [r3]
	mov	r1, #0
	mov	r2, #4
	mov	r8, r0
	bl	Func_a1778
	mov	r3, #0xd
	strb	r3, [r0, #5]
	mov	r3, r10
	str	r0, [r3, #0x44]
	mov	r2, #0
	mov	r0, #0
	mov	r1, #0
	mov	r3, r8
	str	r5, [sp]
	str	r5, [sp, #4]
	mov	r6, r10
	bl	_Func_1ec6c
	mov	r2, #8
	mov	r9, r2
	add	r6, #0x48
	mov	r7, #0x60
.La6818:
	mov	r3, r9
	str	r3, [sp]
	mov	r1, r5
	mov	r3, r7
	mov	r0, #4
	mov	r2, r8
	bl	_Func_1eb64
	add	r5, #1
	stmia	r6!, {r0}
	add	r7, #0x10
	cmp	r5, #7
	ble	.La6818
	mov	r2, #0x18
	mov	r6, r10
	mov	r5, #8
	mov	r9, r2
	add	r6, #0x68
	mov	r7, #0x60
.La683e:
	mov	r3, r9
	str	r3, [sp]
	mov	r1, r5
	mov	r3, r7
	mov	r0, #4
	mov	r2, r8
	bl	_Func_1eb64
	add	r5, #1
	stmia	r6!, {r0}
	add	r7, #0x10
	cmp	r5, #0xf
	ble	.La683e
	add	sp, #8
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a6794

@ CloseItemScreenWindows
@ Takes no arguments. Destroys the party actors and closes the three windows at
@ state+0x10, +0x20 and +0x10C, each with r1 = 1 so the tilemap is restored.
.thumb_func_start Func_a6874
	push	{r5, lr}
	ldr	r3, =iwram_1f2c
	ldr	r5, [r3]
	bl	Func_a195c
	ldr	r0, [r5, #0x10]
	mov	r1, #1
	bl	_Func_16418
	ldr	r0, [r5, #0x20]
	mov	r1, #1
	bl	_Func_16418
	mov	r3, #0x86
	lsl	r3, #1
	add	r5, r3
	ldr	r0, [r5]
	mov	r1, #1
	bl	_Func_16418
	pop	{r5}
	pop	{r0}
	bx	r0
.func_end Func_a6874

@ LoadItemIcons
@ r0 = list. Loads panel set 4 into each of the 32 nodes whose list entry is
@ non-zero, then hides the empty ones with Func_a3d24. The Func_a3e28 of the
@ item screen; only the panel set differs.
.thumb_func_start Func_a68a8
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	mov	r8, r0
	mov	r6, r3
	add	r6, #0x48
	mov	r5, r8
	mov	r7, #0x1f
.La68bc:
	ldrh	r1, [r5]
	add	r5, #2
	cmp	r1, #0
	beq	.La68d0
	ldr	r3, [r6]
	mov	r0, #4
	ldrb	r2, [r3, #0xe]
	mov	r3, #0
	bl	_Func_1bcd4
.La68d0:
	sub	r7, #1
	add	r6, #4
	cmp	r7, #0
	bge	.La68bc
	mov	r0, r8
	bl	Func_a3d24
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_a68a8

@ FilterItemList
@ r0 = character record, r1 = destination, r2 = filter.
@ Walks the 32-entry list at record+0x58 -- halfword ids at stride 4, masked
@ 0x3FFF and resolved through _Func_78b9c -- and copies the ones that pass into
@ a dense destination, returning the count. The destination's 32 bytes are
@ zeroed first.
@
@ Filter 1 keeps every entry whose display record has a non-zero +0x0C. Any
@ other filter runs `3 + (r2 != 2)` grouping passes, so entries come out ordered
@ by category rather than by slot -- the same idea as Func_a1e38, but keyed on
@ the display record instead of the ability record.
.thumb_func_start Func_a68ec
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	mov	r10, r1
	mov	r1, r2
	mov	r2, #2
	eor	r2, r1
	neg	r3, r2
	orr	r3, r2
	lsr	r3, #31
	mov	r8, r3
	mov	r2, #3
	add	r8, r2
	mov	r3, r10
	ldr	r2, =0
	sub	sp, #8
	mov	r9, r0
	add	r3, #0x3e
	mov	r12, r10
.La691a:
	strh	r2, [r3]
	sub	r3, #2
	cmp	r3, r12
	bge	.La691a
	mov	r4, #0
	cmp	r1, #1
	bne	.La6970
	ldr	r7, =0x3fff
	mov	r1, #0
	mov	r6, #0x58
	mov	r5, r10
	b	.La693c

	.pool_aligned

.La693c:
	mov	r3, r9
	ldrh	r2, [r6, r3]
	mov	r3, r2
	cmp	r3, #0
	beq	.La6966
	mov	r0, r7
	and	r0, r2
	str	r1, [sp, #4]
	str	r4, [sp]
	bl	_Func_78b9c
	ldrb	r3, [r0, #0xc]
	ldr	r1, [sp, #4]
	ldr	r4, [sp]
	cmp	r3, #0
	beq	.La6966
	mov	r2, r9
	ldrh	r3, [r2, r6]
	add	r4, #1
	strh	r3, [r5]
	add	r5, #2
.La6966:
	add	r1, #1
	add	r6, #4
	cmp	r1, #0x1f
	ble	.La693c
	b	.La69ea
.La6970:
	mov	r7, #0
	cmp	r7, r8
	bge	.La69ea
	mov	r3, #0x40
	mov	r11, r3
.La697a:
	lsl	r3, r4, #1
	mov	r2, r10
	mov	r6, r9
	add	r5, r3, r2
	mov	r1, #0x1f
	add	r6, #0x58
.La6986:
	ldrh	r2, [r6]
	mov	r3, r2
	cmp	r3, #0
	beq	.La69dc
	ldr	r0, =0x3fff
	and	r0, r2
	str	r1, [sp, #4]
	str	r4, [sp]
	bl	_Func_78b9c
	ldr	r1, [sp, #4]
	ldr	r4, [sp]
	cmp	r7, #0
	bne	.La69b8
	ldrb	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.La69d4
	ldrb	r2, [r0, #1]
	mov	r3, r11
	and	r3, r2
	cmp	r3, #0
	bne	.La69d4
	b	.La69b8

	.pool_aligned

.La69b8:
	cmp	r7, #1
	beq	.La69dc
	cmp	r7, #2
	beq	.La69dc
	cmp	r7, #3
	bne	.La69dc
	ldrb	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.La69dc
	ldrb	r2, [r0, #1]
	mov	r3, r11
	and	r3, r2
	cmp	r3, #0
	bne	.La69dc
.La69d4:
	ldrh	r3, [r6]
	add	r4, #1
	strh	r3, [r5]
	add	r5, #2
.La69dc:
	sub	r1, #1
	add	r6, #4
	cmp	r1, #0
	bge	.La6986
	add	r7, #1
	cmp	r7, r8
	blt	.La697a
.La69ea:
	mov	r0, r4
	add	sp, #8
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a68ec

@ BuildItemScrollState
@ r0 = destination, r1 = which cursor. Identical to Func_a5578 except that it
@ reads the roster through state+0x218 rather than the ability count.
@
@ Fills the seven-word scroll descriptor every list renderer in this module
@ consumes. The five-row page size is baked in as a literal divisor:
@
@     [0x00] the character record from _Func_77394
@     [0x08] index / 5          the page
@     [0x0C] total / 5 rounded up   the page count
@     [0x10] index % 5          the row inside the page
@     [0x14] total
@     [0x18] index, clamped to total - 1
@
@ Returns 1 always. Word [0x04] is left untouched.
.thumb_func_start Func_a6a00
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	mov	r2, #0x86
	ldr	r6, [r3]
	lsl	r2, #2
	mov	r10, r2
	mov	r5, r1
	add	r3, r6, #2
	add	r5, r10
	mov	r9, r0
	ldrb	r0, [r3, r5]
	mov	r8, r3
	bl	_Func_77394
	mov	r2, r10
	ldrb	r7, [r6, r2]
	mov	r2, r8
	ldrb	r3, [r2, r5]
	mov	r2, #0x98
	lsl	r2, #2
	add	r3, r2
	ldrsb	r6, [r6, r3]
	add	r3, r6, #1
	mov	r11, r0
	cmp	r3, r7
	ble	.La6a42
	sub	r6, r7, #1
.La6a42:
	mov	r1, #5
	mov	r0, r6
	bl	Func_af0_from_thumb
	mov	r1, #5
	mov	r10, r0
	mov	r0, r6
	bl	Func_b1c_from_thumb
	mov	r1, #5
	mov	r8, r0
	mov	r0, r7
	bl	Func_af0_from_thumb
	mov	r1, #5
	mov	r5, r0
	mov	r0, r7
	bl	Func_b1c_from_thumb
	cmp	r0, #0
	beq	.La6a6e
	add	r5, #1
.La6a6e:
	mov	r2, r9
	mov	r3, r11
	str	r3, [r2]
	mov	r3, r10
	str	r3, [r2, #8]
	mov	r3, r8
	str	r5, [r2, #0xc]
	str	r3, [r2, #0x10]
	str	r7, [r2, #0x14]
	str	r6, [r2, #0x18]
	mov	r0, #1
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a6a00

@ DrawItemDetail
@ r0, r1 = unused, r2 = scroll descriptor. The Func_a5614 of the item screen:
@ recomputes the absolute index, and unless save bit 0x151 is up prints the
@ item's detail line -- STRING 0x53A + (id & 0x1FF) -- then tints the five rows
@ with palette 0x0E for the selected one and 0x0F for the rest. When 0x151 IS
@ up it clears save bit 0x2FF instead and skips the text, leaving whatever
@ message is on screen alone.
.thumb_func_start Func_a6a98
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r8
	push	{r6, r7}
	mov	r8, r2
	ldr	r3, =iwram_1f2c
	ldr	r2, [r2, #8]
	mov	r1, r8
	ldr	r7, [r3]
	lsl	r3, r2, #2
	add	r3, r2
	ldr	r2, [r1, #0x10]
	add	r3, r2
	str	r3, [r1, #0x18]
	ldr	r0, =0x151
	sub	sp, #8
	bl	_Func_79338
	cmp	r0, #0
	bne	.La6af4
	ldr	r0, [r7, #0x2c]
	bl	_Func_16498
	mov	r0, #1
	bl	Func_30f8
	mov	r2, r8
	ldr	r3, [r2, #0x18]
	mov	r1, #0xe4
	lsl	r3, #1
	lsl	r1, #1
	add	r3, r1
	ldrh	r2, [r7, r3]
	mov	r3, r2
	cmp	r3, #0
	beq	.La6afa
	ldr	r0, =0x1ff
	ldr	r3, =0x53a
	and	r0, r2
	add	r0, r3
	ldr	r1, [r7, #0x2c]
	mov	r2, #0
	mov	r3, #0
	bl	_Func_1e7c0
	b	.La6afa
.La6af4:
	ldr	r0, =0x2ff
	bl	_Func_79374
.La6afa:
	mov	r2, #1
	mov	r6, #0
	mov	r10, r2
	mov	r5, #1
.La6b02:
	mov	r1, r8
	ldr	r3, [r1, #0x10]
	cmp	r6, r3
	bne	.La6b20
	mov	r2, r10
	mov	r3, #0xe
	ldr	r0, [r7, #0x20]
	mov	r1, #0
	str	r2, [sp]
	str	r3, [sp, #4]
	mov	r2, r5
	mov	r3, #0xf
	bl	Func_a2268
	b	.La6b32
.La6b20:
	mov	r3, r10
	ldr	r0, [r7, #0x20]
	mov	r1, #0
	str	r3, [sp]
	mov	r2, r5
	mov	r3, #0xf
	str	r3, [sp, #4]
	bl	Func_a2268
.La6b32:
	add	r6, #1
	add	r5, #2
	cmp	r6, #4
	ble	.La6b02
	mov	r0, #1
	bl	Func_30f8
	mov	r0, #1
	add	sp, #8
	pop	{r3, r5}
	mov	r8, r3
	mov	r10, r5
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a6a98

@ DrawItemPage
@ r0 = window, r1 = unused, r2 = scroll descriptor. Draws one five-row page of
@ items at x 0x70, with the page bar from Func_a21b0 and heading 0xAED. The
@ title is 0xAE1 when bit 1 of state+0x220 is set and 0xB89 otherwise. Each row
@ resolves through _Func_77394 for the owner and _Func_78b9c for the item.
@ 154 lines; traced structurally.
.thumb_func_start Func_a6b64
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	ldr	r3, =iwram_1f2c
	ldr	r3, [r3]
	sub	sp, #4
	mov	r10, r3
	mov	r8, r0
	mov	r6, r2
	bl	_Func_16498
	mov	r3, #0xb
	str	r3, [sp]
	mov	r2, #0xb
	mov	r3, #0x10
	mov	r0, r8
	mov	r1, #0
	bl	_Func_1e41c
	mov	r3, #0x88
	lsl	r3, #2
	add	r3, r10
	ldrh	r2, [r3]
	mov	r3, #2
	and	r3, r2
	cmp	r3, #0
	beq	.La6bb0
	ldr	r0, =0xae1
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_1e7c0
	b	.La6bbc
.La6bb0:
	ldr	r0, =0xb89
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_1e7c0
.La6bbc:
	ldr	r2, [r6, #8]
	lsl	r3, r2, #2
	add	r5, r3, r2
	ldr	r3, [r6, #0x14]
	sub	r3, r5
	lsl	r3, #24
	lsr	r3, #24
	mov	r11, r3
	cmp	r3, #5
	bls	.La6bd4
	mov	r1, #5
	mov	r11, r1
.La6bd4:
	mov	r3, #0x22
	str	r3, [sp]
	mov	r0, #5
	mov	r1, r5
	mov	r2, r8
	mov	r3, #0x70
	bl	Func_a2324
	mov	r2, #0xf
	ldr	r1, [r6, #0x14]
	ldr	r3, [r6, #8]
	mov	r0, r8
	str	r2, [sp]
	mov	r2, #5
	bl	Func_a21b0
	mov	r2, #0x60
	mov	r3, #0
	ldr	r0, =0xaed
	mov	r1, r8
	bl	_Func_1e7c0
	mov	r2, #0
	mov	r3, r11
	mov	r9, r2
	cmp	r3, #0
	bls	.La6c9c
	mov	r1, #0xe4
	lsl	r3, r5, #1
	lsl	r1, #1
	add	r6, r3, r1
.La6c12:
	ldr	r3, =0x21a
	add	r3, r10
	ldrb	r0, [r3]
	bl	_Func_77394
	mov	r2, r10
	ldrh	r3, [r6, r2]
	mov	r5, r0
	ldr	r0, =0x3fff
	and	r0, r3
	bl	_Func_78b9c
	mov	r7, r0
	ldrb	r2, [r7, #9]
	mov	r1, #0x3a
	ldrsh	r3, [r5, r1]
	cmp	r2, r3
	ble	.La6c3e
	mov	r0, #2
	bl	_Func_1e71c
	b	.La6c5c
.La6c3e:
	mov	r2, r10
	ldrh	r3, [r6, r2]
	ldr	r0, =0x3fff
	and	r0, r3
	bl	Func_a735c
	cmp	r0, #0
	beq	.La6c56
	mov	r0, #4
	bl	_Func_1e71c
	b	.La6c5c
.La6c56:
	mov	r0, #0xf
	bl	_Func_1e71c
.La6c5c:
	mov	r1, r10
	ldrh	r3, [r6, r1]
	ldr	r0, =0x3fff
	mov	r2, r9
	and	r0, r3
	lsl	r5, r2, #4
	ldr	r3, =0x333
	add	r5, #8
	add	r0, r3
	mov	r1, r8
	mov	r2, #0x10
	mov	r3, r5
	bl	_Func_1e7c0
	ldrb	r0, [r7, #9]
	mov	r3, #0x68
	mov	r1, #2
	mov	r2, r8
	str	r5, [sp]
	bl	_Func_1e9d4
	mov	r0, #0xf
	bl	_Func_1e71c
	mov	r3, r9
	add	r3, #1
	lsl	r3, #24
	lsr	r3, #24
	mov	r9, r3
	add	r6, #2
	cmp	r11, r9
	bhi	.La6c12
.La6c9c:
	mov	r0, #1
	add	sp, #4
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a6b64

@ RunItemPicker
@ Takes no arguments. The item screen's big loop -- pick an item out of a
@ character's list, with the party sprites reacting through _Func_ba30 and
@ Func_a735c gating which entries can be chosen. Builds the descriptor with
@ Func_a6a00, draws with Func_a6b64 and Func_a6a98, moves with Func_a1fd4, and
@ Func_a65e4 assigns a shortcut when that is what the caller asked for. Title
@ strings 0xAE1 and 0xB89 as in Func_a6b64. 826 lines; traced structurally.
.thumb_func_start Func_a6ccc
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0x64
	str	r0, [sp, #0x30]
	ldr	r1, [sp, #0x30]
	ldr	r3, =iwram_1f2c
	lsl	r1, #2
	ldr	r7, [r3]
	mov	r0, #0
	mov	r3, r1
	str	r0, [sp, #0x2c]
	str	r0, [sp, #0x1c]
	str	r0, [sp, #0x18]
	str	r1, [sp, #0x14]
	add	r3, #0x14
	ldr	r2, [r7, r3]
	mov	r3, #0xd
	strb	r3, [r2, #5]
	mov	r5, r7
	mov	r3, #0xe
	str	r3, [sp]
	add	r5, #0x34
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r0, r5
	mov	r1, #0xd
	mov	r2, #3
	mov	r3, #0x11
	bl	Func_a10d0
	ldr	r5, [r5]
	mov	r2, #0
	mov	r8, r5
	str	r2, [sp, #0x24]
	add	r3, r7, #2
	ldr	r0, [sp, #0x30]
	lsl	r0, #1
	str	r3, [sp, #0x10]
	add	r4, sp, #0x34
	str	r0, [sp, #0xc]
	b	.La72cc
.La6d28:
	ldr	r1, [sp, #0x30]
	mov	r2, #0x86
	lsl	r2, #2
	add	r3, r1, r2
	ldr	r1, [sp, #0x10]
	ldrb	r0, [r1, r3]
	str	r4, [sp, #8]
	bl	_Func_77394
	mov	r2, #0x9a
	str	r0, [sp, #0x20]
	lsl	r2, #2
	add	r3, r7, r2
	ldrb	r3, [r3]
	ldr	r4, [sp, #8]
	cmp	r3, #0
	beq	.La6db2
	mov	r3, #0xe4
	lsl	r3, #1
	add	r1, r7, r3
	mov	r2, #1
	b	.La6dbe
.La6d54:
	mov	r0, #0x82
	b	.La707e
.La6d58:
	mov	r0, #0x71
	str	r4, [sp, #8]
	bl	_Func_f9080
	mov	r0, #1
	neg	r0, r0
	mov	r1, #1
	str	r0, [sp, #0x2c]
	str	r1, [sp, #0x24]
	ldr	r4, [sp, #8]
	b	.La72cc
.La6d6e:
	ldr	r3, [r4, #0x18]
	mov	r2, #0xe4
	lsl	r2, #1
	lsl	r3, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	str	r3, [sp, #0x2c]
	mov	r3, #0x9a
	lsl	r3, #2
	add	r2, r7, r3
	mov	r3, #1
	strb	r3, [r2]
	str	r3, [sp, #0x24]
	b	.La72cc
.La6d8a:
	mov	r0, #0x82
	str	r4, [sp, #8]
	bl	_Func_f9080
	ldr	r4, [sp, #8]
	ldr	r3, [r4, #0x18]
	mov	r0, #0xe4
	lsl	r3, #1
	lsl	r0, #1
	add	r3, r0
	ldrh	r3, [r7, r3]
	mov	r1, #0x9a
	lsl	r1, #2
	str	r3, [sp, #0x2c]
	add	r2, r7, r1
	mov	r3, #2
	strb	r3, [r2]
	mov	r2, #1
	str	r2, [sp, #0x24]
	b	.La72cc
.La6db2:
	mov	r3, #0xe4
	lsl	r3, #1
	add	r1, r7, r3
	ldr	r0, [sp, #0x20]
	mov	r2, #2
	str	r4, [sp, #8]
.La6dbe:
	bl	Func_a68ec
	mov	r1, #0x86
	lsl	r1, #2
	add	r3, r7, r1
	strb	r0, [r3]
	ldr	r4, [sp, #8]
	mov	r2, #0xe4
	lsl	r2, #1
	add	r0, r7, r2
	str	r4, [sp, #8]
	bl	Func_a68a8
	ldr	r4, [sp, #8]
	ldr	r1, [sp, #0x30]
	mov	r0, r4
	bl	Func_a6a00
	mov	r3, #1
	str	r3, [sp, #0x28]
	mov	r9, r3
	ldr	r3, [sp, #0x14]
	ldr	r2, =iwram_1c94
	add	r3, #0x14
	ldr	r3, [r7, r3]
	mov	r1, #4
	mov	r0, r9
	mov	r10, r1
	mov	r11, r2
	strb	r0, [r3, #5]
	ldr	r4, [sp, #8]
	b	.La72b8
.La6dfe:
	ldr	r1, [r4, #0x10]
	lsl	r1, #4
	add	r1, #0x24
	mov	r0, #0x58
	str	r4, [sp, #8]
	bl	Func_a1a40
	mov	r3, r9
	ldr	r4, [sp, #8]
	cmp	r3, #0
	beq	.La6ece
	ldr	r1, [sp, #0x1c]
	mov	r2, #0xe4
	lsl	r3, r1, #1
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	mov	r0, #0
	mov	r9, r0
	cmp	r3, #0
	beq	.La6e34
	lsl	r3, r1, #2
	add	r3, #0x48
	ldr	r0, [r7, r3]
	bl	Func_a17c4
	ldr	r4, [sp, #8]
.La6e34:
	ldr	r3, [sp, #0x28]
	cmp	r3, #0
	beq	.La6e54
	mov	r0, #0
	str	r0, [sp, #0x28]
	mov	r0, #1
	str	r4, [sp, #8]
	bl	Func_30f8
	ldr	r4, [sp, #8]
	mov	r0, r8
	mov	r2, r4
	mov	r1, #0
	bl	Func_a6b64
	ldr	r4, [sp, #8]
.La6e54:
	mov	r2, r4
	add	r1, sp, #0x50
	mov	r0, r8
	str	r4, [sp, #8]
	bl	Func_a6a98
	mov	r3, #0xbc
	ldr	r1, [sp, #0xc]
	ldr	r4, [sp, #8]
	lsl	r3, #1
	add	r2, r1, r3
	ldr	r3, [r4, #0x18]
	mov	r0, #0xe4
	lsl	r0, #1
	lsl	r3, #1
	add	r3, r0
	ldrh	r3, [r7, r3]
	mov	r1, #0x87
	strh	r3, [r7, r2]
	lsl	r1, #2
	add	r3, r7, r1
	ldr	r2, [r3]
	mov	r3, #0xd
	strb	r3, [r2, #5]
	ldr	r2, [r4, #0x18]
	lsl	r3, r2, #1
	add	r3, r0
	ldrh	r3, [r7, r3]
	cmp	r3, #0
	beq	.La6ea0
	lsl	r3, r2, #2
	add	r3, #0x48
	ldr	r0, [r7, r3]
	mov	r3, #9
	strb	r3, [r0, #5]
	mov	r3, #0xfa
	strh	r6, [r0, #0xc]
	strb	r3, [r0, #0xf]
.La6ea0:
	ldr	r3, =0x219
	add	r2, r7, r3
	ldrb	r3, [r2]
	mov	r5, #0
	cmp	r6, r3
	bcs	.La6ece
	mov	r6, r2
.La6eae:
	mov	r0, #0x8a
	lsl	r3, r5, #2
	lsl	r0, #1
	add	r3, r0
	ldr	r0, [r7, r3]
	mov	r1, #1
	str	r4, [sp, #8]
	bl	_Func_ba30
	add	r3, r5, #1
	lsl	r3, #24
	lsr	r5, r3, #24
	ldrb	r3, [r6]
	ldr	r4, [sp, #8]
	cmp	r5, r3
	bcc	.La6eae
.La6ece:
	mov	r0, #1
	str	r4, [sp, #8]
	bl	Func_30f8
	ldr	r4, [sp, #8]
	ldr	r1, [r4, #0x18]
	ldr	r3, =iwram_1ae8
	str	r1, [sp, #0x1c]
	ldr	r3, [r3]
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	bne	.La6efc
	add	r3, sp, #0x3c
	ldr	r1, [r4, #0x14]
	mov	r0, #0
	str	r3, [sp]
	mov	r2, #5
	add	r3, sp, #0x44
	bl	Func_a1fd4
	ldr	r4, [sp, #8]
	b	.La6f00
.La6efc:
	mov	r0, #1
	neg	r0, r0
.La6f00:
	cmp	r0, #1
	bne	.La6f0a
	mov	r3, #1
	str	r3, [sp, #0x28]
	mov	r9, r3
.La6f0a:
	cmp	r0, #0
	bne	.La6f12
	mov	r1, #1
	mov	r9, r1
.La6f12:
	mov	r2, #1
	neg	r2, r2
	cmp	r0, r2
	bne	.La6f1e
	mov	r3, #0
	mov	r9, r3
.La6f1e:
	mov	r0, #0x9a
	lsl	r0, #2
	add	r3, r7, r0
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.La6f2c
	b	.La7010
.La6f2c:
	mov	r1, r11
	ldr	r3, [r1]
	mov	r2, r10
	and	r3, r2
	cmp	r3, #0
	beq	.La6fb0
	ldr	r3, [sp, #0x18]
	cmp	r3, #0
	bne	.La6fb0
	ldr	r3, [r4, #0x18]
	sub	r0, #0xa0
	lsl	r3, #1
	add	r3, r0
	ldrh	r3, [r7, r3]
	ldr	r0, .La6f64	@ 0x3fff
	and	r0, r3
	str	r4, [sp, #8]
	bl	_Func_78b9c
	ldrb	r3, [r0, #0xc]
	ldr	r4, [sp, #8]
	cmp	r3, #0
	bne	.La6f78
	mov	r0, #0x72
	bl	_Func_f9080
	b	.La6fae

	.align	2, 0
.La6f64:
	.word	0x3fff
	.pool

.La6f78:
	mov	r0, #0xae
	str	r4, [sp, #8]
	bl	_Func_f9080
	mov	r1, #1
	mov	r2, #0x88
	str	r1, [sp, #0x18]
	lsl	r2, #2
	add	r1, r7, r2
	ldrh	r2, [r1]
	ldr	r3, =2
	orr	r3, r2
	strh	r3, [r1]
	mov	r3, #0x60
	str	r3, [sp]
	mov	r0, r8
	mov	r1, #0
	mov	r2, #0x58
	mov	r3, #0x78
	bl	_Func_164d4
	ldr	r0, =0xae1
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_1e7c0
.La6fae:
	ldr	r4, [sp, #8]
.La6fb0:
	ldr	r3, =iwram_1ae8
	ldr	r3, [r3]
	mov	r0, r10
	and	r3, r0
	cmp	r3, #0
	bne	.La7010
	b	.La6fcc

	.pool_aligned

.La6fcc:
	ldr	r1, [sp, #0x18]
	cmp	r1, #1
	bne	.La7010
	mov	r2, #0
	mov	r3, #0x88
	str	r2, [sp, #0x18]
	lsl	r3, #2
	add	r1, r7, r3
	ldrh	r2, [r1]
	ldr	r3, =0xfffd
	and	r2, r3
	mov	r3, #0x60
	strh	r2, [r1]
	mov	r0, r8
	str	r3, [sp]
	mov	r1, #0
	mov	r2, #0x58
	mov	r3, #0x78
	str	r4, [sp, #8]
	bl	_Func_164d4
	ldr	r0, =0xb89
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0x58
	bl	_Func_1e7c0
	ldr	r4, [sp, #8]
	b	.La7010

	.pool_aligned

.La7010:
	mov	r0, r11
	ldr	r2, [r0]
	mov	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La709a
	mov	r1, #0x9a
	lsl	r1, #2
	add	r3, r7, r1
	ldrb	r3, [r3]
	cmp	r3, #0
	beq	.La702a
	b	.La6d54
.La702a:
	ldr	r3, [r4, #0x18]
	mov	r0, #0xe4
	lsl	r3, #1
	lsl	r0, #1
	add	r2, r3, r0
	ldrh	r3, [r7, r2]
	cmp	r3, #0
	beq	.La709a
	mov	r0, r3
	str	r4, [sp, #8]
	bl	Func_a735c
	ldr	r4, [sp, #8]
	cmp	r0, #0
	beq	.La704a
	b	.La727c
.La704a:
	ldr	r3, [r4, #0x18]
	mov	r1, #0xe4
	lsl	r1, #1
	lsl	r3, #1
	add	r3, r1
	ldrh	r3, [r7, r3]
	ldr	r0, =0x3fff
	and	r0, r3
	str	r4, [sp, #8]
	bl	_Func_78b9c
	ldr	r1, [sp, #0x20]
	ldrb	r2, [r0, #9]
	mov	r0, #0x3a
	ldrsh	r3, [r1, r0]
	ldr	r4, [sp, #8]
	cmp	r2, r3
	ble	.La707c
	mov	r0, #0x72
	bl	_Func_f9080
	ldr	r4, [sp, #8]
	b	.La709a

	.pool_aligned

.La707c:
	mov	r0, #0xad
.La707e:
	str	r4, [sp, #8]
	bl	_Func_f9080
	ldr	r4, [sp, #8]
	ldr	r3, [r4, #0x18]
	mov	r2, #0xe4
	lsl	r3, #1
	lsl	r2, #1
	add	r3, r2
	ldrh	r3, [r7, r3]
	str	r3, [sp, #0x2c]
	mov	r3, #1
	str	r3, [sp, #0x24]
	b	.La72cc
.La709a:
	mov	r0, r11
	ldr	r2, [r0]
	mov	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La70a8
	b	.La6d58
.La70a8:
	ldr	r1, =iwram_1b04
	ldr	r2, [r1]
	add	r3, #0xfe
	and	r2, r3
	cmp	r2, #0
	bne	.La70c2
	ldr	r2, [r1]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	bne	.La70c2
	b	.La71c8
.La70c2:
	ldr	r3, =iwram_1ae8
	ldr	r3, [r3]
	mov	r1, r10
	and	r3, r1
	cmp	r3, #0
	beq	.La70d0
	b	.La71c8
.La70d0:
	mov	r2, #0x9a
	lsl	r2, #2
	add	r3, r7, r2
	ldrb	r3, [r3]
	neg	r5, r3
	orr	r5, r3
	mov	r0, #0x6f
	mov	r3, #2
	lsr	r5, #31
	sub	r5, r3, r5
	str	r4, [sp, #8]
	bl	_Func_f9080
	mov	r1, #0x86
	ldr	r0, [sp, #0x30]
	ldr	r2, [sp, #0x10]
	lsl	r1, #2
	add	r3, r0, r1
	ldr	r4, [sp, #8]
	ldrb	r3, [r2, r3]
	mov	r0, #0x98
	lsl	r0, #2
	ldr	r2, [r4, #0x18]
	add	r3, r0
	strb	r2, [r7, r3]
	ldr	r1, [sp, #0x30]
	add	r1, #0x1c
	ldrsb	r6, [r7, r1]
	mov	r9, r1
	lsl	r5, #24
.La710c:
	ldr	r3, =iwram_1b04
	mov	r2, #0x80
	ldr	r3, [r3]
	lsl	r2, #1
	and	r3, r2
	cmp	r3, #0
	beq	.La7128
	add	r6, #1
	b	.La712a

	.pool_aligned

.La7128:
	sub	r6, #1
.La712a:
	ldr	r2, =0x219
	add	r3, r7, r2
	ldrb	r1, [r3]
	add	r0, r6, r1
	str	r4, [sp, #8]
	bl	Func_b1c_from_thumb
	mov	r6, r0
	lsl	r3, r6, #1
	mov	r2, #0x82
	mov	r10, r3
	lsl	r2, #2
	add	r2, r10
	ldrh	r3, [r7, r2]
	ldr	r0, =0x21a
	str	r3, [r7, #8]
	ldrh	r2, [r7, r2]
	add	r3, r7, r0
	strb	r2, [r3]
	ldrb	r0, [r3]
	bl	_Func_77394
	mov	r2, #0xe4
	lsl	r2, #1
	add	r1, r7, r2
	asr	r2, r5, #24
	bl	Func_a68ec
	mov	r1, #0x86
	lsl	r1, #2
	add	r3, r7, r1
	strb	r0, [r3]
	lsl	r0, #24
	ldr	r4, [sp, #8]
	cmp	r0, #0
	beq	.La710c
	mov	r2, r9
	strb	r6, [r7, r2]
	ldr	r2, .La71ac	@ 0x1e
	mov	r5, #0
	sub	r1, #0xd4
.La717c:
	lsl	r3, r5, #1
	add	r3, r1
	strh	r2, [r7, r3]
	add	r3, r5, #1
	lsl	r3, #24
	lsr	r5, r3, #24
	cmp	r5, #3
	bls	.La717c
	mov	r2, #0xa2
	ldr	r3, .La71b0	@ 0x1a
	lsl	r2, #1
	add	r2, r10
	mov	r5, #0x82
	strh	r3, [r7, r2]
	lsl	r5, #2
	add	r5, r10
	ldr	r0, [r7, #0x24]
	ldrh	r1, [r7, r5]
	mov	r2, #0
	mov	r3, #0
	str	r4, [sp, #8]
	bl	Func_a112c
	b	.La71bc

	.align	2, 0
.La71ac:
	.word	0x1e
.La71b0:
	.word	0x1a
	.pool

.La71bc:
	ldrh	r1, [r7, r5]
	mov	r0, r7
	bl	Func_a1804
	ldr	r4, [sp, #8]
	b	.La72cc
.La71c8:
	mov	r3, r11
	ldr	r2, [r3]
	mov	r3, #0x80
	lsl	r3, #2
	and	r2, r3
	cmp	r2, #0
	beq	.La7244
	ldr	r3, =iwram_1ae8
	ldr	r3, [r3]
	mov	r0, r10
	and	r3, r0
	cmp	r3, #0
	beq	.La7244
	ldr	r3, [r4, #0x18]
	mov	r1, #0xe4
	lsl	r1, #1
	lsl	r3, #1
	add	r3, r1
	ldrh	r3, [r7, r3]
	ldr	r0, .La720c	@ 0x3fff
	and	r0, r3
	str	r4, [sp, #8]
	bl	_Func_78b9c
	ldrb	r3, [r0, #0xc]
	ldr	r4, [sp, #8]
	cmp	r3, #0
	bne	.La7214
	mov	r0, #0x72
	bl	_Func_f9080
	ldr	r4, [sp, #8]
	b	.La7244

	.align	2, 0
.La720c:
	.word	0x3fff
	.pool

.La7214:
	mov	r0, #0x82
	str	r4, [sp, #8]
	bl	_Func_f9080
	mov	r0, #0x86
	ldr	r2, [sp, #0x30]
	ldr	r1, [sp, #0x10]
	ldr	r4, [sp, #8]
	lsl	r0, #2
	add	r3, r2, r0
	ldrb	r0, [r1, r3]
	ldr	r3, [r4, #0x18]
	mov	r2, #0xe4
	lsl	r2, #1
	lsl	r3, #1
	add	r3, r2
	ldrh	r1, [r7, r3]
	mov	r2, #0
	bl	Func_a65e4
	ldr	r4, [sp, #8]
	cmp	r0, #0
	beq	.La7244
	b	.La6d6e
.La7244:
	mov	r3, r11
	ldr	r2, [r3]
	mov	r3, #0x80
	lsl	r3, #1
	and	r2, r3
	cmp	r2, #0
	beq	.La72b8
	ldr	r3, =iwram_1ae8
	ldr	r3, [r3]
	mov	r0, r10
	and	r3, r0
	cmp	r3, #0
	beq	.La72b8
	ldr	r3, [r4, #0x18]
	mov	r1, #0xe4
	lsl	r1, #1
	lsl	r3, #1
	add	r3, r1
	ldrh	r3, [r7, r3]
	ldr	r0, .La7288	@ 0x3fff
	and	r0, r3
	str	r4, [sp, #8]
	bl	_Func_78b9c
	ldrb	r3, [r0, #0xc]
	ldr	r4, [sp, #8]
	cmp	r3, #0
	bne	.La7290
.La727c:
	mov	r0, #0x72
	bl	_Func_f9080
	ldr	r4, [sp, #8]
	b	.La72b8

	.align	2, 0
.La7288:
	.word	0x3fff
	.pool

.La7290:
	ldr	r2, [sp, #0x30]
	mov	r0, #0x86
	ldr	r1, [sp, #0x10]
	lsl	r0, #2
	add	r3, r2, r0
	ldrb	r0, [r1, r3]
	ldr	r3, [r4, #0x18]
	mov	r2, #0xe4
	lsl	r2, #1
	lsl	r3, #1
	add	r3, r2
	ldrh	r1, [r7, r3]
	mov	r2, #1
	str	r4, [sp, #8]
	bl	Func_a65e4
	ldr	r4, [sp, #8]
	cmp	r0, #0
	beq	.La72b8
	b	.La6d8a
.La72b8:
	mov	r0, #0xa8
	lsl	r0, #1
	str	r4, [sp, #8]
	bl	_Func_79338
	mov	r6, r0
	ldr	r4, [sp, #8]
	cmp	r6, #0
	bne	.La72cc
	b	.La6dfe
.La72cc:
	ldr	r3, [sp, #0x24]
	cmp	r3, #0
	bne	.La72e4
	mov	r0, #0xa8
	lsl	r0, #1
	str	r4, [sp, #8]
	bl	_Func_79338
	ldr	r4, [sp, #8]
	cmp	r0, #0
	bne	.La72e4
	b	.La6d28
.La72e4:
	mov	r0, #0x88
	lsl	r0, #2
	add	r1, r7, r0
	ldrh	r2, [r1]
	ldr	r3, =0xfffd
	and	r3, r2
	strh	r3, [r1]
	ldr	r0, [r7, #0x44]
	str	r4, [sp, #8]
	bl	Func_a17c4
	mov	r3, #0xba
	ldr	r1, [sp, #0xc]
	ldr	r4, [sp, #8]
	lsl	r3, #1
	add	r2, r1, r3
	ldr	r3, [r4, #0x18]
	strh	r3, [r7, r2]
	ldr	r0, [sp, #0x30]
	mov	r1, #0x86
	ldr	r2, [sp, #0x10]
	lsl	r1, #2
	add	r3, r0, r1
	ldrb	r3, [r2, r3]
	mov	r0, #0x98
	ldr	r2, [r4, #0x18]
	lsl	r0, #2
	add	r3, r0
	strb	r2, [r7, r3]
	ldr	r1, [sp, #0xc]
	mov	r2, #0xbc
	add	r0, sp, #0x2c
	ldrh	r0, [r0]
	lsl	r2, #1
	add	r3, r1, r2
	strh	r0, [r7, r3]
	mov	r0, #0xa8
	lsl	r0, #1
	bl	_Func_79338
	cmp	r0, #0
	beq	.La733e
	mov	r1, #1
	neg	r1, r1
	str	r1, [sp, #0x2c]
.La733e:
	mov	r0, #1
	bl	Func_30f8
	ldr	r0, [sp, #0x2c]
	add	sp, #0x64
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_a6ccc

@ IsItemSelectable
@ r0 = item id, masked to 14 bits. Resolves the display record and returns 1
@ only when its +0x0C is zero AND bits 6 and 7 of +0x01 are not both set --
@ that is, the entry is a plain item and not flagged as hidden. Everything else
@ returns 0.
.thumb_func_start Func_a735c
	push	{lr}
	lsl	r0, #18
	lsr	r0, #18
	bl	_Func_78b9c
	ldrb	r3, [r0, #0xc]
	cmp	r3, #0
	bne	.La7378
	ldrb	r2, [r0, #1]
	mov	r3, #0xc0
	and	r3, r2
	mov	r0, #1
	cmp	r3, #0xc0
	bne	.La737a
.La7378:
	mov	r0, #0
.La737a:
	pop	{r1}
	bx	r1
.func_end Func_a735c

	.section .rodata

.Laeb4c:
	.incrom 0xaeb4c, 0xaebcc
.Laebcc:
	.incrom 0xaebcc, 0xaed4c
