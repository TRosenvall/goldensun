	.include "macros.inc"
	.include "gba.inc"

@ PaintDjinnBackground
@ Takes no arguments. Sets the Djinn screen's whole display up. Opens the body
@ window at (0, 5, 0x1E, 0xF), copies 0x2000 bytes of tiles and 0x80 of palette
@ out of the state+0x184 scratch into VRAM, fills the char block with
@ 0x33333333 and the palette with 0x55555555, lays out the grid with
@ _Func_21a18, blits Data_af26c over 0x60052C0, pulls the string table in with
@ Func_45e8 and DMA3s two more palette blocks. Func_aac84(8) brightens the
@ result and Func_aafb8 draws the contents.
.thumb_func_start Func_80aad10  @ 0x080aad10
	push	{r5, r6, lr}
	mov	r6, r8
	push	{r6}
	ldr	r3, =iwram_3001f2c
	mov	r2, #0xc2
	ldr	r0, [r3]
	lsl	r2, #1
	add	r3, r0, r2
	ldr	r3, [r3]
	sub	sp, #8
	mov	r8, r3
	mov	r3, #0xf
	str	r3, [sp]
	mov	r3, #2
	str	r3, [sp, #4]
	mov	r1, #0
	mov	r3, #0x1e
	mov	r2, #5
	add	r0, #0x30
	bl	Func_80a10d0
	mov	r0, #1
	bl	WaitFrames
	mov	r0, r8
	mov	r2, #0x80
	ldr	r6, =Func_8001af8
	ldr	r1, =0x6004000
	lsl	r2, #6
	add	r0, #0xa8
	bl	_call_via_r6
	ldr	r0, =0x20a8
	ldr	r1, =0x5000080
	add	r0, r8
	mov	r2, #0x80
	bl	_call_via_r6
	mov	r1, #0x80
	ldr	r5, =Func_80008d8
	lsl	r1, #6
	ldr	r2, =0x33333333
	ldr	r0, =0x6004000
	bl	_call_via_r5
	mov	r1, #0x80
	ldr	r2, =0x55555555
	ldr	r0, =0x5000080
	bl	_call_via_r5
	ldr	r0, =0x6005000
	bl	_Func_8021a18
	ldr	r1, =Data_af26c
	mov	r2, #0x20
	ldr	r0, =0x60052c0
	bl	_call_via_r6
	bl	GetSpritePalette
	ldr	r3, =REG_DMA3SAD
	ldr	r1, =0x50000a0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r5, =0x50001e8
	ldr	r2, =0x50000bc
	ldrh	r3, [r5]
	ldr	r0, =0x50001e0
	strh	r3, [r2]
	add	r1, #0x40
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, #8
	bl	Func_80aac84
	ldrh	r3, [r5]
	ldr	r2, =0x50000e8
	strh	r3, [r2]
	ldrh	r3, [r5]
	sub	r2, #0x20
	strh	r3, [r2]
	mov	r0, r8
	bl	Func_80aafb8
	add	sp, #8
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6}
	pop	{r1}
	bx	r1
.func_end Func_80aad10

