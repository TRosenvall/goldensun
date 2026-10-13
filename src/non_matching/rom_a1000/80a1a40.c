/* Func_80a1a40 -- PlaceCursor -- PARK at 8 differing encodings of 57.
 *   PIN-FREE, DEVICE-FREE, FLAG-FREE.  BODY UNCHANGED THIS BATCH.
 *
 * ===== BATCH 349 BRIEF D: THE FIGURE STANDS, AND THE PARK'S 17-VARIANT TABLE
 * ===== IS FIVE PROGRAMS.  THE BOUND IS UPGRADED FROM EMPIRICAL TO STRUCTURAL.
 *
 * THE FOUR NUMBERS, measured this batch (not inherited):
 *   ENCODINGS           8 differing of 57   (ref 57 / ours 57), first index 1
 *   16-BIT INSTRUCTIONS 48 / 48
 *   BYTES               128 / 128  (--func reports no SIZE line)
 *   POOL / 32-BIT       equal     (the 57-entry stream includes pool+pad)
 *   RELOCATIONS         DIFFER -- the four same symbols, three of them 4 bytes
 *                       earlier.  A CONSEQUENCE of the pool order, not a second
 *                       blocker, exactly as every prior header records.
 *   PINS 0 (tools/shimcount.py).
 * All length dimensions agree, so the figure IS a real distance.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a1a40.c \
 *     asm/rom_a1000/rom_a1814_a_c.s --func Func_80a1a40
 *
 * SPLIT SHAPE, re-verified this batch.  tools/datacheck.py
 * asm/rom_a1000/rom_a1814_a_c.s is SILENT (exit 0).  tools/piece.py: the .s
 * holds TWO functions -- this one at 8 of 57 and Func_80a1ac0 at 67 of 123
 * (src/non_matching/rom_a1000/80a1ac0.c) -- and reports NO non-function
 * exports.  So the split is pure text and THIS FUNCTION CUTS OUT ALONE;
 * nothing here is hostage to the mate at 67.
 *   tools/split_s.py asm/rom_a1000/rom_a1814_a_c.s Func_80a1a40 --dry-run:
 *     would write asm/rom_a1000/rom_a1814_a_c_b.s  (1 function, 69 lines)  <- this
 *     would write asm/rom_a1000/rom_a1814_a_c_c.s  (1 function, 148 lines)
 *     would REMOVE asm/rom_a1000/rom_a1814_a_c.s, would rewrite stage1.ld
 *   Install path on a landing: src/rom_a1000/rom_a1814_a_c_b.c.  Exports: none
 *   needed -- .Laf294 and .Laf29d are already `.global' in
 *   asm/rom_a1000/rom_a1814_c_c_c_c.s, so the __asm__-named externs link as-is.
 *   MANIFEST carries "split": null -- this is a park, not a landing.
 *
 * ===== THE RECORDED NEGATIVE LIST, RE-TESTED WITH OBJECT FINGERPRINTING =====
 *
 * The seventeen variant BODIES from batch 337 brief E are still on disk at
 * scratch_elev/b337/E/w4/, so they were re-swept as written rather than
 * reconstructed from the prose -- the strongest form of this re-test available.
 * tools/sweep_variants.py fingerprints each variant's OBJECT (encodings +
 * relocations), which autosweep and the b337 harness did not:
 *
 *   THE PARK'S TABLE CLAIMS 17 SPELLINGS.  THEY ARE 5 DISTINCT PROGRAMS.
 *   12 of the 17 rows were bit-identical to the base or to another row.
 *
 *   program  figure  dsize  rows that ARE it
 *   -------  ------  -----  -------------------------------------------------
 *   BASE       8       0    a7_struct_andeq  (the park's "plain halfword scalar
 *                           mask / variable mask used twice" row -- it is the
 *                           PARK BODY, bit for bit, and measured NOTHING)
 *   A         51      -4    a1_struct, a2_union, a3_bf16, a4_array,
 *                           c1_cast, c3_vx_only, c4_cast_arr      (SEVEN rows)
 *   B         49      -8    a5_scalar, c2_vy_only                 (two rows)
 *   C         50      -8    a6_struct_late                        (unique)
 *   D         17      -4    b1_hi_var, b2_hi_var_u, b4_two_holders,
 *                           b5_arr_mask, b6_late                  (FIVE rows)
 *   E         51     -20    b3_hi_const  (the CONTROL)             (unique)
 *
 * WHAT THAT CHANGES, and it is a sharpening rather than a refutation:
 *
 * (1) THE TYPE-CONSTRUCTOR DIMENSION IS ONE DATA POINT, NOT FOUR.  Batch 330
 *     introduced `struct / union / `v : 16' / `unsigned short v[1]'' as a
 *     deliberate four-way axis and batch 337 re-ran it in two positions.  Every
 *     one of those rows is the SAME OBJECT as its neighbours (program A within
 *     the constant-mask family, program D within the variable-mask family).
 *     The park's own explanation is why, and it is correct -- promote_mode's
 *     switch (explow.c:897-901) covers INTEGER_TYPE and friends and lets
 *     RECORD_TYPE, UNION_TYPE and ARRAY_TYPE fall out of the `default:' at :911,
 *     so all four constructors dodge it IDENTICALLY.  The consequence nobody
 *     drew: there is nothing left to try on that axis, because there never were
 *     four spellings.
 * (2) THE PARK'S "THREE OF THE CAST ROWS KEEP THE HImode 0x1ff FIX ... AND ARE
 *     STILL ONE INSTRUCTION SHORT" IS ONE PROGRAM DESCRIBED AS THREE ROWS.
 *     c1_cast, c3_vx_only and c4_cast_arr are all bit-identical to a1_struct.
 *     The observation stands; its weight as independent evidence does not.
 * (3) THE PARK'S OWN BASELINE ROW MEASURED NOTHING.  a7_struct_andeq is the row
 *     the park carried forward from batch 322 as "exactly inert at 8".  "Inert"
 *     was the right figure for the wrong reason: it is the base.
 *
 * SO FIVE DISTINCT PROGRAMS, AND THEY ARE EXACTLY THE CORNERS OF THE PARK'S OWN
 * 2x2 -- constant mask (A/B/C/E) against variable mask (D), crossed with where
 * the mask is consumed.  The park's bound is honest: its evidence is thinner
 * than its row count suggests, and it points the same way.
 *
 * ===== THE BOUND, RESTATED STRUCTURALLY.  BOTH GATES READ THE SAME PREDICATE
 *
 * The park closes with "the two requirements have now been shown jointly
 * unsatisfiable BY EVERY SPELLING OF THIS SHAPE".  That is an induction over
 * spellings, and after the count above it is an induction over five programs.
 * It does not need to be: the two gates are the same test on the same operand,
 * and all three lines are on disk.
 *
 *   REQUIREMENT 1 -- A NARROW FIX.  The ROM needs key(0xffff) < key(0x1ff) = 104
 *   with the 0xffff reference pinned at addr 36 by an exact instruction stream,
 *   so its `pool_range' must be under 68, and HImode's 64 is the only one.  Every
 *   other narrow carrier is enumerated and closed in this header (the decisive
 *   one being that `note_invalid_constants', arm.c:5424-5452, pushes a fix only
 *   for an operand that is CONSTANT_P in a memory-ok alternative, and a
 *   const_int can never satisfy `m').  `*thumb_movhi_insn' alternative 1 is
 *   `mn', range 64 (arm.md:4318, :4353) -- THE only narrow carrier a constant
 *   can reach.  To get a value into it, something must emit
 *   `(set (reg:HI) (const_int))'.  At -O2 the producer is expand_binop's
 *   expensive-constant force_reg, optabs.c:640-642, verbatim:
 *
 *       if (CONSTANT_P (op1) && preserve_subexpressions_p ()
 *           && ! shift_op && rtx_cost (op1, binoptab->code) > 2)
 *         op1 = force_reg (mode, op1);
 *
 *   GATED ON `CONSTANT_P (op1)'.
 *
 *   REQUIREMENT 2 -- THE AND MUST SURVIVE fold.  fold-const.c:5728-5731:
 *
 *       case BIT_AND_EXPR:
 *         if (integer_all_onesp (arg1))
 *           return non_lvalue (convert (type, arg0));
 *
 *   GATED ON `integer_all_onesp (arg1)', which requires an INTEGER_CST.  (The
 *   two sibling rules that close the alternative opcodes, re-read this batch:
 *   BIT_IOR_EXPR at :5673-5676 returns the all-ones constant itself, and
 *   BIT_XOR_EXPR at :5702-5706 rewrites to BIT_NOT_EXPR.  On Thumb `and', `orr'
 *   and `eor' are all register-only, so those were the only other patterns that
 *   could have protected a narrow fix, and fold reaches all three.)
 *
 * THE TWO GATES TEST THE SAME THING: whether the mask operand is a compile-time
 * constant.
 *   - Mask IS a constant  -> requirement 1 is satisfiable AND fold deletes the
 *     AND, taking the reference at addr 36 with it.  (Programs A, B, C, E.)
 *   - Mask is NOT a constant -> fold cannot fire, so the AND survives; but
 *     `CONSTANT_P (op1)' is FALSE, so expand_binop never force_regs anything, no
 *     `(set (reg:HI) (const_int))' is ever created, and every fix comes out
 *     SImode at range 1020.  (Program D -- which is precisely what the park
 *     measured: "with the mask a HImode VARIABLE every fix came out SImode, the
 *     minipool went to the end of the function and nothing had to be jumped
 *     over".)
 * THERE IS NO THIRD STATE.  So the park's joint unsatisfiability is not an
 * empirical bound over spellings -- it is a consequence of both gates reading
 * constancy of the same operand, and it cannot be spelled around.
 *
 * SCOPE OF THAT CLOSURE, stated so it is not over-read: it closes THE MASK-
 * SPELLING AXIS, i.e. every route to the narrow fix that goes through a HImode
 * AND whose constant is 0xffff.  It does NOT prove the function unmatchable.
 * What would reopen it is a producer of a HImode `(set (reg:HI) (const_int))'
 * that is NOT expand_binop's force_reg, or a HImode consumer of 0xffff whose
 * pattern has no immediate alternative and whose tree fold does not rewrite.
 * The park has closed store_fixed_bit_field's value mask (expmed.c, `must_and'
 * false at bitpos 0 of a HImode unit), extract_fixed_bit_field's read mask
 * (expmed.c:1621-1637, its mode-change clause inside `#if 0' at :1625-1631), and
 * widen_operand (every CONST_INT has VOIDmode).  Those three plus expand_binop
 * are, as far as four batches have found, the whole set.
 *
 * CORPUS AGREEMENT, carried forward: of 4,418 files under asm/ whose first line
 * is `@ Generated by gcc 2.96', 359 lines are `ldrh rN, .L<pool>' carrying 108
 * distinct pooled values; 65535 IS NOT AMONG THEM.
 *
 * NOTHING WAS RE-SCREENED: the flag axis (all eight per-file groups) and the
 * anchor axis (11 placements, of which the park's own harvest note records 5 as
 * bit-identical, so 6 distinct programs) are recorded closed in this header and
 * were left alone, per the brief.
 *
 * SO THE PARK STANDS AT 8, with a structural bound in place of an empirical one
 * and its evidence base honestly counted.
 * -- scratch_elev/b349/D
 */
struct Win {
    unsigned char pad00[0xc];
    unsigned short fc;
    unsigned short fe;
};

struct Cur {
    unsigned char pad00[6];
    unsigned short x;
    unsigned short y;
    unsigned char pad0a[0xa];
    unsigned short b14 : 8;
    unsigned short : 8;
    unsigned short a16 : 9;
};

struct State {
    unsigned char pad00[0x10];
    struct Win *win;
    struct Cur *cur;
};

extern struct State *iwram_3001f2c;
extern volatile unsigned int iwram_3001e40;
extern unsigned char Laf294[] __asm__(".Laf294");
extern unsigned char Laf29d[] __asm__(".Laf29d");

void Func_80a1a40(int x, int y)
{
    struct State *st;
    struct Cur *cur;
    int vx;
    int vy;

    st = iwram_3001f2c;
    cur = st->cur;
    vx = Laf294[(iwram_3001e40 >> 1) & 7] + x + (st->win->fc << 3) + 8;
    cur->x = vx;
    vx &= 0xffff;
    cur->a16 = vx;
    vy = Laf29d[(iwram_3001e40 >> 1) & 7] + y + (st->win->fe << 3) + 8;
    cur->y = vy;
    vy &= 0xffff;
    cur->b14 = vy;
}
