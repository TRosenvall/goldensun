	.include "macros.inc"

@ Random
@ Takes no arguments. Returns a 16-bit pseudo-random value.
@ A linear congruential generator: seed = seed * 0x41C64E6D + 0x3039, kept at
@ iwram_1cb4, returning bits 8..23 of the new seed (`lsl #8` then `lsr #16`).
@ NOTE rom_77000's Func_79bc4 is a SECOND generator with identical constants but
@ its seed in ewram_23a8. The two streams are independent, and only that one is
@ part of the save state -- this seed lives in IWRAM and does not survive a
@ reset.
.thumb_func_start Func_4458
	ldr	r1, =iwram_1cb4
	ldr	r3, =0x41c64e6d
	ldr	r2, [r1]
	mov	r0, r2
	mul	r0, r3
	ldr	r3, =0x3039
	add	r0, r3
	str	r0, [r1]
	lsl	r0, #8
	lsr	r0, #16
	bx	lr
.func_end Func_4458
