/* Func_80a1a40 -- PlaceCursor -- 0x080a1a40, asm/rom_a1000/rom_a1814_a_c.s
 *
 * PARK, 8 differing encodings of 57.  RE-MEASURED batch 325, brief F (was
 * measured 8 of 57 in batch 322 and the figure reproduces exactly).  PINS: 0.
 * ref 128 bytes / 57 encodings / 53 instruction rows; ours 128 bytes / 57
 * encodings / 53 instruction rows; per-opcode memory profile IDENTICAL
 * (push 1, ldr 12, ldrb 2, ldrh 3, strh 3, strb 1, pop 2).  Relocations differ:
 * the same four symbols, three of them 4 bytes earlier -- a CONSEQUENCE of the
 * pool order, not a second blocker.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_a1000/80a1a40.c asm/rom_a1000/rom_a1814_a_c.s --func Func_80a1a40
 *
 * SPLIT SHAPE: a pure text split, two ways.  tools/datacheck.py
 * asm/rom_a1000/rom_a1814_a_c.s is silent (exit 0).  tools/split_s.py
 * asm/rom_a1000/rom_a1814_a_c.s Func_80a1a40 --dry-run:
 *     would write asm/rom_a1000/rom_a1814_a_c_b.s  (1 function, 69 lines)  <- this
 *     would write asm/rom_a1000/rom_a1814_a_c_c.s  (1 function, 148 lines)
 *     would REMOVE asm/rom_a1000/rom_a1814_a_c.s, would rewrite stage1.ld
 * Install path on a landing: src/rom_a1000/rom_a1814_a_c_b.c.  Exports: none --
 * .Laf294 and .Laf29d are already `.global` in
 * asm/rom_a1000/rom_a1814_c_c_c_c.s, so the __asm__-named externs link as-is.
 *
 * THE INSTRUCTION STREAM IS EXACT: both streams hold the same 53 instructions
 * and the function assembles to 0x80 bytes, the ROM's exact size, WITH the
 * literal pool before the epilogue and the `b` that jumps over it reproduced.
 * THE RESIDUE IS POOL WORD ORDER AND NOTHING ELSE: 0xffff sorts first in the ROM
 * and fifth here, which moves three pool words and five `ldr` pc-offsets.
 *
 * ============== THE MECHANISM, WITH THE DUMP THAT SHOWS IT ==============
 * `push_minipool_fix` (arm.c:5380) prints every pool fix into `<base>.c.26.mach`
 * in ascending `addr + range` (arm.c:4820).  For this body, verbatim:
 *
 *   SImode i14  addr  0 range (0,1020) `iwram_3001f2c'  -> 1020
 *   SImode i169 addr  4 range (0,1020) `iwram_3001e40'  -> 1024
 *   SImode i22  addr 18 range (0,1020) `*.Laf294'       -> 1038
 *   SImode i66  addr 36 range (0,1020) 0xffff           -> 1056
 *   HImode i71  addr 40 range (0,  64) 0x1ff            ->  104
 *   SImode i82  addr 54 range (0,1020) 0xfffffe00       -> 1074
 *   SImode i95  addr 66 range (0,1020) `*.Laf29d'       -> 1086
 *
 * The ROM needs key(0xffff) < key(0x1ff) = 104 with the 0xffff reference at
 * addr 36, so its `pool_range` must be under 68.  **HImode's 64 is the only
 * narrow range a CONSTANT can reach on Thumb**, and that is now read off every
 * candidate rather than assumed: `*thumb_movqi_insn` alternative 1 is constraint
 * `m` with NO `n` (arm.md:4634, :4638, range 32), `*thumb_zero_extendqisi2`
 * takes a `memory_operand` (arm.md:3155, range 32), and the ranges 60 and 32,32
 * belong to `*arm_zero_extendhisi2` and `extendsfdf2` -- ARM-only and float.
 * `*thumb_movhi_insn` alternative 1 is `mn`, range 64 (arm.md:4318, :4353),
 * which is why it is the one that accepts a `const_int`.
 *
 * ===== CORRECTION, batch 325: THE OLD HEADER NAMED THE WRONG PRODUCER =====
 * The previous header offered `widen_operand`'s
 * `gen_lowpart (SImode, force_reg (HImode, op))` as "the general recipe for a
 * narrow pool fix".  **widen_operand can never produce one.**  Its first branch
 * is `! no_extend || GET_MODE (op) == VOIDmode || SUBREG_PROMOTED_VAR_P`, and
 * EVERY CONST_INT has VOIDmode, so a constant operand always returns through
 * `convert_modes` and stays a constant.  This function's own `.02.jump` is the
 * counterexample: insn 71 is already `(set (reg:HI 60) (const_int 511))` BEFORE
 * the `(and:SI (subreg:SI (reg:HI 60) 0) (reg:SI 63))` at insn 76.
 *
 * THE REAL PRODUCER is `expand_binop`'s expensive-constant force_reg:
 *     optabs.c:636  if (CONSTANT_P (op0) && preserve_subexpressions_p () ...
 *     optabs.c:640  if (CONSTANT_P (op1) && preserve_subexpressions_p ()
 *                       && ! shift_op && rtx_cost (op1, binoptab->code) > 2)
 *                     op1 = force_reg (mode, op1);
 * with `mode` the binop's own mode.  `preserve_subexpressions_p` (stmt.c)
 * returns 1 OUTRIGHT when `flag_expensive_optimizations`, so it is
 * unconditionally true at -O2 and the loop/insn-count heuristic under it never
 * runs.  So the general recipe is:
 *
 *   ANY HImode binop -- and, ior, xor, plus, minus, mult; NOT a shift, which
 *   `shift_op` excludes -- with a constant operand of `rtx_cost > 2` emits
 *   `(set (reg:HI) (const_int))` and therefore a HImode pool fix at range 64.
 *
 * The `& 0x1ff` this body relies on is one instance of that, not of widen_operand.
 *
 * ============ WHY 0xffff SPECIFICALLY IS STILL UNREACHABLE ============
 * The old verdict stands, re-read in this gcc, at both levels:
 *
 *   TREE: `fold`'s `case BIT_AND_EXPR:` opens with
 *   `if (integer_all_onesp (arg1)) return non_lvalue (convert (type, arg0));`
 *   -- unconditional, no side-effect guard, and fold canonicalises the constant
 *   to arg1.  A HImode AND means a 16-bit result type, for which 0xffff IS all
 *   ones.  The same rule kills `| 0xffff` (returns the constant) and `^ 0xffff`
 *   (rewritten to BIT_NOT_EXPR).  C's integer promotions make every 16-bit
 *   arithmetic expression an `int` expression unless `convert_to_integer`
 *   SHORTENS it -- and the shortening path builds the narrow tree and calls
 *   `fold` on it, which applies the same rule.  MEASURED: the
 *   `unsigned short t; t = vx & 0xffff;` shape compiles to 44 instruction rows
 *   against the reference's 53 -- nine instructions GONE, which is the fold rule
 *   firing.  (scratch_elev/b325/F/work/b4.c)
 *
 *   RTL: `store_fixed_bit_field` (expmed.c) emits exactly ONE value mask,
 *   `mask_rtx (mode, 0, bitsize, 0)`, under
 *   `must_and = (GET_MODE_BITSIZE (GET_MODE (value)) != bitsize
 *                && bitpos + bitsize != GET_MODE_BITSIZE (mode))`.
 *   0xffff needs bitsize 16, and in a HImode unit bitsize 16 forces bitpos 0, so
 *   the second conjunct is false and no mask is emitted at all.  The ROM has TWO
 *   masks on the value (0xffff then 0x1ff), so the 0xffff is a SOURCE-level mask.
 *   The only other HImode binop that could carry 0xffff is add/sub/mult, and the
 *   ROM's instructions are `and r2, r5` and `and r3, r5` -- the opcode names the
 *   optab.
 *
 * REACHABILITY of the narrow fix in general (denominator printed, batch 322): of
 * 4,418 files under asm/ whose first line is `@ Generated by gcc 2.96`, 359 lines
 * are `ldrh rN, .L<pool>` carrying 108 distinct pooled values (0 x96, 511 x18,
 * -16384 x15, 1 x15, 31 x12, 1023 x10, 255 x8, ...).  **65535 is not among the
 * 108**, which is the corpus agreeing with the two bounds above.
 *
 * ========= THE INSTRUMENT THAT CLOSES THE ARITHMETIC (batch 325) =========
 * scratch_elev/b325/F/work/b3.c replaces both masks with
 * `unsigned short tx; tx = vx & 0xfffe;` -- the same shape with a mask that is
 * NOT all-ones, so fold leaves it and convert_to_integer shortens it to a HImode
 * AND.  `.26.mach` then reads a **HImode fixup at addr 36, range (0,64),
 * 0xfffffffe -> 100**, ahead of 0x1ff, with the five symbols after it: THE ROM'S
 * POOL ORDER EXACTLY.  As a figure it is worthless and is recorded only as a
 * figure ABOUT THE BLOCKER -- 47 of 57 at 132 bytes against 128 and 55
 * instruction rows against 53, because the HImode temp costs two instructions and
 * 0xfffe is the wrong value.  What it proves is that NOTHING BUT THE MODE OF THE
 * 0xffff LOAD stands between this body and a match.
 *
 * ============ WHAT WAS WON, and it is most of the function ============
 * Three levers took this from 47 to 0 on the instruction stream:
 *
 *   - THE BITFIELD IS WHAT PLACES THE POOL.  Writing the attribute merge by hand
 *     as `(attr & 0xfffffe00) | (v & 0x1ff)` lets convert_to_integer shorten the
 *     mask to 0xfe00.  An `int` local for the merge defeats the shortening and
 *     restores `.word 0xfffffe00`, but leaves both masks SImode, so the pool goes
 *     to the END of the function.  Declaring the field `unsigned short a16 : 9`
 *     makes store_bit_field emit the mask at HImode -- range 64 -- which drags
 *     the minipool UP and makes gcc manufacture the `b` over it.
 *   - AN EAGERLY-LOADED POINTER IS FIXED BY GIVING A LATER POINTER AN EARLIER
 *     LIVE RANGE.  Assigning `cur = st->cur;` immediately after `st` -- fourteen
 *     instructions before the ROM loads it -- makes the two conflict, forces st
 *     to r5, makes reload pick r6 rather than r5 as the low temp for the constant
 *     7, and that anti-dependence stops sched2 hoisting `ldr r6, [r5, #0x10]`.
 *     ONE STATEMENT MOVE: 12 to 2.
 *   - Splitting `a |= vx & 0x1ff;` into `vx &= 0x1ff;` as its own statement was
 *     worth 20 to 12 -- statement splitting, here on a mask chain.
 *
 * iwram_3001e40 must be `volatile`: the ROM keeps its address in r14, reloads
 * the value, and recomputes the `>> 1 & 7`.
 *
 * TRIED AND LOST, so nobody repeats them: a field read-back with a hand-written
 * merge (47); unsigned short locals, where PROMOTE_MODE gives lsl/lsr rather than
 * an and (41); int locals with an explicit &= 0xffff (25); win and cur as named
 * locals in six placements (20-30); the constant-as-destination spelling (20);
 * multiply-by-8 rather than a shift by 3 (20); compound accumulation (20); u16
 * locals or casts on the bitfield store (29-31); -fno-schedule-insns2, WORSE at
 * 31 -- sched2 is helping here; and a `unsigned short m; m = 0xffff;` variable
 * used TWICE, which puts the right value in the pool but combine promotes the
 * standalone `(set (reg:HI m) (const_int 65535))` to SImode, so the fix is SImode
 * at range 1020 and the order is unchanged -- exactly inert at 8
 * (scratch_elev/b322/H/work/a1.c).
 *
 * SO THE PARK STANDS.  It is a compiler question, not a spelling search: a HImode
 * `and` whose constant operand is 0xffff.  Both producers are closed with a file
 * and a line each, the narrow-range alternatives are enumerated and closed, and
 * the pool arithmetic is confirmed by an instrument that reproduces the ROM order.
 *
 * A NOTE FOR neighbour.py: it returned an 8-way tie on iwram_3001e40, all in a
 * different directory with different conventions.  Applying the N-way-tie rule to
 * iwram_3001f2c instead gave ~40 same-directory siblings, one of which already
 * declared the exact struct this function needs.  SUGGESTED REFINEMENT: rank a
 * shared global by how FEW files use it, and prefer same-directory hits.
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
