/* Func_8077f70  --  0x08077f70, asm/rom_77000/rom_77320_a_c_c.s
 *
 * ===== BATCH 325 G.  STILL A PARK, NOW AT 3 differing encodings of 120. =====
 *
 * NON-MATCHING, 3 differing encodings of 120; ref 120 against ours 120, SIZE
 * EQUAL, RELOCATIONS IDENTICAL (objcmp prints no RELOCATIONS line).  So the 3
 * IS a distance.  PINS: 0.  SHIMS: 0.  DEVICES: 0.  FLAGS: none.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_77000/8077f70.c asm/rom_77000/rom_77320_a_c_c.s --func Func_8077f70
 *
 * SPLIT SHAPE, if it ever lands -- unchanged from the previous header and
 * re-checked: asm/rom_77000/rom_77320_a_c_c.s holds THREE functions
 * (Func_8077f70 first, then Func_807808c, Func_8078144) and
 * tools/datacheck.py prints nothing (no data section).
 *   python3 tools/split_s.py asm/rom_77000/rom_77320_a_c_c.s Func_8077f70
 * Installed path would be src/rom_77000/rom_77320_a_c_c_b.c.
 *
 * ===========================================================================
 * THE PARK WAS ENROLLED AS `tu-pool`.  THAT ENROLLMENT IS WRONG, AND CAUSE 1 --
 * SIX OF THE OLD NINE -- IS NOW CLOSED.  9 -> 3.
 *
 * The previous header's decomposition was right: CAUSE 1 was the literal pool's
 * internal order (3 pool words plus the 3 `ldr` offsets that follow from them,
 * six of the nine) and CAUSE 2 is one halfword store sunk by sched2 (three).
 * Its reading of cause 1 was right too, down to the inequality: the 0x1ff fix
 * had to be a NARROW-mode operand.  Two things it got wrong, and both were
 * load-bearing.
 *
 * (1) ITS ARITHMETIC SAID "R <= 40, SO QImode, SO ONE OF THE TWO PREMISES IS
 * WRONG."  NEITHER PREMISE IS WRONG; THE INFERENCE IS.  It derived R <= 40 from
 * the ROM keeping 0x10 out of this pool, via `add_minipool_forward_ref`'s
 * early return
 *     fix->address >= minipool_vector_head->max_address - fix->fix_size
 * That is the wrong exclusion path.  0x10 is excluded by the BARRIER DROP-BACK
 * in `arm_reorg` (arm.c:5533-5551): the accumulation loop records every barrier
 * whose address is below the head's max_address as `last_barrier` and keeps
 * going, and when it finally breaks, every fix AFTER last_barrier has
 *     fdel->minipool->refcount--;  fdel->minipool = NULL;
 * so it is added to the list and then DROPPED, and `assign_minipool_offsets`
 * gives it no size.  That is what happens in OUR build too, which the park's
 * own `.26.mach` shows and it did not read: two separate "Emitting minipool"
 * lines, the pool at 204 holding three words and 0x10 alone at 242.  The
 * exclusion of 0x10 is consistent with ANY value of R, so it constrains
 * nothing, and the "QImode contradiction" dissolves.
 *
 * (2) AND R = 32 IS UNREACHABLE FOR A `const_int` IN PRINCIPLE, so the park's
 * conclusion ("propagate only `narrow`") was weaker than the facts allow.
 * `fix->forwards = get_attr_pool_range (insn)` (arm.c:5371) is the attribute of
 * the REFERENCING INSN.  The Thumb patterns carrying a small pool_range are
 *     *thumb_movhi_insn        64   alt 1 constraint `mn`   (arm.md:4318,4353)
 *     *thumb_zero_extendhisi2  60   `memory_operand` only   (arm.md:3069)
 *     *thumb_zero_extendqisi2  32   `memory_operand` only   (arm.md:3155)
 *     *thumb_extendqisi2       32   `memory_operand` only   (arm.md:3609)
 *     *thumb_movqi_insn        32   alt 1 constraint `m`    (arm.md:4638)
 * -- and only `*thumb_movhi_insn`'s has `n` in it.  A `const_int` can therefore
 * reach exactly ONE narrow pool_range on Thumb, 64, and
 *     > THE REQUIREMENT WAS NOT "NARROW".  IT WAS "HImode", EXACTLY.
 *
 * ===========================================================================
 * AND NOTHING ABOUT THIS POOL IS TU-SCOPED, WHICH RETIRES THE CLASS FOR THIS
 * FUNCTION.  `push_minipool_fix`'s `address` is `insn_addresses` within the
 * current function; `arm_reorg` walks only this function's insn chain and
 * resets `minipool_fix_head` per function; both pools are mid-function.  No
 * input to `add_minipool_forward_ref` comes from outside the function, so a
 * standalone TU has every lever the original TU had.  THE `tu-pool` VERDICT
 * COULD NOT HAVE BEEN RIGHT HERE, whatever the residue turned out to be.
 *
 * ===========================================================================
 * HOW THE HImode FIX WAS FOUND, AND WHY NINE SPELLINGS HAD MISSED IT.
 *
 * The park's nine spellings all varied the MASK's declaration, and the
 * `movhi` expander says why they could not work: its TARGET_THUMB arm
 * (arm.md:4269) calls `force_reg (HImode, operands[1])` only
 * `if (GET_CODE (operands[0]) != REG)`, and ARM's PROMOTE_MODE (arm.h:597)
 * widens every narrow LOCAL to SImode.  So no declaration of the mask can put a
 * `const_int` into HImode.
 *
 * The route in is the C FRONT END's shortening of a bitwise operator to its
 * RESULT type.  Found empirically first, by scanning every generated `.s` in
 * the tree for `ldrh rX, .L<n>` that does NOT feed a `strh` -- 165 hits, and
 * `asm/overlays/rom_7ec19c/ovl_30_c_a_c_a_a.s:19` is `ldrh r2,.L9 / and r3,r3,r2`,
 * our exact shape, from `unsigned short d; d = (a->f6 + 0x2000) & ~0x3fff;`.
 * With a u16 RESULT and a LITERAL mask, `.00.rtl` already holds
 *
 *     (set (reg:HI 67) (mem:HI (plus (reg 36) (reg 32)) 6))
 *     (set (reg:HI 68) (const_int 511))      <- *thumb_movhi_insn, range 64
 *     (set (reg:SI 70) (subreg:SI (reg:HI 68) 0))
 *     (set (reg:SI 69) (and:SI (subreg:SI (reg:HI 67) 0) (reg:SI 70)))
 *
 * so the HImode const set is FRONT-END output, and after reload the mixed modes
 * collapse to the ROM's `ldr r0,<pool> ... and r3,r0`.  Both halves are needed:
 * with the mask still an `int` variable the fix stays SImode (52 of 120).
 *
 * THEN IT HAS TO SIT IN THE INNER LOOP'S PREHEADER, as the ROM's does
 * (`ldr r0,.L78018 / mov r1,#0 / mov r2,#0xd8 / b .L78028`).  With the park's
 * `goto` inner loop it does not -- it stays in the loop body and the fix's
 * address lands past the barrier, so it goes into the SECOND pool and the
 * figure is 54.  `.08.loop` says exactly why:
 *
 *     Loop from 45 to 344: 85 real insns.
 *     Insn 296: regno 68 (life 1), move-insn savings 1 not desirable
 *
 * -- loop.c:2184, the LICM desirability test needing
 * `threshold * savings * m->lifetime >= insn_count`, and `insn_count` is 85
 * because **THE ONLY LOOP IN THE DUMP IS THE OUTER ONE**: a label plus a
 * backward `goto` emits no front-end loop notes, so loop.c never sees the inner
 * loop and has no inner preheader to hoist into.  Writing the inner loop as a
 * real `for` gives it one, and the hoist happens for free at the source level.
 *
 * `.26.mach` now prints the ROM's pool verbatim:
 *     ;; HImode fixup for i451; addr 194, range (0,64): 0x1ff
 *     ;; Emitting minipool after insn 325; address 206
 *     ;;  Offset 0, max  258 0x1ff      <- the predicted 194 + 64
 *     ;;  Offset 4, max 1032 0x901
 *     ;;  Offset 8, max 1050 0x11b
 *     ;; Emitting minipool after insn 492; address 244
 *     ;;  Offset 0, max  296 0x10
 *
 * MEASURED, and NEITHER HALF WORKS ALONE (all at 120/120 unless noted):
 *     the park's body                                      9
 *     u16 result, mask still an `int` variable            52   (fix stays SImode)
 *     u16 result, literal mask, `goto` inner loop         54   (+4 bytes)
 *     u16 mask AND u16 result, `goto` inner loop          62
 *     `for` inner loop, literal mask, `int` result        26
 *     `for` inner loop, literal mask, u16 result           5
 *     ... and `r2 += 2` before `r1++` in the step clause    3
 *     (`r1 < 0xf` for `r1 <= 0xe` is exactly inert at 3)
 * The last two encodings of the five were the increment order alone.
 *
 * ===========================================================================
 * CAUSE 2 IS THE WHOLE REMAINING 3, AND IT IS THE PARK'S, UNCHANGED.
 * Indices 23/24/25.  ROM `strh r3,[r5,#0x3a] / lsl r1,#16 / asr r1,#16`; ours
 * puts the store after the pair.  The previous header's sched2 numbers hold in
 * the new body: in block `b 1 bb 0` the store is insn 72 at priority 34 with
 * three dependents and the shift is insn 75 at priority 36 with two, both ready
 * at the same cycle.  Its two mechanisms also hold: the sibling store 68 keeps
 * its place because it carries an ANTI-dependence on 75 (75 overwrites the r1
 * that 68 reads) and `arm_adjust_cost` returns 0 for REG_DEP_ANTI, so 68
 * inherits 75's 36 exactly; our store sources r3, which nothing in the shift
 * chain writes, so its only path to the block end is the memory edge to the
 * call, 1 + 33 = 34.
 *
 * ONE THING ADDED, AND IT CLOSES THE "WE WIN AT A TIE" HOPE.
 * `rank_for_schedule` (haifa-sched.c) opens with
 *     priority_val = INSN_PRIORITY (tmp2) - INSN_PRIORITY (tmp);
 *     if (priority_val) return priority_val;
 * and the class rung and the dependent-count rung are BOTH below it.  So the
 * store's 3-dependents-to-2 advantage can never be reached while it is two
 * points behind, and the price is exactly those two points: the store needs two
 * more hops of dependence below it, or the shift chain two fewer.  The ROM's
 * own instruction sequence fixes both lengths -- `lsl #16 / asr #16 / lsl #14 /
 * bl` is three edges above the call, so prio(75) = prio(87) + 3 is not
 * negotiable, and the store's only non-call dependent is the post-call
 * `mov r3,#0x80` at priority 3.
 *
 * MEASURED FOR CAUSE 2 ON THE NEW BODY, all 120/120:
 *     the 0x3a store written first in the source          3  INERT
 *     the 0x36 read before the 0x34 read                  3  INERT
 *     the dividend taken from the raw load, which
 *       shortens the shift chain by one edge              3  INERT
 *     `r1 = (short)r1;` for the two explicit shifts       4  WORSE
 * Plus everything the previous header measured inert for cause 2, none of which
 * was refuted: both stores volatile, either one volatile, the two store
 * statements swapped, the 0x36 load volatile -- with its finding that a
 * `volatile` MEM adds no scheduling dependence the alias set does not already
 * give (both stores print `(mem:HI (plus ...) 6)`, alias set 6).
 *
 * EVERYTHING ELSE IN THE PREVIOUS HEADER IS KEPT AND NONE OF IT WAS REFUTED:
 * the `[offset]` A/B result at ldrsh sites, the loop.c second-pass threshold
 * bracket of 15..17, the goto amendment, "a pooled small constant whose
 * consumer is a halfword store is blocker 1b, not a symbol", the
 * declaration-lever warning about testing under --no-sched2, the two-shift sign
 * extension beating a `(short)` cast, and the corpus-neighbour transplant out
 * of src/rom_77000/rom_77320_c_b.c.  The body below is the park's with two
 * edits: the inner item loop is a `for`, and its test reads a `unsigned short`
 * temporary.
 *
 * -- scratch_elev/b325/G
 */
