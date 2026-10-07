/* Func_80ae9f0 -- asm/rom_a1000/rom_ae88c_c_c_c.s
 *
 * NON-MATCHING, 13 of 43 encodings  (RE-MEASURED, batch 333 brief B).
 *   COUNTS NOW AGREE: ref 43, ours 43, instructions thirty-nine against
 *   thirty-nine, SIZE ninety-two bytes both sides.  So this figure IS a
 *   distance and the earlier twenty-six was a MISALIGNMENT figure -- the
 *   stream was one instruction long and every later position differed.
 *   RELOCATIONS now agree in symbols and offsets.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_a1000/80ae9f0.c \
 *     asm/rom_a1000/rom_ae88c_c_c_c.s --func Func_80ae9f0
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: REGISTER ROTATION on the parameters.  (The figure once written
 * here, twenty-three of forty-four and "one line long", is superseded: see
 * the batch-333 section below.  The one line was not the rotation.)
 *
 * Picks a halfword from one of two offsets in the block at iwram_3001f2c
 * depending on the fourth argument, adjusts the third, calls _Func_801eadc
 * with five arguments (the fifth on the stack), and on success clears two
 * fields and sets a flag. Both arms, both offsets, the stack argument, the
 * failure return of -1 and the success path all reproduce.
 *
 * The residue is which register holds which parameter:
 *
 *     rom    d->r5  a->r4  c stays in r2 and is modified IN PLACE (`sub r2,#3`)
 *     ours   c->r4  a->r5  c copied out of r2 first
 *
 * and it propagates: the ROM's base advance is `add r3, r1` (in place,
 * clobbering the base) where ours is the three-operand `add r3, r2, r1`,
 * because our named `base` stays live across both arms.
 *
 * MEASURED:
 *   base addressed as `base + 0x392` in each arm       45 lines, 23 differ
 *   base ADVANCED in place, `base += 0x392;`           45 lines, 29 differ
 *
 * The second is the informative negative. The three-operand add looked like the
 * tell -- the ROM clobbers its base, so writing an in-place advance should
 * match -- and it makes the function SIX WORSE. Advancing the pointer keeps it
 * live longer in our version, not shorter, because the load then depends on the
 * advanced value rather than on a base-plus-offset the scheduler can fold.
 *
 * So the in-place add is a CONSEQUENCE of the ROM's register assignment, not a
 * cause of it -- the same relationship recorded on Func_8079664, where forcing
 * the ROM's runtime gState offset made that function six worse for the same
 * reason. Two instances now: when an addressing-mode difference sits downstream
 * of a register rotation, spelling the addressing mode does not fix the
 * rotation and usually costs lines.
 *
 * ========================================================================
 * BATCH 333 BRIEF B -- THE ONE EXTRA INSTRUCTION WAS NOT THE ROTATION.
 * IT WAS A POOLED HImode ZERO.  Removing it halves the figure.
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park was OURS LONGER.  So the question was
 * never what statement to add; it was what shape lets gcc stop keeping a
 * value it is keeping.
 *
 * The park above read "BLOCKER: REGISTER ROTATION on the parameters ... one
 * line long".  The rotation is real and is still the whole remaining residue,
 * but it is NOT the extra line.  Side by side the success arm read:
 *
 *     rom    mov r3, #0 / strb r3, [r0, #4] / strh r3, [r0, #0xc]
 *     ours   mov r3, #0 / strb r3, [r0, #4] / LDRH r3, <pool> / strh r3, ...
 *
 * gcc built a SECOND, HImode zero for the halfword store and PUT IT IN THE
 * LITERAL POOL, instead of reusing the SImode zero already sitting in r3 for
 * the byte store.  That is one extra instruction and one extra pool word --
 * which is also the whole of the size and relocation difference.
 *
 * WHY AN HImode CONSTANT ALWAYS REACHES THE POOL IN THUMB.  `*thumb_movhi_insn`
 * (arm.md:4318-4320) constrains operand 1 as "l,mn,l,*h,*r,I".  Alternative 1
 * is `mn`, and `n` matches ANY const_int; the immediate alternative `I` is
 * alternative 5.  recog takes the FIRST match, so an HImode const_int -- zero
 * included -- is recognised as the memory alternative and goes through the
 * minipool, where an SImode zero is a one-instruction `mov`.  (The HImode pool
 * fix also has pool_range sixty-four, arm.md:4353, which is the same mechanism
 * Func_8091eb0's park names for its branch-over-the-pool.)
 *
 * THE FIX IS ONE NAMED LOCAL.  `int z; z = 0;` used for BOTH zero stores keeps
 * the value a register at the halfword store, so no HImode constant is ever
 * built.  The body below does that and nothing else.
 *
 * MEASURED EXACTLY INERT, all thirteen with the counts equal: `unsigned short`
 * for the named zero; an `(unsigned short)` cast at the halfword store; `int`
 * for the base with the cast at the use; the base declared after the pointer
 * and the index; an `unsigned short *` base with a halfword index; the fourth
 * argument tested through a named local first.
 * MEASURED WORSE: naming the one as well as the zero (seventeen); hoisting the
 * zero's assignment ABOVE the call (twenty-six -- it then crosses the call and
 * the whole allocation moves); the base loaded separately inside each arm
 * (forty, and one instruction long again).
 * A DIFFERENT PROGRAM, recorded so it is not re-found: advancing the base in
 * place in each arm and loading the halfword after the join reads thirty-eight
 * instructions -- ONE FEWER THAN THE REFERENCE, so it fails the memory screen.
 *
 * WHAT REMAINS IS EXACTLY THE ROTATION THE PARK NAMED, and it is now the only
 * thing left: the reference copies the FOURTH argument out of its incoming
 * register and leaves the THIRD where it arrives, modifying it in place and
 * spilling it to the stack slot before that register is reused for the first
 * argument; ours copies the third out and leaves the fourth in place.  Both
 * streams are the same length and the stack argument is stored once in each.
 * NEXT: which incoming argument register the pool/base pseudo steals.  That is
 * a local-alloc question, not a spelling one -- eight spellings are a plateau.
 */
extern int iwram_3001f2c;
extern char *_Func_801eadc(int id, int flags, int a, int b, int c);

int Func_80ae9f0(int a, int b, int c, int d)
{
    char *base;
    char *p;
    int id;
    int z;

    base = (char *)iwram_3001f2c;
    if (d == 0) {
        id = *(unsigned short *)(base + 0x392);
        c -= 3;
    } else {
        id = *(unsigned short *)(base + 0xe5 * 4);
        c -= 4;
    }
    p = _Func_801eadc(id, 0x80 << 23, a, b, c);
    if (p == 0)
        return -1;
    z = 0;
    p[4] = z;
    *(unsigned short *)(p + 0xc) = z;
    p[5] = 1;
    return 1;
}
