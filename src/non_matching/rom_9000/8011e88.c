/* HeightTile_A -- 0x08011e88  (asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s)
 *
 * NON-MATCHING, 2 differing encodings of 36, NO PINS, NO DEVICES
 *   (RE-MEASURED batch 327 brief I; reproduces exactly).
 *   --func: 2 of 36 (ref 36, ours 36), first differing index 26.
 *   --whole: "2 of 36 differ (ours 36), first at index 26" -- agrees.
 *   0 relocation differences.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011e88.c asm/rom_9000/rom_11ce0_a_c_c_a_c_c_a.s --func HeightTile_A
 *
 *   SPLIT SHAPE: the split is DONE (batch 324's HeightTile_B landing did it);
 *   this function's asm is already alone in ..._a_c_c_a_c_c_a.s, datacheck.py
 *   on the reference is CLEAN (no data section, no exports), and the install
 *   path if it ever lands is src/rom_9000/rom_11ce0_a_c_c_a_c_c_a.c with NO
 *   further split_s.py run.
 *
 * *** ARITHMETIC CORRECTION TO THE PREVIOUS HEADER ***
 * It said "36 instructions against 36, 72 bytes against 72".  The byte count is
 * right and the instruction count is NOT: counted off both streams, the ROM and
 * ours each hold **35 real instructions** = 70 bytes, plus ONE 2-byte alignment
 * pad = 72 bytes = 36 encodings.  `of 36` in the figure is encodings.
 *
 * WHAT THE RESIDUE IS: two encodings, indices 26 and 27, a real instruction
 * pair and not pool words.
 *
 *     rom   mov r0, r3 / mul r0, r2       (0x1c18 then 0x4350)
 *     ours  mov r0, r2 / mul r0, r3       (0x1c10 then 0x4358)
 *
 * `*thumb_mulsi3` (arm.md:1116) ties the dest to md operand 1 and C `X * Y`
 * expands to `(mult Y X)`, so md operand 1 is the SECOND C operand and it is
 * the register the `mov` reads.  The ROM's high arm puts the sample difference
 * in r2 and `t - 8` in r3, and its `mov r0, r3` makes md operand 1 = `t - 8`.
 * So the ROM needs `a * (t - 8)`, the FLIP of what ships.
 *
 * *** THE FLIP IS NOW EXCLUDED ON LENGTH, WHICH IS A STRONGER BOUND THAN THE
 *     PREVIOUS HEADER'S "32 of 36". ***
 * That 32 was MISALIGNMENT, NOT A DISTANCE.  objcmp's repaired pad guard prints,
 * on every flipped body:
 *     INSTRUCTION COUNT ref 35, ours 36 (excluding 1/0 pad word(s))
 *     NOTE: size and encoding count MATCH -- a pad is absorbing the difference
 * The flipped body is **36 real instructions against the ROM's 35**.  The extra
 * instruction is exactly the resurrected incoming copy of `t`:
 *     (insn 6 (set (reg/v:SI 4 r4) (reg:SI 1 r1)))   ->   mov r4, r1
 * which is `NOTE_INSN_DELETED` in the body that ships.  The whole register map
 * rotates with it (t->r4, a->r1, b->r2), which is why 32 of 36 positions differ.
 * This is a FIFTH sighting of the padding trap on a figure already in a header.
 *
 * *** THE PREVIOUS HEADER'S OPEN QUESTION IS ANSWERED: THE CARRIER IS
 *     `expand_preferences`, NOT `set_preference`. ***
 * It recorded "which insn supplies `;; 34 preferences: 1` is NOT established",
 * having ruled out `set_preference` (global.c:1585) because its `XEXP (src, 0)`
 * strip on the multiply yields reg 51, not reg 34.  That ruling is correct and
 * the answer is one function up.  Measured and read, in four steps:
 *
 *  1. `.18.greg` for the two spellings differs in EXACTLY ONE LINE,
 *     `;; 34 preferences: 1`, present only in the flip.  Every conflict set,
 *     every other allocno's preferences, and the allocation order
 *     `32 46 54 34 35 33` are bit-identical -- so the previous header's
 *     retraction of the allocation-ORDER argument was right.
 *  2. `.17.lreg` insn 66's REG_DEAD NOTE ORDER FOLLOWS THE MULT'S RTL OPERAND
 *     ORDER exactly:
 *       base `(mult 34 51)` -> (REG_DEAD 34) then (REG_DEAD 51)
 *       flip `(mult 51 34)` -> (REG_DEAD 51) then (REG_DEAD 34)
 *  3. `expand_preferences` (global.c:828-869) walks `REG_NOTES` IN ORDER and
 *     IORs `hard_reg_preferences` / `hard_reg_full_preferences` SYMMETRICALLY
 *     between SET_DEST's allocno and each REG_DEAD allocno, CUMULATIVELY.  With
 *     notes [51, 34]: allocno 54 absorbs 51's r1 preference first, then 34 is
 *     merged with the now-r1-bearing 54 and INHERITS r1.  With notes [34, 51]:
 *     34 is merged while 54 is still empty and inherits nothing.  The chain is
 *     t (r1) -> `t - 8` -> the product -> `a`, and the mult's operand order is
 *     the only thing deciding whether the last link transmits.
 *  4. The consequence runs through `prune_preferences` (global.c:941), which
 *     does `AND_COMPL_HARD_REG_SET (temp, allocno[num].hard_reg_full_preferences)`
 *     BEFORE storing `regs_someone_prefers` -- so a register the allocno ITSELF
 *     prefers is REMOVED from the set `find_reg`'s pass 0 avoids.  In the base
 *     body r1 stays in 34's `regs_someone_prefers` (because 33 prefers it) and
 *     pass 0 skips it, giving 34 r2; in the flip it is subtracted out, and
 *     `find_reg`'s trailing preference loop (global.c:1097-1110) then overrides
 *     best_reg with r1.
 *  Pass order inside `global_alloc` confirmed (global.c:496-547):
 *  global_conflicts -> mirror_conflicts -> expand_preferences ->
 *  qsort(allocno_compare) -> prune_preferences -> dump_conflicts, so the dump IS
 *  after both preference passes and the one-line diff is a real input diff.
 *
 * WHY UNFLIPPED CAN NEVER MATCH EITHER -- stating the previous header's
 * "cancelling errors" trap as an impossibility.  Matching the `mov`/`mul` pair
 * with the unflipped spelling requires the sample difference in r3 (it becomes
 * md operand 1), while the FOUR EARLIER encodings of the high arm
 * (`lsl r2,r3,#19` / `mov r3,r1` / `sub r2,r4` / `sub r3,#8`) pin it to r2.
 * The two requirements are contradictory, which is exactly why the fresh-local
 * body reads 4 with a coincidentally-correct multiply.  **The flip is NECESSARY
 * and costs +1 instruction.  That is the whole bound.**
 *
 * NEW-AXIS SWEEP, batch 327 -- four axes the previous 32-body cross did not
 * touch (it crossed statement PLACEMENT x flip; these change TYPE, SIGNATURE,
 * WRITEBACK TARGET and DECLARATION ORDER).  All EXACTLY INERT against the plain
 * flip, reproducing 32 differing encodings AND the same first difference
 * `index 4: ref 04da ours 1c0c`, all at 36 instructions:
 *     `unsigned int t` parameter with `if (t <= 7)` and explicit casts ... 32
 *     product written back into `a`  (`a = a * (t - 8); return b + a / 8;`) 32
 *     product written back into the PARAMETER `t = a * (t - 8)` .......... 32
 *     declaration order swapped to `int b; int a;` ...................... 32
 *   and, outside the flip axis:
 *     `b = (a - b) * (t - 8) / 8 + b;` .... 6 of 36 but 36 instructions (LENGTH)
 * => the rotation is decided solely by the mult's RTL operand order; nothing
 *    above it in the source reaches the decision.
 *
 * THE FAMILY: `_A` IS THE LAST UNLANDED MEMBER OF SIXTEEN AND IT DOES NOT CLOSE.
 * The landed twin `HeightTile_B` (src/rom_9000/rom_11ce0_a_c_c_a_c_c_b.c) is the
 * SAME BODY with the flip -- and with the signature `(unsigned char *p,
 * int unused, int t)`, which puts `t` in **r2**.  `_A`'s ROM `cmp r1, #7` puts
 * `t` in **r1**, and r1 being the incoming argument register is precisely what
 * makes the inherited preference cost an instruction.  So the family's
 * multiply-order lever is real and `_A` is the member where it cannot be paid
 * for.  The family's OTHER lever -- writing the biased index back into the
 * parameter, which landed `_4` and `_6` -- also cannot apply: it needs a second
 * statement on the index and `_A` has only one (`t -= 8` measures 24, deleting
 * the ROM's `mov r3, r1`).
 *
 * PRIORITY ARITHMETIC, carried forward unchanged (it is still the reason the
 * rotation cannot be out-ranked).  From `.17.lreg`, global.c:607's
 * floor_log2(n_refs) * n_refs / live_length:
 *     32 p  8 refs / 12 = 2.000      34 a  7 refs / 17 = 0.824
 *     46    5 refs /  5 = 2.000      35 b  4 refs / 13 = 0.615
 *     54    5 refs /  5 = 2.000      33 t  4 refs / 14 = 0.571
 * For `t` to precede `a` it needs SIX refs (floor_log2 makes five worth
 * nothing: 10/14 = 0.714 < 0.824, six gives 12/14 = 0.857) or `a` must fall to
 * FOUR.  Every reference here is an emitted instruction, so neither is reachable.
 *
 * LEVER 1 (reuse the existing local `a` for the third sample) stands, with the
 * previous header's reason: a fresh local lives only in the tail block, is
 * therefore a LOCAL quantity, never reaches greg, and local-alloc hands it r3 --
 * while the ROM wants the third sample in r2.  Reusing `a` makes it a global
 * allocno and greg gives it r2.
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