extern void ClearFlag(int id);
extern void SetFlag(int id);
extern void Func_8079ae8(int unit);
extern void CalcStats(int unit);
extern void *GetUnit(int unit);
extern void EquipItem(int unit);
extern unsigned char gState[];

void Func_8077f70(void)
{
    void *r5;
    unsigned char *g;
    int r0;
    int r1;
    int r2;
    int r3;
    int i;
    unsigned short t;

    ClearFlag(0x20);
    ClearFlag(0x21);
    SetFlag(0x901);
    Func_8079ae8(5);
    CalcStats(5);
    ClearFlag(0x11b);
    SetFlag(0x11a);

    for (i = 0; i < 2; i++) {
        r5 = GetUnit(i);
        r1 = *(unsigned short *)((char *)r5 + 0x34);
        r3 = *(unsigned short *)((char *)r5 + 0x36);
        *(unsigned short *)((char *)r5 + 0x38) = r1;
        *(unsigned short *)((char *)r5 + 0x3a) = r3;
        r1 <<= 16;
        r1 >>= 16;
        r0 = r1 << 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x14) = r3;
        if ((r3 << 16) != 0) {
            goto label_0x3a;
        }
        r3 = *(short *)((char *)r5 + 0x38);
        if (r3 == 0) {
            goto label_0x3a;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x14) = r3;
    label_0x3a:
        r0 = *(short *)((char *)r5 + 0x3a);
        r1 = *(short *)((char *)r5 + 0x36);
        r0 <<= 14;
        r0 /= r1;
        r3 = 0x80;
        r3 <<= 7;
        if (r0 > r3) {
            r3 = 0x80 << 7;
        } else {
            if (r0 < 0) {
                r3 = 0;
            } else {
                r3 = r0;
            }
        }
        *(short *)((char *)r5 + 0x16) = r3;
        if ((r3 << 16) != 0) {
            goto label_items;
        }
        r3 = *(short *)((char *)r5 + 0x3a);
        if (r3 == 0) {
            goto label_items;
        }
        r3 = 1;
        *(short *)((char *)r5 + 0x16) = r3;
    label_items:
        for (r1 = 0, r2 = 0xd8; r1 <= 0xe; r2 += 2, r1++) {
            t = *(unsigned short *)(r2 + (int)r5) & 0x1ff;
            if (t == 0xf) {
                *(unsigned short *)(r2 + (int)r5) = 0x10;
                EquipItem(i);
                break;
            }
        }
        Func_8079ae8(i);
        CalcStats(i);
    }

    GiveInnateMove(0, 0x8c);
    GiveInnateMove(0, 0x95);
    GiveInnateMove(1, 0x8c);
    GiveInnateMove(2, 0x8d);
    g = gState;
    *(int *)(g + 0x10) += 0x96 << 1;
}
