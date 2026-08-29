/* OvlFunc_881_20081c4 -- asm/overlays/rom_77a7c8/ovl_30_a_a_a_c_c.s
 *
 * BLOCKER: two instruction-scheduling pairs -- 12 of 62, same length
 *
 * THE .call_via BLOCK IS EXACT, including the pool load interleaved into the
 * middle of the argument build, which is the part worth reusing:
 *
 *     mov r1, #0x80 / ldr r3, =Func_8000888 / lsl r1, #0xb / mov r12, pc / bx r3
 *
 * Single call site, helper with the symbol BOUND inside (the correct form for
 * one site -- see docs/elevation.md), clobbers "memory" and "r12" only, no
 * "lr" (the sequence never writes lr; the ARM callee returns via r12).
 *
 * GETTING THE PUSH LIST RIGHT was the whole first half of the work and is the
 * generalisable bit.  The first draft used r5, r6 AND r7 where the ROM uses
 * r5, r6 -- because `t = 0x80 << 13;` was written before the call, making it
 * live across it.  The ROM builds that constant AFTER the call and keeps it in
 * r4, which is call-clobbered under -fcall-used-r4 and therefore free.
 * Assigning the call result to its own local and building `t` on the next line
 * took it from 24 differing (first diff at 0, wrong prologue) to 12 (first
 * diff at 24).
 *
 *     r = call_via_r3(__sin(*p << 3), 0x80 << 11);
 *     t = 0x80 << 13;                       <- born AFTER the call, as the ROM does
 *     *(int *)(a + 0xc) = r + t;
 *
 * WHAT IS LEFT, two pairs, both pure scheduling:
 *     rom  str r3,[r6,#0x8] / add r5,#0x64      ours the two swapped
 *     rom  mov r3,#0xa6 / add r0,r4            ours add r0,r4 / mov r3,#0xa6
 * The second is the split-constant-build interleave (the ROM slots an
 * unrelated instruction between `mov r3,#0xa6` and `lsl r3,#0x13`) -- known
 * reachable, 51 of 2987 generated .s files contain that shape, but no spelling
 * found.  MEASURED: swapping the two source statements for the first pair
 * changes nothing; gcc reschedules them back.
 *
 * Best C: scratch/x81c4.c.
 */