@ ComputeDjinnLayout
@ r0.. = parameters. Pure arithmetic over the 0x3FFF / 0x4000 id space, no
@ calls out. 173 lines; traced structurally.
.thumb_func_start Func_80aae14  @ 0x080aae14
	push	{r5, r6, r7, lr}
	mov	r7, r11
	mov	r6, r10
	mov	r5, r9
	push	{r5, r6, r7}
	mov	r7, r8
	push	{r7}
	sub	sp, #0xc
	mov	r8, r1
	mov	r1, #0
	str	r2, [sp, #8]
	str	r3, [sp, #4]
	str	r1, [sp]
	mov	r2, r8
	ldrh	r3, [r2]
	mov	r12, r0
	mov	r10, r1
	mov	r11, r1
	cmp	r3, #0
	beq	.Laaeac
	ldr	r3, =0x3fff
	ldr	r5, [sp, #8]
	mov	r14, r3
	mov	r0, r8
	sub	r5, #2
.Laae46:
	ldrh	r2, [r0]
	mov	r3, r14
	and	r3, r2
	strh	r3, [r5, #2]
	mov	r1, #1
	add	r10, r1
	mov	r1, r12
	ldrh	r2, [r1]
	ldrh	r3, [r0]
	eor	r3, r2
	mov	r2, r14
	and	r3, r2
	add	r5, #2
	mov	r4, #0
	cmp	r3, #0
	beq	.Laae7e
	ldr	r7, .Laae90	@ 0x3fff
	mov	r6, r0
.Laae6a:
	add	r4, #1
	cmp	r4, #0x1f
	bgt	.Laae7e
	add	r1, #4
	ldrh	r3, [r6]
	ldrh	r2, [r1]
	eor	r3, r2
	and	r3, r7
	cmp	r3, #0
	bne	.Laae6a
.Laae7e:
	cmp	r4, #0x20
	bne	.Laae9c
	mov	r3, #1
	add	r11, r3
	ldr	r2, .Laae94	@ 0x8000
	ldrh	r3, [r5]
	orr	r3, r2
	strh	r3, [r5]
	b	.Laae9c

	.align	2, 0
.Laae90:
	.word	0x3fff
.Laae94:
	.word	0x8000
	.pool

.Laae9c:
	mov	r3, r8
	add	r0, #4
	add	r3, #0x7c
	cmp	r0, r3
	bgt	.Laaeac
	ldrh	r3, [r0]
	cmp	r3, #0
	bne	.Laae46
.Laaeac:
	mov	r2, r12
	ldrh	r3, [r2]
	mov	r1, #0
	mov	r9, r1
	cmp	r3, #0
	beq	.Laaf38
	mov	r1, r10
	ldr	r2, [sp, #8]
	lsl	r3, r1, #1
	mov	r14, r12
	add	r0, r3, r2
	mov	r7, #0
.Laaec4:
	mov	r1, r12
	ldrh	r3, [r7, r1]
	mov	r1, r8
	ldrh	r2, [r1]
	eor	r3, r2
	ldr	r2, =0x3fff
	and	r3, r2
	mov	r4, #0
	cmp	r3, #0
	beq	.Laaef0
	ldr	r6, .Laaf04	@ 0x3fff
	mov	r5, r14
.Laaedc:
	add	r4, #1
	cmp	r4, #0x1f
	bgt	.Laaef0
	add	r1, #4
	ldrh	r3, [r5]
	ldrh	r2, [r1]
	eor	r3, r2
	and	r3, r6
	cmp	r3, #0
	bne	.Laaedc
.Laaef0:
	cmp	r4, #0x20
	bne	.Laaf1a
	ldr	r3, [sp]
	add	r3, #1
	str	r3, [sp]
	mov	r1, r12
	ldrh	r3, [r7, r1]
	ldr	r2, =0x3fff
	b	.Laaf0c

	.align	2, 0
.Laaf04:
	.word	0x3fff
	.pool

.Laaf0c:
	and	r2, r3
	ldr	r3, =0x4000
	orr	r2, r3
	strh	r2, [r0]
	mov	r2, #1
	add	r0, #2
	add	r10, r2
.Laaf1a:
	mov	r1, #1
	add	r9, r1
	mov	r3, #4
	mov	r2, r9
	add	r7, #4
	add	r14, r3
	cmp	r2, #0x1f
	bgt	.Laaf38
	mov	r1, r12
	ldrh	r3, [r7, r1]
	cmp	r3, #0
	bne	.Laaec4
	b	.Laaf38

	.pool_aligned

.Laaf38:
	ldr	r3, [sp, #4]
	mov	r2, r11
	str	r2, [r3]
	ldr	r1, [sp]
	ldr	r3, [sp, #0x2c]	@ 0x2c
	mov	r0, r10
	str	r1, [r3]
	add	sp, #0xc
	pop	{r3, r5, r6, r7}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	mov	r11, r7
	pop	{r5, r6, r7}
	pop	{r1}
	bx	r1
.func_end Func_80aae14
