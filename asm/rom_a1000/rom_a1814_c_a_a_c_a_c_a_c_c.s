	.include "macros.inc"
	.include "gba.inc"

@ SyncObjPaletteToBg
@ Takes no arguments. DMA3-copies OBJ palette bank 0 (0x5000200) down into BG
@ bank 14 (0x50001C0), plus one further colour. Keeps the text drawn into the
@ tilemap the same colours as the sprites drawn over it.
.thumb_func_start Func_80a22f4  @ 0x080a22f4
	ldr	r3, =REG_DMA3SAD
	ldr	r0, =0x5000200
	ldr	r1, =0x50001c0
	ldr	r2, =0x80000010
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	add	r1, #0x1c
	ldr	r0, =0x50001e8
	ldr	r2, =0x80000001
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	bx	lr
.func_end Func_80a22f4

@ ShowNodeRun
@ r0 = count, r1 = first index, r2 = unused, r3 = x, arg5 = y.
@ Hides all 32 nodes at state+0x48 by setting each one's +0x05 to 0x0D, then
@ walks `count` of them from `first`, placing each at x and a y that steps down
@ by 0x10, rewinding it with Func_a17c4 and marking it live. Stops early on a
@ null slot or once the index passes the visible-row count at state+0x218.
.thumb_func_start Func_80a2324  @ 0x080a2324
	push	{r5, r6, r7, lr}
	mov	r7, r10
	mov	r6, r9
	mov	r5, r8
	push	{r5, r6, r7}
	mov	r9, r3
	ldr	r3, =iwram_3001f2c
	ldr	r3, [r3]
	sub	sp, #4
	mov	r8, r3
	mov	r2, #0xd
	add	r3, #0x48
	mov	r6, #0x1f
.La233e:
	ldmia	r3!, {r5}
	cmp	r5, #0
	beq	.La2346
	strb	r2, [r5, #5]
.La2346:
	sub	r6, #1
	cmp	r6, #0
	bge	.La233e
	mov	r6, r1
	add	r0, r6
	cmp	r6, r0
	bge	.La23ac
	lsl	r2, r6, #2
	mov	r3, r2
	add	r3, #0x48
	mov	r1, r8
	ldr	r5, [r1, r3]
	cmp	r5, #0
	beq	.La23ac
	mov	r3, #0x86
	lsl	r3, #2
	add	r3, r8
	ldrb	r3, [r3]
	sub	r3, #1
	cmp	r6, r3
	bgt	.La23ac
	add	r3, r2, r1
	mov	r2, r3
	ldr	r7, [sp, #0x20]
	mov	r10, r0
	add	r2, #0x48
.La237a:
	mov	r3, r9
	strh	r3, [r5, #6]
	strh	r7, [r5, #8]
	mov	r0, r5
	str	r2, [sp]
	bl	Func_80a17c4
	add	r6, #1
	mov	r3, #1
	strb	r3, [r5, #5]
	add	r7, #0x10
	ldr	r2, [sp]
	cmp	r6, r10
	bge	.La23ac
	add	r2, #4
	ldr	r5, [r2]
	cmp	r5, #0
	beq	.La23ac
	mov	r3, #0x86
	lsl	r3, #2
	add	r3, r8
	ldrb	r3, [r3]
	sub	r3, #1
	cmp	r6, r3
	ble	.La237a
.La23ac:
	add	sp, #4
	pop	{r3, r5, r6}
	mov	r8, r3
	mov	r9, r5
	mov	r10, r6
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a2324
