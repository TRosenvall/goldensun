	.include "macros.inc"
	.include "gba.inc"

@ TintTilemapRect
@ r0 = window, r1 = x, r2 = y, r3 = width, arg5 = height, arg6 = palette bank.
@ ORs the bank into bits 12..15 of every tilemap entry in the rectangle, which
@ recolours what is already drawn without touching the tile indices. Coordinates
@ are relative to the window's own origin and clipped to the 30x20 map, and the
@ dirty byte at [iwram_1e8c]+0xEA3 is raised so the next frame uploads it.
.thumb_func_start Func_80a2268  @ 0x080a2268
	push	{r5, r6, r7, lr}
	mov	r6, r3
	ldr	r3, =iwram_3001e8c
	ldr	r3, [r3]
	mov	r12, r3
	ldrh	r3, [r0, #0xc]
	add	r3, r1, r3
	add	r1, r3, #1
	ldrh	r3, [r0, #0xe]
	ldr	r7, [sp, #0x14]
	add	r3, r2, r3
	ldr	r5, [sp, #0x10]
	add	r2, r3, #1
	lsl	r7, #12
	cmp	r1, #0
	bge	.La228c
	add	r6, r1
	mov	r1, #0
.La228c:
	add	r3, r1, r6
	cmp	r3, #0x1d
	ble	.La2296
	mov	r3, #0x1e
	sub	r6, r3, r1
.La2296:
	cmp	r2, #0
	bge	.La229e
	add	r5, r2
	mov	r2, #0
.La229e:
	add	r3, r2, r5
	cmp	r3, #0x1d
	ble	.La22a8
	mov	r3, #0x14
	sub	r5, r3, r2
.La22a8:
	cmp	r6, #0
	ble	.La22e2
	cmp	r5, #0
	ble	.La22e2
	lsl	r2, #6
	lsl	r3, r1, #1
	add	r1, r2, r3
.La22b6:
	mov	r3, r12
	mov	r0, r6
	add	r4, r1, r3
	cmp	r0, #0
	beq	.La22d2
	ldr	r2, =0xffffefff
.La22c2:
	ldrh	r3, [r4]
	and	r3, r2
	orr	r3, r7
	sub	r0, #1
	strh	r3, [r4]
	add	r4, #2
	cmp	r0, #0
	bne	.La22c2
.La22d2:
	sub	r5, #1
	add	r1, #0x40
	cmp	r5, #0
	bne	.La22b6
	ldr	r2, =0xea3
	mov	r3, #1
	add	r2, r12
	strb	r3, [r2]
.La22e2:
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end Func_80a2268

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
