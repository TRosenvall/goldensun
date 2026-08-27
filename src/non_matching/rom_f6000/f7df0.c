/* Func_80f7df0 -- asm/rom_f6000/rom_f6008_c.s
 *
 * BLOCKER: REGISTER ALLOCATION (four-way permutation, instructions exact)
 *
 * 18 of 30 differing, same length, and every differing line is a register
 * rename -- the instruction sequence is identical operation for operation:
 *
 *     rom  r4 = base   r1 = idx*12   r2 = table value  r5 = idx*12+4
 *     ours r5 = base   r4 = idx*12   r3 = table value  r1 = idx*12+4
 *
 * Getting here took two structural findings that ARE reusable:
 *
 *  1. The ROM addresses everything as `[r4, rOFF]` -- base register plus a
 *     byte offset held in a register -- not as formed pointers.  Writing
 *     `*(char **)(b + no)` with `no` a NAMED LOCAL assigned in its own
 *     statement produces that; writing `b + 0x3404 + idx * 4` inline makes gcc
 *     fold the base in first and emit `ldr r3, [r0]` instead.  That alone went
 *     28 of 30 -> 23.
 *  2. `add r5, r1, #4` is a separate register for `no + 4`, so that offset
 *     needs its own local too.  That went 23 -> 18 and produced the ROM's
 *     extra `push {r5`.
 *
 * MEASURED at 18, all four declaration orders (the batch-115 declaration-order
 * lever is inert here):
 *   b, hp, np, no, n4, to, ho          18
 *   no, n4, to, ho, b, hp, np          18
 *   to, no, ho, n4, b, hp, np          18
 *   b, no, hp, np, n4, to, ho          18
 *   b, to, ho, hp, no, n4, np          18
 *
 * Best C is scratch/m7df0.c.  The structure is a doubly-linked insert-at-head
 * where `prev` is a pointer-to-the-previous-next (the ROM stores the ADDRESS
 * of the head slot into node+4, then stores the node into old_head+4).
 */
