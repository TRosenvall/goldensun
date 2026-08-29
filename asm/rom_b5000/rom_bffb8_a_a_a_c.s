	.include "macros.inc"
	.include "gba.inc"

@ SetWindowRegisters
@ r0.. = parameters. Writes the window registers (REG_WIN0H, REG_WINOUT) for
@ the battle scene.
.thumb_func_start Func_80c00d8  @ 0x080c00d8
	push	{r5, r6, lr}
	mov	r1, #0x80
	mov	r2, #1
	ldr	r5, =Func_80008d8
	lsl	r1, #1
	neg	r2, r2
	mov	r6, r0
	bl	_call_via_r5
	mov	r3, #0x80
	lsl	r3, #1
	add	r6, r3
	mov	r0, r6
	mov	r1, #0x80
	ldr	r2, =0x3ff03ff
	bl	_call_via_r5
	ldr	r2, =ewram_2010200
	ldr	r1, =0x20002
	add	r6, #0x80
	mov	r3, #0
.Lc0102:
	add	r3, #1
	stmia	r6!, {r2}
	add	r2, r1
	cmp	r3, #0xef
	bls	.Lc0102
	mov	r1, #0xa0
	ldr	r3, =Func_80008d8
	mov	r0, r6
	lsl	r1, #2
	ldr	r2, =0x3ff03ff
	bl	_call_via_r3
	pop	{r5, r6}
	pop	{r0}
	bx	r0
.func_end Func_80c00d8

@ SetBackgroundRegisters
@ r0.. = parameters. Writes REG_BG0CNT / REG_BG1CNT for the battle scene.
.thumb_func_start Func_80c0130  @ 0x080c0130
	push	{lr}
	ldr	r2, =iwram_3001f00
	ldr	r3, [r2]
	ldr	r3, [r3, #8]
	cmp	r3, #2
	bne	.Lc016a
	mov	r3, r2
	sub	r3, #0x88
	ldr	r4, [r3]
	ldr	r3, [r4]
	lsl	r0, r3, #2
	add	r0, r3
	lsl	r0, #6
	add	r0, r4, r0
	ldrh	r3, [r0, #0x20]
	ldr	r1, =REG_BG2CNT
	strh	r3, [r1]
	ldr	r3, =REG_DMA0SAD
	ldr	r2, =0xa2600001
	add	r0, #0x22
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, r4
	add	r3, #0x24
	add	r0, #0x10
	add	r1, #0x14
	ldr	r2, =0x84000004
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
.Lc016a:
	pop	{r0}
	bx	r0
.func_end Func_80c0130
