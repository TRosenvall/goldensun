	.include "macros.inc"
	.include "gba.inc"

@ ApplyRasterBlend
@ Takes no arguments. Allocates the 0x540-byte overlay state in mode 9, then
@ programs the blend: REG_BLDCNT = 0x3F42, REG_BLDALPHA = 0x0C04, and three
@ fields inside the state block at [iwram_1ecc] -- +0x534 = 0x3F3F,
@ +0x536 = 0x1F, +0x52A = 0x0A. The same block is set up again by OvlFunc_200
@ on map load, so this is the re-entry path.
.thumb_func_start OvlFunc_919_200815c
	push	{lr}
	mov	r0, #9
	bl	__Func_808fe38
	ldr	r2, =REG_BLDCNT
	ldr	r3, .L190	@ 0x3f42
	strh	r3, [r2]
	ldr	r3, .L194	@ 0xc04
	add	r2, #2
	strh	r3, [r2]
	ldr	r3, =iwram_3001ecc
	ldr	r2, [r3]
	ldr	r3, =0x534
	add	r1, r2, r3
	ldr	r3, =0x3f3f
	strh	r3, [r1]
	ldr	r3, =0x536
	add	r1, r2, r3
	mov	r3, #0x1f
	strh	r3, [r1]
	ldr	r3, =0x52a
	add	r2, r3
	mov	r3, #0xa
	strh	r3, [r2]
	b	.L1b0

	.align	2, 0
.L190:
	.word	0x3f42
.L194:
	.word	0xc04
	.pool

.L1b0:
	pop	{r0}
	bx	r0
.func_end OvlFunc_919_200815c

