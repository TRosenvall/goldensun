/* Func_80a40ac (DiscardFirstUnlockedItem) -- NON-MATCHING, 14 of 47 encodings.
 *
 *   STREAM LENGTHS AGREE (ref 47, ours 47), INSTRUCTION COUNT AGREES (44 both),
 *   SIZE 100 both sides, and THE RELOCATIONS ARE CLEAN -- both objects carry
 *   exactly the two call relocations and nothing else.  So this figure IS a
 *   distance.  Pin-free and device-free: no symbol-table entry is referenced.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80a40ac.c \
 *     asm/rom_a1000/rom_a1814_c_c_a_a.s --func Func_80a40ac
 *
 * Walks the inventory from slot 0, skipping any slot whose bit 9 is set, and
 * stops at the first empty one.  For the first unlocked slot it finds it calls
 * _Func_80788c4 (quantity + 1) times -- the whole stack -- and then stops.
 * Returns 1 when it ran off the end of the list without finding anything to
 * drop, or when that final consume answered 2; 0 otherwise.  The epilogue is
 * `pop {r5,r6,r7} / pop {r1} / bx r1`, and a single-register pop into r1
 * encodes a one-to-four-byte declared return, so the int return is confirmed
 * by the bytes.
 *
 * WHAT THE RESIDUE IS.  Two things, both allocation and layout, nothing
 * expression-level:
 *
 *  - THE TWO SCAN VARIABLES HOLD EACH OTHER'S REGISTERS.  The reference keeps
 *    the tested word in r3 and the body's own load in r2; ours has them the
 *    other way round, and with that goes the body's load order -- the
 *    reference has `ldrh v,[r0] / ldr mask,pool / and`, ours has
 *    `ldrh mask,pool / and / ldrh v,[r0]`.  Eight encodings.  The reference's
 *    AND reads the BODY load, which is live afterwards, and reaching that
 *    needs a copy-free AND and a same-block load at once -- and those two are
 *    mutually exclusive here, for the reason set out under THE THIRD RESIDUE.
 *
 *  - THE DONE BLOCK'S BRANCH POLARITY, four encodings.  The reference is
 *    `mov r0,#0 / cmp r1,#2 / bne epilogue / b one`; ours is
 *    `cmp r1,#2 / beq one / mov r0,#0 / b epilogue`.  Same four instructions,
 *    inverted test.  Seven spellings were measured and every one inverts.
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
