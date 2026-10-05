/* HeightTile_A -- 0x08011e88  (asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s)
 *
 * NON-MATCHING, 2 differing encodings of 36, NO PINS, NO DEVICES  (RE-MEASURED, batch 325 brief C).
 * 36 instructions against 36, 72 bytes against 72, 0 relocation differences,
 * first differing index 26.  The batch-323 figure is exact and reproduces.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011e88.c asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s --func HeightTile_A
 *
 * RECIPE REPOINTED.  The previous recipe named the pre-split
 * asm/rom_9000/rom_11ce0_a_c_c_a_c_c.s and was DEAD -- objcmp exits
 * FileNotFoundError on it.  Batch 324's HeightTile_B landing split that file,
 * and this header's old SPLIT SHAPE prediction is now spent: the split is DONE,
 * the tool named the halves `_a` and `_b` (not `_b` and `_c` as predicted),
 * HeightTile_A's asm is at ..._a_c_c_a_c_c_a.s, and **no further split is
 * needed if this ever lands** -- the install path would be
 * src/rom_9000/rom_11ce0_a_c_c_a_c_c_a.c.
 *   datacheck.py on the reference: CLEAN, no data section, no exports.
 *
 * WHAT THE RESIDUE IS: two encodings, indices 26 and 27, a real instruction
 * pair and not pool words.
 *
 *     rom   mov r0, r3 / mul r0, r2       (0x1c18 then 0x4350)
 *     ours  mov r0, r2 / mul r0, r3       (0x1c10 then 0x4358)
 *
 * THE PATTERN, read out of arm.md rather than inferred:
 *
 *     (define_insn "*thumb_mulsi3"                                 arm.md:1116
 *       [(set (match_operand:SI 0 "register_operand" "=&l,&l,&l")
 *             (mult:SI (match_operand:SI 1 "register_operand" "%l,*h,0")
 *                      (match_operand:SI 2 "register_operand" "l,l,l")))]
 *       which_alternative < 2  ->  "mov %0, %1 ; mul %0, %0, %2"
 *       else                   ->  "mul %0, %0, %2"
 *
 * so **the register the `mov` reads IS md operand 1**, and alternative 2 (md
 * operand 1 constrained `0`, tied to the destination) is the no-`mov` form.
 * Confirmed on this function's own RTL: the ONLY difference between the two
 * spellings' `.17.lreg` is insn 66,
 *
 *     `(t - 8) * a`   (set (reg 54) (mult (reg/v 34) (reg 51)))    34 = a, 51 = t-8
 *     `a * (t - 8)`   (set (reg 54) (mult (reg 51) (reg/v 34)))
 *
 * i.e. C `X * Y` really builds `(mult Y X)` and md operand 1 really is the
 * SECOND C operand.  The ROM needs md operand 1 = `t - 8`, so it needs
 * `a * (t - 8)`.
 *
 * WHY THAT SPELLING STILL COSTS 23 AT BEST -- and this CORRECTS the mechanism
 * the batch-323 header gave for the same number.
 *
 * `.18.greg` for the two bodies differs in exactly two lines:
 *
 *     `(t-8) * a`   ;; 6 regs to allocate: 32 46 54 34 35 33
 *                   dispositions: 33 in 1   34 in 2   35 in 4   <- the ROM's map
 *     `a * (t-8)`   ;; 34 preferences: 1                        <- ADDED
 *                   dispositions: 33 in 4   34 in 1   35 in 2   <- rotated
 *
 * and the consequence shows up as one insn: in the good body insn 6, the
 * incoming copy of `t`, is `NOTE_INSN_DELETED`; in the flipped body it survives
 *
 *     (insn 6 (set (reg/v:SI 4 r4) (reg:SI 1 r1)))      ->   mov r4, r1
 *
 * **That surviving `mov r4, r1` is the observable to screen on, not the
 * figure.**  The ROM's prologue has no such copy.
 *
 * THE CORRECTION.  The old header said the flip "leaves `a` with a
 * hard-register preference it should not have, and `a` -- allocated BEFORE
 * `t` -- takes r1", then argued the fix would need "`a` down to 4 refs or `t`
 * up to 6".  That is an argument about allocation ORDER, and **the order is
 * IDENTICAL in both bodies** (`32 46 54 34 35 33`), as are all ref counts.
 * `a` is allocated before `t` either way.  What the flip actually changes is
 * find_reg's CHOICE, via the added `;; 34 preferences: 1` -- not
 * allocno_compare's ranking.
 *
 * The priority arithmetic is still worth having, and it still closes the door.
 * From `.17.lreg`, with global.c:607's floor_log2(n_refs) * n_refs / live_length:
 *
 *     32 p  8 refs / 12 = 2.000        34 a  7 refs / 17 = 0.824
 *     46    5 refs /  5 = 2.000        35 b  4 refs / 13 = 0.615
 *     54    5 refs /  5 = 2.000        33 t  4 refs / 14 = 0.571
 *
 * reproducing the order exactly.  For `t` to precede `a` it needs SIX refs --
 * floor_log2 makes five worth nothing, 10/14 = 0.714 < 0.824, while six gives
 * 12/14 = 0.857 -- or `a` must fall to FOUR, since five is 10/17 = 0.588 and
 * still ahead.  Every reference in this function is an emitted instruction, so
 * neither is reachable.  That is the stop, reached by a more exact route.
 *
 * OPEN QUESTION, recorded with its evidence rather than as a mechanism:
 * **which insn supplies `;; 34 preferences: 1` is NOT established.**
 * `set_preference` (global.c:1585) is called only from `mark_reg_store`
 * (global.c:1440) as (dest, SET_SRC), strips one `XEXP (src, 0)` for a
 * format-`e` src, and needs one side to be a hard register after
 * `reg_renumber`.  On insn 66 that strip yields reg 51, not reg 34, so that
 * call cannot put a preference on 34.  The figure and the `mov r4, r1`
 * observable stand without it.
 *
 * LEVER 1 (reuse the existing local `a` for the third sample) IS GENUINELY
 * RIGHT, and now for a stated reason rather than a measurement.  With a fresh
 * local `c` the third sample is live only in the tail block, so it is a LOCAL
 * quantity, never reaches greg, and local-alloc hands it **r3** -- while the
 * ROM wants the third sample in **r2**.  `.18.greg` for both fresh-local
 * bodies shows `36 in 3` beside `34 in 2`, 34 now being the block-0 first
 * sample at only 3 refs.  Reusing `a` makes the third sample a global allocno
 * and greg gives it r2.  **The fresh-local bodies cannot get below 4 however
 * the multiply is spelled, because they lose the register the ROM needs for
 * the thing the multiply reads.**
 *
 * A SECOND TRAP, found here: A MATCHING `mov`/`mul` PAIR IS NOT EVIDENCE THE
 * MULTIPLY IS SPELLED RIGHT.  The fresh-local UNFLIPPED body reads 4 of 36 and
 * its aligned diff does NOT include the `mov`/`mul` pair -- the pair agrees
 * with the ROM's bytes, for the wrong reason.  There `c` is in r3 and `t - 8`
 * in r2, exactly transposed against the ROM, so the same two encodings mean md
 * operand 1 = `c` in our program and md operand 1 = `t - 8` in the ROM's.  An
 * operand-order error and a register transposition cancelling inside the
 * encoding.  HeightTile_4 hit the same shape from the other side (its
 * `t = a - b` emits the ROM's `sub` exactly and is a wrong program).
 * **Check the register map before believing a matching multiply.**
 *
 * MEASURED THIS ROUND -- a FULL 32-BODY CROSS, the product of
 *   {arm-2 multiply flipped / not}
 *   x {index inline, named after the third load, named after `a -= b`, hoisted
 *      into block 0}
 *   x {`a -= b` / `a = a - b`}
 *   x {arm-1 multiply flipped / not}
 * all 36 instructions against 36 unless noted:
 *
 *    2  this body; `a = a - b`; `u` named after `a -= b`; and both crossed
 *    4  arm-1 multiply flipped (in all four of its combinations)
 *    4  `u` named after the third load
 *    6  `u` after the third load + arm-1 flipped
 *   23  FLIP + `u` hoisted into block 0 -- and index 0 itself differs,
 *       ROM b500 against ours b520: **the rotation makes us PUSH r5.**
 *   24  `u` hoisted into block 0, unflipped
 *   31  FLIP + `u` named after the third load
 *   32  FLIP, with every other combination
 *
 * plus, hand-built outside the cross:
 *   fresh local `c` for the third sample, unflipped        4 of 36
 *   fresh local `c` for the third sample, FLIPPED          6 of 36
 *   `c = c - b` instead of `c -= b`, flipped               6 of 36
 *   `t -= 8` writeback + `a * t`                          24 of 36
 *   `t -= 8` before the third load + `a * t`              24 of 36
 *   `t -= 8` after `a -= b` + `t * a`            14 of 36, ours 34 insns (SHORT)
 *   `p[1]`/`p[2]` indexing instead of `p++`      33 of 36, ours 34 insns (SHORT)
 *   `(a * (t-8)) / 8 + b` instead of `b + ...`            29 of 36
 *
 * NOTHING BEATS 2, and the flip never goes below 23.  The batch-323 bound
 * therefore SURVIVES on a wider edit list than the one that produced it --
 * notably including the one edit its own 8-edit crossfire had omitted, because
 * lever 1 had settled it.
 *
 * WHY THE FAMILY'S WRITEBACK LEVER DOES NOT APPLY HERE.  HeightTile_4 and
 * HeightTile_6 both landed in batch 325 on `a = t + 0xf` -- writing the biased
 * index back into the parameter to buy it one extra reference and with it its
 * own argument register.  `_A` has no bias statement to write back: its low arm
 * uses the raw index and its high arm's `t - 8` writeback (`t -= 8`) measures
 * 24, because it deletes the ROM's `mov r3, r1`.  The lever needs a second
 * statement on the index, and `_A` has only one.
 *
 * PINS ARE STRICTLY WORSE (batch 271, not re-tested):
 * `register int a __asm__("r2")` is 28 at 40 lines, both pins also 28.
 */
int HeightTile_A(signed char *p, int t)
{
    int a;
    int b;

    a = *p << 19;
    p++;
    b = *p << 19;
    p++;
    if ((unsigned int)t <= 7)
        return a + ((b - a) * t) / 8;
    a = *p << 19;
    a -= b;
    return b + ((t - 8) * a) / 8;
}
