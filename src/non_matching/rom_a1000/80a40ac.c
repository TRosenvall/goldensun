/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING, 14 of 47 encodings.
 *
 *   RE-MEASURED BATCH 338, brief A, and the figure reproduces EXACTLY.  Both
 *   numbers, because they are not the same number:
 *     STREAM ENCODINGS  ref 47, ours 47   -- 14 differ
 *     INSTRUCTION COUNT ref 44, ours 44   (one 32-bit pool word and two `bl`)
 *     SIZE 100 both sides; relocations clean (the two calls, nothing else).
 *   So this figure IS a distance.  Pin-free and device-free (`shimcount`: no rows).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a40ac.c \
 *     asm/rom_a1000/rom_a1814_c_c_a_a.s --func Func_80a40ac
 *
 * AUTOSWEPT (tools/autosweep.py, batch 338) -- RECORDED NEGATIVES, DO NOT RE-RUN.
 *   flag axis:   -O1 37, SCHED2_CFLAGS 18, FIXEDR7_CFLAGS 51 (worse than the
 *                reference length), and GCSE / ALIAS / STRENGTH / CSE /
 *                RERUNLOOP all inert at 14.
 *   anchor axis: 29 top-level boundaries, 25 measured, ALL inert at 14.
 * Both cheap axes are closed.
 *
 * ============================================================
 * BATCH 338: THE RESIDUE IS NOW ONE MECHANISM, NOT THREE, AND IT IS CLOSED
 * WITH LINE NUMBERS.  The batch-335 header below is correct in every
 * observation and its three-part residue description is right; what it did not
 * have is the index-level decomposition and the ONE question all of it reduces
 * to.  Index-aligned side-by-side (helper kept at
 * scratch_elev/b338/A/a_sbs.py, which imports objcmp rather than forking it):
 *
 *     idx  ref                         ours
 *      3 X movs r3,#0xd8               movs r2,#0xd8
 *      4 X ldrh r3,[r0,r3]             ldrh r2,[r0,r2]
 *      9 X ldrh r2,[r0]                ldr  r3,[pc,#40]
 *     10 X ldr  r3,[pc,#40]            ands r3,r2
 *     11 X ands r3,r2                  cmp  r3,#0
 *     12 X cmp  r3,#0                  bne  .next
 *     13 X bne  .next                  ldrh r3,[r0]
 *     14 X lsrs r3,r2,#11              lsrs r3,r3,#11
 *     31 X movs r0,#0                  cmp  r1,#2
 *     32 X cmp  r1,#2                  beq  .one
 *     33 X bne  .pop                   movs r0,#0
 *     34 X e006 b .one                 e008 b .pop
 *     39 X ldrh r3,[r0]                ldrh r2,[r0]
 *     40 X cmp  r3,#0                  cmp  r2,#0
 *
 * Everything else, including the pool word at index 30 and the whole inner
 * do-while, is byte-identical.  Note `ands r3,r2` (0x4013) is ALREADY present
 * and copy-free in our body -- the AND is not the problem.
 *
 * THE WHOLE OF INDICES 3,4,9-14,39,40 COLLAPSES TO ONE QUESTION, measured:
 * read the halfword DIRECTLY in the body block (`*p & 0x200`, no cached `t`,
 * no cast) and indices 0 THROUGH 9 GO EXACT -- the ROM`s `ldrh r2,[r0]` at the
 * top of the body block, in the ROM`s register, with the ROM`s copy-free
 * `ands r3,r2` at index 12 and the ROM`s `lsrs r3,r2,#11`.  The ONLY thing
 * wrong in that body is how 0x200 is materialised:
 *
 *     ref   ldr  r3,[pc,#40]  +  .word 0x200       (1 insn + 1 pool word)
 *     ours  movs r3,#128 / lsls r3,r3,#2           (2 insns, NO pool)
 *
 * so it reads 38 of 47 at 46 encodings -- ONE INSTRUCTION LONG and ONE POOL
 * WORD SHORT.  Candidate kept at scratch_elev/b338/A/v40ac/d_plain.c.
 *
 * AND THAT ONE QUESTION IS CLOSED BY THE COMPILER, four citations:
 *
 *  1. IN SImode 0x200 CAN NEVER POOL.  `thumb_shiftable_const`
 *     (arm.c:8579-8593) returns 1 whenever `(val & (0xff << i)) == val` for
 *     some i in 0..24; 0x200 is 0x80 << 2, so it qualifies.  arm.c:2082 and the
 *     `K` constraint (arm.h:1098) then take the two-insn mov/shift path.  There
 *     is no spelling of an SImode 0x200 that reaches the minipool.
 *  2. IN HImode 0x200 MUST POOL.  `*thumb_movhi_insn` (arm.md:4318-4358) offers
 *     exactly one non-register source for a constant: alternative 1, `mn`, with
 *     `pool_range 64` and length 4, emitting `ldrh %0, %1`.  Its only immediate
 *     alternative is 5, constraint `I` (0..255), and 0x200 does not fit.  There
 *     is NO shiftable-const alternative in HImode.  (gas assembles gcc`s
 *     `ldrh rd, .Lpool` into the ROM`s `ldr rd,[pc,#n]` over a `.word`, which is
 *     why the two sides agree byte-for-byte at index 10/9 and at index 30 --
 *     checked, not assumed.)
 *     >> So the batch-335 header`s claim that the HImode cast is what buys the
 *        pool is CORRECT.  Its stated mechanism -- pool_range 64 forcing an
 *        early dump -- is the wrong half: pool_range is not what decides
 *        POOLING-versus-SPLITTING, the absence of a shiftable alternative is.
 *  3. SO THE AND MUST BE HImode, WHICH NEEDS A TRUNCATION, WHICH NEEDS THE
 *     HALFWORD IN AN int FIRST.  `*p` is already `unsigned short`, so
 *     `(unsigned short)*p` is a no-op and the AND stays SImode (measured: 38,
 *     identical to the cast-free body).
 *  4. AND A SAME-BLOCK TRUNCATION COSTS A COPY.  `v = *p;` then
 *     `(unsigned short)v & 0x200` in the same block: combine`s LOG_LINK
 *     substitutes the single-use cast chain into the AND, canonicalisation
 *     (combine.c:3492-3505) swaps because the substituted operand`s class is not
 *     `o`, and regmove cannot swap back (regmove.c:1153 needs a bare REG in
 *     operand 1; regmove.c:1199-1205 needs operands_match_p, which a SUBREG
 *     fails), so fixup_match_1 inserts the copy.  MEASURED +1 INSTRUCTION:
 *     41 of 47 at dsize +4, in both the `unsigned int` and the `int` spelling.
 *
 * THE MUTUAL EXCLUSION, STATED AS A BOUND WITH ITS EVIDENCE ATTACHED:
 *   HImode AND  => pools (good) but needs a same-block truncation => +1 copy.
 *   SImode AND  => right registers, right load placement, copy-free (indices
 *                  0-9 exact) but 0x200 splits into mov/lsl => -1 pool word,
 *                  +1 instruction.
 *   Cross-block truncation (the body below) => pools AND copy-free, but the AND
 *                  then reads the PREVIOUS iteration`s halfword, so the
 *                  body-block load falls after the branch and r2/r3 swap.
 *   Those are the three corners and every spelling measured lands on one of
 *   them.  The batch-335 header`s sentence "reaching that needs a copy-free AND
 *   and a same-block load at once -- and those two are mutually exclusive here"
 *   is CONFIRMED, and the reason is now a compiler citation rather than an
 *   observation.
 *
 * MEASURED AND WORSE IN BATCH 338 -- sixteen more bodies, do not re-run.  All
 * are differing encodings of the reference`s 47; dsize is ours minus ref.
 *   mask the body load, four spellings of `v = *p` + (u16) cast ..... 41, +4
 *   mask the body load, `(unsigned short)w` with `int w = *p` ........ 41, +4
 *   `*p & 0x200` read directly, and `0x200 & *p` ..................... 38, -4
 *   `(unsigned short)*p & 0x200` (the cast is a no-op) ............... 38, -4
 *   `(*p & 0xffff) & 0x200` .......................................... 38, -4
 *   `v = *p;` then `unsigned short w = v;` masked (cast named) ....... 32
 *   mask `*p`, shift the cached `t` .................................. 40, -4
 *   mask the cached `t`, shift `*p` directly ......................... 14 INERT
 *   mask the cached `t`, with `v = *p` live before the test .......... 18
 *   mask and shift both from the cached `t` .......................... 17
 *
 * WHAT IS LEFT, and it is NOT an expression-level question.  Indices 31-34, the
 * done block`s branch polarity, are a separate and smaller residue (4 of the
 * 14) and seven spellings of it were already measured inert in batch 335.  The
 * reference`s `movs r0,#0` sits in the SAME basic block as `cmp r1,#2`, ours
 * sits in the branch target, so no scheduling lever can reach it -- the
 * question is which block gcc puts the constant in, and that is settled before
 * sched2.  If this park is reopened, reopen it on index 9: a body that pools an
 * SImode 0x200 would land the function, and `thumb_shiftable_const` says no
 * body does.
 *
 * ============================================================
 * REWRITTEN IN BATCH 335.  Everything above the line is new.  The two
 * superseded figures are spelled in WORDS so no tool can harvest them as live
 * claims: the park used to claim THIRTY-SIX of forty-seven, with the stream
 * lengths disagreeing (forty-four against forty-two) AND the relocations
 * differing IN THEIR SYMBOLS, so that figure was never a distance and the body
 * could not have passed a whole-ROM comparison at all.
 *
 * WHAT THE PARK HAD RIGHT.  Its three levers all survive and are all still in
 * the body: name the offset for the register-offset read, an int for the loaded
 * value so gcc emits the zero-extending halfword load, and clobber the offset
 * with the loaded value so the load lands in the register that held the offset.
 *
 * WHAT THE PARK HAD WRONG, AND IT WAS THE WHOLE BLOCKER.  It read the residue
 * as "which of r2 and r3 holds the offset, which is the register-pressure
 * category and which no source form has ever selected", and it reached the
 * pooled mask through the symbol-table entry const.sym names for this
 * function.  THAT ENTRY IS NOT NEEDED, and the body that used it could never
 * match: a symbol address pools in SImode, whose pool_range is 1020
 * (*thumb_movsi_insn, arm.md:3838-3858), so the word lands in the END pool and
 * the reference's MID-FUNCTION pool and its branch over it never appear.
 *
 * THE TWO MISSING INSTRUCTIONS, BOTH LOCATED:
 *
 *  1. THE MID-FUNCTION POOL AND THE BRANCH OVER IT.  *thumb_movhi_insn
 *     (arm.md:4318-4320) constrains its source `mn` at ALTERNATIVE 1 with
 *     pool_range 64 (arm.md:4353), and recog takes the first match, so an
 *     HImode constant always pools and pools EARLY -- which is what forces
 *     arm_reorg to dump the word right after the loop with the reference's
 *     branch across it, at zero extra instructions.  A (u16) cast on the
 *     WIDER-typed value makes the mask an HImode entry.  This is the identical
 *     one-character lever that landed Func_80a2144 --
 *     src/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_c_c_c_c.c -- whose own header
 *     ends by saying the symbol it replaced now has no beneficiary.  Read that
 *     file before touching this one again.
 *
 *     const.sym's entry for this mask says in terms: "The masked value is
 *     widened to int by integer promotion before the AND, so there is no
 *     HImode expression for gcc to pool into."  THAT SENTENCE IS REFUTED.  Its
 *     eight probed spellings are all either a declared-narrow variable or a
 *     cast on the CONSTANT; none of them is a cast on a wider VALUE, which is
 *     the only one that narrows the AND.  The entry now has no beneficiary
 *     here either, and retiring it is an owner call with the evidence complete.
 *
 *  2. THE LOOP COUNTER IS A COPY OF THE QUANTITY.  The reference materialises
 *     the count, tests it, and then copies it into the counter register.  A
 *     separate counter variable assigned from the count before the do-while
 *     gives exactly that copy; decrementing the count itself does not.
 *
 * THE THIRD RESIDUE, WHICH THE LENGTH HID.  With both of the above the body is
 * ONE INSTRUCTION LONG, because the (u16) cast also buys a register copy in
 * front of the AND.  Mechanism, read and not recalled:
 *
 *   - the initial RTL is `(and:SI (subreg:SI (reg:HI mask)) (reg:SI cast))`,
 *     mask FIRST, which is the reference's two-address AND writing the mask's
 *     own register;
 *   - combine substitutes the single-use cast chain into the AND and then
 *     CANONICALISES at combine.c:3492-3505 ("put a constant last and a complex
 *     expression first"): operand 0 is a SUBREG whose inner is an object and
 *     operand 1 is then also a SUBREG, whose class is not 'o', so the two are
 *     swapped and the plain register ends up in operand 1;
 *   - regmove cannot swap them back.  regmove.c:1153 skips the insn unless
 *     operand 1 is a bare REG, and regmove.c:1199-1205 takes the commutative
 *     alternative only when operands_match_p holds between the OTHER operand
 *     and the destination -- a SUBREG never matches -- so fixup_match_1 inserts
 *     the copy from operand 1.
 *
 * THE ESCAPE, and it is the reusable part: THE CAST'S VALUE MUST BE DEFINED IN
 * A DIFFERENT BASIC BLOCK FROM THE AND, so combine's log links cannot reach the
 * cast chain and the mask stays in operand 1.  The scan variable tested at the
 * loop head holds bit-for-bit the same halfword as the body's own load, so
 * masking THAT one instead costs nothing and keeps both the pool and the
 * copy-free AND.  That is what the body below does, and it is what takes the
 * instruction count to exact.
 *
 * MEASURED AND INERT OR WORSE -- do not re-run.  All figures are differing
 * encodings of the reference's forty-seven; instruction counts given where they
 * are not forty-four.  A crossed grid of mask spelling by shift spelling by
 * loop counter by one-or-two scan variables by mask-test-before-or-after-the
 * shift, eighty bodies, then three focused rounds:
 *   (u16)v & 0x200, 0x200 & (u16)v, (u16)v & (u16)0x200, (u16)(v & 0x200)
 *     -- all four IDENTICAL to each other at every other setting
 *   v & (u16)0x200 ................... LOSES THE POOL, size 96, 29 to 40
 *   an unsigned short mask variable, in either operand order, and the same
 *     widened back to unsigned ....... LOSES THE POOL, 38
 *   (u16) on the halfword MEM itself .. LOSES THE POOL (expand takes the SImode
 *     constant path), 38 -- right AND order, wrong constant mode
 *   an unsigned short copy of the value, masked ... LOSES THE POOL, 32
 *   naming the cast and using it twice ... the zero-extension becomes REAL, a
 *     shift pair, pool lost, 31
 *   (unsigned)v >> 11 with an int value == an unsigned int value, everywhere
 *   the shift computed BEFORE the mask test ... the AND loses its copy, but the
 *     HImode window then reaches past the done block and the POOL-SKIP BRANCH
 *     disappears: forty-three instructions, 29 to 41
 *   decrementing the count in the loop instead of a separate counter ... 23 at
 *     best, forty-four instructions, with the AND copy and the missing counter
 *     copy cancelling in the length -- the trap this park fell into twice
 *   seven done-block spellings (test reversed, a named zero, a reused dead int,
 *     a subtraction, a multiply by the result, a separate return label) ... 14,
 *     except the separate label which is forty-five instructions and 27
 *   five declaration orders of the locals ... 14, every one
 *   a one-member union round the halfword, to give its accesses alias set 0 and
 *     raise its sched2 priority, crossed with all three body orders ... 15, 18,
 *     19 -- WORSE in every crossing
 *
 * NEXT, if reopened.  The register swap and the done-block polarity are both
 * downstream of choices made after reload.  The one dimension not varied is
 * whether the two scan loads can be ONE variable whose live range is split by
 * the allocator, which is what the reference's register pair looks like from
 * the outside; every one-variable body measured here is worse, but none of them
 * tried to lengthen the body load's live range past the mask test.
 */
extern char *_GetUnit(int id);
extern int _Func_80788c4(int a, int b);

int Func_80a40ac(int who)
{
    char *u;
    unsigned short *p;
    unsigned int v;
    unsigned int t;
    int i, r, q, n, k;

    u = _GetUnit(who);
    t = 0xd8;
    t = *(unsigned short *)(u + t);
    r = 0;
    i = 0;
    p = (unsigned short *)(u + 0xd8);
    goto test;
body:
    if (((unsigned short)t & 0x200) != 0)
        goto next;
    v = *p;
    q = v >> 11;
    n = q + 1;
    if (q == 0)
        n = 1;
    if (n == 0)
        goto done;
    k = n;
    do {
        r = _Func_80788c4(who, i);
        k--;
    } while (k != 0);
done:
    if (r != 2)
        return 0;
    goto one;
next:
    i++;
    p++;
    if (i > 0xe)
        goto ret;
    t = *p;
test:
    if (t != 0)
        goto body;
one:
    r = 1;
ret:
    return r;
}
