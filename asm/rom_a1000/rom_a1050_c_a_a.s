	.include "macros.inc"
	.include "gba.inc"

@ ResetCursorSprite
@ r0 = ignored. DMA3-fills 0x29C bytes of the state block with zero, then sets
@ +0x1C to 0xFF and +0x1E, +0x1F, +0x112, +0x113 to 1. The block-clear is what
@ makes the shared tag 0x37 scratch safe to reuse between screens.
.thumb_func_start Func_80a1090  @ 0x080a1090
	ldr	r3, =iwram_3001f2c
	sub	sp, #4
	ldr	r4, [r3]
	mov	r0, sp
	mov	r3, #0
	str	r3, [r0]
	mov	r1, r4
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x8500029c
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	mov	r1, #0x89
	mov	r3, #0xff
	lsl	r1, #1
	strb	r3, [r4, #0x1c]
	add	r2, r4, r1
	mov	r3, #1
	add	r1, #1
	strb	r3, [r4, #0x1e]
	strb	r3, [r4, #0x1f]
	strb	r3, [r2]
	add	r2, r4, r1
	strb	r3, [r2]
	add	sp, #4
	bx	lr
.func_end Func_80a1090
