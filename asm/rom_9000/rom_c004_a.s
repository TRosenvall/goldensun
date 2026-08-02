	.include "macros.inc"
	.include "gba.inc"

@ InitEntitySystem
@ r0=mode. Brings up the entity layer on top of the sprite layer:
@   - allocates the 0x5C-byte system header (tag 6) and the 0x1C00-byte entity
@     table (tag 5) with galloc_ewram, then DMA zero-fills both
@   - InitSprites initialises the underlying actor/part pools
@   - registers the per-frame update task at priority 0xC8A via StartTask:
@     Func_d340 (the 14-slot x/z loop) in mode 4, otherwise UpdateActors
@   - registers the draw task at priority 0xC80: Func_c880 (projected 3D) for
@     modes 3 and 4, otherwise Func_c62c (2D map-relative), which also clears
@     iwram_1d1c and iwram_1cc0
@   - seeds the header's default palette (+0x06 = 0x0F) and flags (+0x07 = 0)
.thumb_func_start InitActors  @ 0x0800c004
	push	{r5, r6, r7, lr}
	mov	r7, r8
	push	{r7}
	mov	r7, r0
	mov	r1, #0x5c
	mov	r0, #6
	sub	sp, #4
	bl	galloc_ewram
	mov	r1, #0xe0
	lsl	r1, #5
	mov	r8, r0
	mov	r0, #5
	bl	galloc_ewram
	mov	r6, r0
	mov	r0, r7
	bl	InitSprites
	mov	r5, #0
	mov	r4, sp
	str	r5, [r4]
	ldr	r3, =REG_DMA3SAD
	mov	r0, r4
	mov	r1, r6
	ldr	r2, =0x85000700
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	str	r5, [r4]
	mov	r0, r4
	mov	r1, r8
	ldr	r2, =0x85000017
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	cmp	r7, #4
	bne	.Lc056
	ldr	r0, =Func_800d340
	ldr	r1, =0xc8a
	bl	StartTask
	b	.Lc05e
.Lc056:
	ldr	r0, =UpdateActors
	ldr	r1, =0xc8a
	bl	StartTask
.Lc05e:
	sub	r3, r7, #3
	cmp	r3, #1
	bhi	.Lc070
	mov	r1, #0xc8
	ldr	r0, =Func_800c880
	lsl	r1, #4
	bl	StartTask
	b	.Lc084
.Lc070:
	mov	r1, #0xc8
	ldr	r0, =Func_800c62c
	lsl	r1, #4
	bl	StartTask
	ldr	r3, =iwram_3001d1c
	mov	r2, #0
	str	r2, [r3]
	ldr	r3, =iwram_3001cc0
	str	r2, [r3]
.Lc084:
	mov	r1, r8
	mov	r2, #0
	mov	r3, #0xf
	strb	r3, [r1, #6]
	strb	r2, [r1, #7]
	add	sp, #4
	pop	{r3}
	mov	r8, r3
	pop	{r5, r6, r7}
	pop	{r0}
	bx	r0
.func_end InitActors

