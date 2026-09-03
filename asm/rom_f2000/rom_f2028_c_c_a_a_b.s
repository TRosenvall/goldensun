	.include "macros.inc"
	.include "gba.inc"

@ StartPaletteFadeEngine
@ Takes no arguments. Takes 0x3004 bytes under tag 0x20 -- the block iwram_1ed0
@ points at -- and snapshots both palettes into the front of it: 0x200 bytes from
@ 0x5000000 and 0x200 from 0x5000200, so +0x000..+0x400 is the packed 512-colour
@ copy. Func_f3078 then unpacks it into the working buffer at +0x1000, and
@ Func_f2f10 is registered at sort key 0xC80.
@
@ The block's layout, from here and Func_f3858:
@
@     +0x0000  the packed snapshot, and a step index halfword at +0
@     +0x0400  the CURRENT unpacked palette, 0x600 halfwords
@     +0x1000  the TARGET unpacked palette
@     +0x1C00  the per-frame step from Func_f2ebc
@     +0x3001  frames remaining     +0x3002  a phase flag
.thumb_func_start Func_80f377c  @ 0x080f377c
	push	{lr}
	ldr	r1, =0x3004
	mov	r0, #0x20
	sub	sp, #4
	bl	galloc_ewram
	mov	r3, #0
	mov	r4, r0
	mov	r0, sp
	str	r3, [r0]
	mov	r1, r4
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000c01
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r0, #0xa0
	lsl	r0, #19
	ldr	r2, =0x84000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r2, #0x80
	lsl	r2, #2
	add	r1, r4, r2
	ldr	r0, =0x5000200
	ldr	r2, =0x84000080
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r3, #0x80
	lsl	r3, #5
	mov	r0, #0x80
	add	r2, r4, r3
	mov	r1, r4
	mov	r3, #0
	lsl	r0, #9
	bl	Func_80f3078
	mov	r1, #0xc8
	lsl	r1, #4
	ldr	r0, =Func_80f2f10
	bl	StartTask
	add	sp, #4
	pop	{r0}
	bx	r0
.func_end Func_80f377c
