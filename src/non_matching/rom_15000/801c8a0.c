/* Func_801c8a0 -- 0x0801c8a0, asm/rom_15000/rom_1aeec_c_a_c_a_b.s
 *
 * BLOCKER: unfoldable symbol+offset.  The ROM loads a symbol base and adds a
 * runtime-BUILT constant offset to it:
 *
 *      mov  r2, #0x88
 *      ldr  r3, =0x2000240          ; gState
 *      lsl  r2, #0x2                ; 0x88 << 2 = 0x220
 *      add  r3, r2
 *      ldrh r3, [r3, #0x0]          ; reads 0x2000460
 *
 * gcc-2.96 folds a symbol plus a constant offset into a single pool entry
 * unconditionally -- we get `ldr r3, =gState+544 / ldrh r3, [r3, #0]`, two
 * instructions where the ROM has four.  Nothing in the source appears able to
 * stop it.
 *
 * Everything else in this function matches.  Best screen: ours 57 lines against
 * the ROM's 63; the six-line deficit is this address build (3) plus the
 * register-allocation shift it causes downstream (out2 lands in r12 instead of
 * the ROM's r14, and `a`/`i` swap r1 and r2).  Both loop bodies are
 * instruction-for-instruction identical to the ROM.
 *
 * SOLVED, and reusable:
 *   - BOTH loops are do/while, not `for`.  Written as `for (i = 0; i <= 0x1bf;
 *     i++)` gcc rotates the loop, jumps INTO the middle of the body to reach
 *     the test, and -- because it converted `i <= 0x1bf` to `i < 0x1c0` -- then
 *     rebuilds the bound as `mov r3,#0xe0 / lsl r3,#1` on every iteration.
 *     The ROM instead hoists `ldr r5,=0x1bf` once and tests `ble` at the
 *     bottom, which is exactly what a do/while gives.  Worth remembering: a
 *     rebuilt loop bound is NOT always the goto-loop tell -- here it was a
 *     `for` that should have been a do/while, and the `<=K` to `<K+1` rewrite
 *     is what made rebuilding cheap enough for gcc to prefer it.
 *   - The two loops differ in ONE deliberate way: loop 1 reads its global once
 *     before the loop, loop 2 re-reads `ewram_2000462` inside the body every
 *     iteration.  That asymmetry is in the ROM (`mov r2,r12 / ldrh r4,[r2]`
 *     inside loop 2) and it reproduces from writing the load in that position
 *     in the source.  It is not an optimiser artefact.
 *   - `key` must be `unsigned int`, not `unsigned short`: a HImode local is
 *     loaded with `ldrsh` plus a zero index register (see the note on
 *     Func_80a3ddc), and `>> 10` on a signed int gives `asr`, not the ROM's
 *     `lsr`.
 *
 * TRIED AND DID NOT MOVE THE ADDRESS BUILD: `((unsigned short *)gState)[0x110]`;
 * `*(unsigned short *)((int)gState + (0x88 << 2))`; writing the offset as
 * `0x88 * 4`; -O1; -fno-rerun-cse-after-loop (69 lines, worse).
 */
