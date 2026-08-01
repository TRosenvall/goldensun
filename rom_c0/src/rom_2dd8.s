	.include "macros.inc"

@ FreeTagged -- rewinds an arena, does not free a block
@ r0 = tag. Reads [iwram_1e50 + tag*4]; a null or low pointer does nothing.
@ Otherwise it clears the tag slot and STORES THE POINTER BACK OVER AN ARENA
@ BUMP POINTER, choosing which by `(ptr >> 22) & 4`:
@     an EWRAM address (0x2xxxxxx) gives 8 & 4 = 0 -> [iwram_1e50 + 0]
@     an IWRAM address (0x3xxxxxx) gives 0xC & 4 = 4 -> [iwram_1e50 + 4]
@ So the arena is rewound to where this block started, and EVERYTHING ALLOCATED
@ AFTER IT IS SILENTLY RECLAIMED.
@ FREEING OUT OF ORDER CORRUPTS THE ARENAS. Every caller in the ROM frees in
@ reverse allocation order -- rom_c9000's handlers release tag 0x2F then 0x2E,
@ the reverse of how they were generated. Preserve that ordering exactly when
@ decompiling; it is load-bearing, not stylistic.
.thumb_func_start Func_2dd8
	ldr	r4, =iwram_1e50
	lsl	r0, #2
	ldr	r1, [r0, r4]
	lsr	r3, r1, #22
	beq	.L2dec
	mov	r2, #0
	str	r2, [r0, r4]
	mov	r0, #4
	and	r3, r0
	str	r1, [r3, r4]
.L2dec:
	bx	lr
	.ssize	Func_2dd8

@ FreeScratch
@ r0 = pointer. Rewinds whichever arena the pointer belongs to back to it, the
@ same way Func_2dd8 does but taking the address directly rather than a tag.
@ Used to release the anonymous blocks Func_4938 and Func_4970 hand out, and
@ carries the same reverse-order requirement.
.thumb_func_start Func_2df0
	ldr	r4, =iwram_1e50
	mov	r1, #4
	lsr	r2, r0, #22
	and	r2, r1
	str	r0, [r2, r4]
	bx	lr
.func_end Func_2df0
