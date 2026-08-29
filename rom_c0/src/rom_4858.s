	.include "macros.inc"
	.include "gba.inc"

@ ResetAllocator
@ Takes no arguments. DMA3-clears the whole 64-word tag table at iwram_1e50,
@ then seeds the two arena bump pointers:
@     [iwram_1e50 + 4] = iwram_2000   (0x3002000) IWRAM arena base
@     [iwram_1e50 + 0] = ewram_30000  (0x2030000) EWRAM arena base
@ Every tagged allocation is dropped, so this is a whole-program reset rather
@ than anything a subsystem should call.
.thumb_func_start Func_4858
	sub	sp, #4
	ldr	r4, =iwram_1e50
	mov	r0, sp
	mov	r3, #0
	str	r3, [r0]
	mov	r1, r4
	ldr	r3, =REG_DMA3SAD
	ldr	r2, =0x85000040
	stmia	r3!, {r0, r1, r2}
	sub	r3, #0xc
	ldr	r3, =iwram_2000
	str	r3, [r4, #4]
	ldr	r3, =ewram_30000
	add	sp, #4
	str	r3, [r4]
	bx	lr
.func_end Func_4858
