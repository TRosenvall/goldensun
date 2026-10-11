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
 *
 * ========================================================================
 * BATCH 336 BRIEF D -- THE RESIDUE IS ONE ALLOCATOR DECISION, IT IS NAMED,
 * AND IT READS **0 of 43** WITH ONE ARTIFACT.  FIGURE UNCHANGED AT
 * 13 of 43 STREAM ENCODINGS (ref 43 / ours 43, instructions 39/39, SIZE and
 * RELOCATIONS silent), BODY UNCHANGED.
 * ========================================================================
 *
 * The park above ends "NEXT: which incoming argument register the pool/base
 * pseudo steals.  That is a local-alloc question, not a spelling one -- eight
 * spellings are a plateau."  That is RIGHT, and here is the decision, with the
 * lines, plus the one fact that flips it.
 *
 * REG_ALLOC_ORDER IS `{3, 2, 1, 0, 12, 14, 4, 5, 6, 7, ...}` (arm.h:989-994):
 * **r3 is tried FIRST.**  local_alloc allocates in two phases -- quantities
 * that HAVE a suggested register first (local-alloc.c:1339-1348, calling
 * `find_free_reg (min_class, mode, q, 0, 1, birth, death)` with
 * just_try_suggested = 1), then everything else (:1385-1400).
 *
 *   * `d`'s parm copy `(set (reg 35) (reg:SI 3 r3))` gives its quantity a COPY
 *     SUGGESTION of r3 via combine_regs, and `.17.lreg` says
 *     `Register 35 used 2 times across 4 insns IN BLOCK 0` -- block-local, so
 *     greg never sees it (`.18.greg`: `;; 6 regs to allocate: 37 34 36 38 32 33`,
 *     with 35 and 40 both ABSENT).  Phase 1 hands `d` **r3**.
 *   * the literal-pool address pseudo (reg 40, also block-local, no suggestion)
 *     reaches phase 2 with r3 taken, and reg_alloc_order gives it **r2**.
 *   * that puts hard r2 into `c`'s conflict set -- `.18.greg` reads
 *     `;; 34 conflicts: 32 33 34 35 36 38 40 43 46 47 1 2 3 13` -- so greg
 *     CANNOT leave `c` in its incoming register and evicts it to r4.
 *
 * Every one of the 13 differing encodings is that one swap and its
 * consequences, including the park's `add r3, r2, r1` (ours) against
 * `add r3, r1` (the ROM's destructive form, because the ROM's base IS r3), and
 * the ROM's `str r2,[sp]` floating AHEAD of `mov r2, r4` because the ROM's
 * stack store has a dependence ours does not.
 *
 * THE PRECONDITION THAT FLIPS IT IS CALL-CROSSING, NOT GLOBALITY.  In
 * `find_free_reg` the exclusion set is built as
 *
 *     else if (qty[qtyno].n_calls_crossed == 0) COPY_HARD_REG_SET (used, fixed_reg_set);
 *     else                                      COPY_HARD_REG_SET (used, call_used_reg_set);
 *
 * so a quantity that CROSSES A CALL cannot take r0-r3 -- and with the build's
 * `-fcall-used-r4`, not r4 either.  Phase 1 then fails for `d`, phase 2 walks
 * reg_alloc_order past r3/r2/r1/r0/r12/r14/r4 and lands on **r5**: the ROM.  r3
 * falls free for the pool pseudo and `c`'s r2 conflict disappears.
 *
 * MEASURED (ref 43 / ours 43 on every row):
 *   this body                                                        13
 *   + `d` live ACROSS the call  (`__asm__ volatile ("" :: "r"(d))`
 *     placed after `if (p == 0) return -1;`)                         **0**
 *   + `d` live across the call as a PIN, `register int d5 __asm__("r5")`  **0**
 *   + `d` merely multi-block, dying BEFORE the call (same asm placed
 *     after the if/else)                                             14,
 *       and `d` lands in **r1** -- the discriminator.  This probe gets the
 *       `c`-stays-in-r2 half RIGHT (`sub r2,#3` / `sub r2,#4` in place, base in
 *       r3) and then puts `d` in r1, because a non-call-crossing allocno still
 *       has r0/r1 available.  **Globality alone is not enough; only the
 *       call_used_reg_set exclusion reaches r5.**
 *   a bare `__asm__ volatile ("")` anywhere                          13 (inert)
 *
 * MEASURED INERT OR WORSE, device-free, this round (ref 43 unless noted):
 *   base re-read from the global inside each arm         40, RELOCDIFF
 *   `c -= (d == 0) ? 3 : 4;` after the join (d global)   40, RELOCDIFF
 *   condition into a named local first                   44, RELOCDIFF
 *   pointer per arm, dereferenced after the join         40, RELOCDIFF
 *   pointer per arm, dereferenced IN the arm             19
 *   arms swapped (`if (d != 0)` leading)                 19
 *   `unsigned short id`                                  44, RELOCDIFF
 *   `c` staged through a named local                     41, RELOCDIFF
 *   `unsigned short *` base with halfword indices        13 (inert)
 *
 * SO THE BOUND, WITH ITS EVIDENCE ATTACHED.  Device-free, `d` must be a
 * quantity with `n_calls_crossed > 0`; that is the ONLY branch of
 * `find_free_reg`'s exclusion set that removes r3 from a copy-suggested
 * block-local quantity, and `regs_live_at` cannot supply r3 because hard r3
 * dies at `d`'s own parm copy (insn 10) while the pool pseudo is born at insn
 * 18.  `n_calls_crossed > 0` requires a LIVE USE of `d` after the `bl`, and
 * this tail has none -- clear +4, clear +0xc as a halfword, set +5 to 1,
 * return 1.  **So the original source used its fourth argument after the
 * allocation call in a way the emitted code does not show.**  Same conclusion
 * on the TU mate Func_80ae99c, whose tail is identical.
 *
 * CORPUS, and the result is a real bound rather than an absence: of the **4,522
 * generated (byte-matching) `.s` files**, **45** carry the shape "evict r3 into
 * a callee-saved register, then load a literal-pool address into r3".  The two
 * closest are OvlFunc_947_200901c (src/overlays/rom_7d0e88/ovl_314_c_a_a_b.c)
 * and OvlFunc_947_2008fcc (.../ovl_314_c_a_a_a_c_b.c); in BOTH the fourth
 * argument is a pointer used far later in the body, so it is a greg allocno
 * whose span conflicts with r0-r3, and neither needs a call.  **No generated
 * file has this shape with the fourth argument dead at the compare.**
 *
 * TOOLING WARNING, the exact trap docs/elevation.md names.  Classifying
 * generated vs hand-written `.s` with `grep -rl gcc2_compiled` is WRONG: the
 * COMMENT at asm/rom_a1000/rom_ae88c_c_c_c.s:5 contains the literal text
 * `.gcc2_compiled.` (so does asm/rom_9000/rom_b074_a_c.s:305), and both files
 * are hand-written.  And matching a pool load as `ldr rX, =sym` finds ZERO
 * generated hits, because gcc emits `ldr rX, .L5` -- the `=` form is
 * hand-written assembler syntax only.  Classify by the FIRST LINE
 * (`@ Generated by gcc` vs `	.include "macros.inc"`), strip comments, and
 * match both pool spellings.
 *
 * NEXT: nothing source-level remains that this project can state.  Treat the
 * 13 as bounded on the `find_free_reg` citation above and report disagreement
 * with THAT line, not with another spelling.  The piece
 * (asm/rom_a1000/rom_ae88c_c_c_c.s, two functions + .rodata) is **two pins from
 * byte-exact on BOTH members** -- one `register int d5 __asm__("r5") = d;` each,
 * same pin, same reason -- which makes it a strong pass-3 candidate.
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
