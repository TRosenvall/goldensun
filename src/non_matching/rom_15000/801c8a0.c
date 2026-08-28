/* Func_801c8a0 -- 0x0801c8a0, asm/rom_15000/rom_1aeec_c_a_c_a_b.s
 *
 * Best screen: ours 60 lines against the ROM's 63.  Candidate kept at
 * scratch/L1c8a0_best.c.
 *
 * CORRECTION.  An earlier version of this park claimed the blocker was that
 * "gcc always folds symbol+constant into one pool entry, and nothing stops it".
 * That was wrong, and the control that refuted it is worth repeating: 34 of 531
 * already-MATCHING functions in this repo contain exactly the shape I called
 * unreachable.  The address build now matches exactly.  See
 * docs/elevation.md, "Address arithmetic in unsigned int locals".
 *
 * SOLVED:
 *   - The four-instruction address build (`mov r2,#0x88 / ldr r3,=gState /
 *     lsl r2,#2 / add r3,r2`) comes from doing the arithmetic in unsigned int
 *     locals across separate statements, not in pointer arithmetic:
 *         r2 = 0x88; r3 = (unsigned int)&gState; r2 <<= 2; r3 += r2;
 *         key = *(unsigned short *)r3;
 *     Any single-expression spelling folds to `ldr r3,=gState+544`.
 *   - Both loops are do/while.  A `for` is actively wrong here: gcc rotates it,
 *     rewrites `i <= 0x1bf` into `i < 0x1c0`, and then rebuilds the bound as
 *     `mov r3,#0xe0 / lsl r3,#1` every iteration because 0x1c0 is a cheap
 *     shifted build.  A rebuilt loop bound is therefore NOT always the goto
 *     tell -- check for a `for` that should be a do/while first.
 *   - `key` must be `unsigned int`.  A HImode local is loaded with `ldrsh` plus
 *     a zero index register, and `>> 10` on a signed int gives `asr`.
 *   - Loop 1 is instruction-for-instruction identical to the ROM.
 *
 * REMAINING BLOCKER, and it is a narrow one: in loop 2 the ROM hoists the
 * loop-invariant CONSTANTS (0x3ff into r6, 0x1bf into r5, the address of
 * ewram_2000462 into r12) but does NOT hoist the LOAD of that address --
 * `mov r2,r12 / ldrh r4,[r2]` runs every iteration.  Ours hoists the load.
 *
 * That combination means loop optimisation ran and the load was killed by a
 * store inside the loop -- the only store there is `*out2 = i` on the found
 * path.  gcc-2.96 evidently proves that store cannot alias the global, and I
 * could not make it stop:
 *     - taking the global's address into a local pointer first  -- no change
 *     - declaring the global as an extern ARRAY and reading [0] -- no change
 *     - reading it through a cast integer, `*(unsigned short *)r3` -- no change
 *     - storing through a cast integer, `*(int *)ro = i`         -- no change
 *     - `volatile` on the global (64 lines, adds an instruction)
 *     - making loop 2 a goto loop: this DOES keep the load inside (62 lines,
 *       the closest structural fit) but then nothing is hoisted, so 0x3ff is
 *       reloaded from the pool inside the body where the ROM has `mov r3, r6`.
 *
 * So the shape wanted is "loop optimisation runs, one specific load does not
 * hoist", and the goto lever is too blunt for it -- it is all-or-nothing.  A
 * function needing partial hoisting is not currently reachable, and that is a
 * cleaner statement of the gap than anything about constant folding.
 */
