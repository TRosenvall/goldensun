/* RealClearChain (0x080f9a30) -- NON-MATCHING, AND NOT BY gcc-2.96.
 *
 * NON-MATCHING, 17 of 16 encodings  (MEASURED batch 330, objcmp --func).
 *   THE LENGTH ARITHMETIC, DONE EXACTLY (batch 330).  There is NO padding and
 *   NO pool on either side, so the encodings line is a pure instruction count:
 *     INSTRUCTION COUNT  ref 16, ours 18     POOL WORD COUNT  ref 0, ours 0
 *     SIZE               ref 32 bytes, ours 36
 *   The figure exceeds the reference length because the streams are two
 *   instructions apart, so it is a MISALIGNMENT reading.  THE REAL SHAPE:
 *   **the candidate is longer by exactly two instructions, and both of them are
 *   the forced lr save and its interworking return** -- `push {lr}` at the head
 *   and `pop {r0}` / `bx r0` where the ROM has a bare `bx lr`.  aligncmp puts
 *   six encodings aligned-equal of sixteen; the rest is one register-pair swap
 *   (the ROM holds the f2c pointer in r3 and the f34 pointer in r1, this body
 *   has them the other way round) and the order of two adjacent loads.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_f9000/80f9a30.c \
 *     asm/rom_f9000/rom_f95e0.s --func RealClearChain
 *
 * Blocker class: the TU was not built by gcc-2.96. This park exists to record
 * the test, not the function.
 *
 * A doubly-linked-list unlink. Sixteen instructions, no calls, and the C below
 * reproduces its structure exactly -- every load, store, branch and label in
 * the same order. It cannot match, and the reason is visible in the ROM's first
 * and last instruction:
 *
 *     rom    ldr r3, [r0, #0x2c] ... bx r14        (no prologue at all)
 *     ours   push {r14} / ldr r1, [r0, #0x2c] ... pop {r0} / bx r0
 *
 * **gcc-2.96 PUSHES lr IN ANY THUMB FUNCTION THAT HAS A CONDITIONAL BRANCH.**
 * Reproduced on three synthetic leaves: `void A(int **p) { if (p[3]) p[3][2] = 0; }`
 * -- one `if`, no calls, three instructions of work -- comes out
 * `push {lr} / ... / pop {r0} / bx r0`. A leaf with NO branch does not:
 * `int Leaf(int *p) { return p[3] + 1; }` is `ldr / add / bx lr`.
 * It is not an interwork artifact: without `-mthumb-interwork` the push is
 * still there and only the return changes, to `pop {pc}`.
 *
 * THE CENSUS, RUN IN BATCH 330.  Of the 4,893 functions in the 4,477
 * gcc-generated .s files under asm/, EVERY ONE that contains a conditional
 * branch carries a `push`.  Zero counterexamples.  So the test below is not an
 * extrapolation from three probes; it is a property of every function this
 * compiler has emitted into this tree.
 *
 *   > A ROM function that contains a conditional branch and NO `push` was not
 *   > compiled by gcc-2.96.
 *
 * WHERE THE PUSH COMES FROM, with the lines to check it (batch 330):
 *   * arm.c:9127 forces lr into live_regs_mask when
 *     `live_regs_mask || ! leaf_function_p () || thumb_far_jump_used_p (1)`;
 *     arm.c:8790-8791 repeats the same test for the epilogue.  This function is
 *     a leaf touching only r0-r3, so live_regs_mask is zero and
 *     leaf_function_p (final.c:4129-4160) is true -- the deciding term is
 *     thumb_far_jump_used_p.
 *   * That flag is STICKY.  arm.c:8610-8611 returns true without rechecking once
 *     it has been set, and its own comment says the decision cannot be revoked.
 *   * `far_jump` is YES only at a branch's MAXIMUM length -- arm.md:5169-5173
 *     for `cbranchsi4`, arm.md:5836-5840 for `*thumb_jump`.  Real lengths exist
 *     only after shorten_branches (toplev.c:3560), which runs BEFORE
 *     final_start_function (toplev.c:3601); before it get_attr_length yields
 *     `insn_default_length` (genattrtab.c:2451-2454) and
 *     insn_current_reference_address returns zero while addresses are unset
 *     (final.c:923-924).  So a pre-shorten_branches evaluation reads YES on any
 *     branch at all, however short.
 *   * arm.c:8625-8634 describes exactly this false positive and names its price:
 *     "a needless push and pop of the link register".
 *   NOT ESTABLISHED: which call actually sets the sticky flag.  The only
 *   pre-final call site is arm.h:1642 inside THUMB_INITIAL_ELIMINATION_OFFSET,
 *   and that path returns early unless regs_ever_live[ARG_POINTER_REGNUM] is set
 *   (arm.c:8634-8639), which no generic file in this gcc assigns.  Treat the
 *   last step as open; the observable behaviour above is measured, the rung is
 *   not.
 *
 * old_agbcc produces the push-less form, and that looked like the answer. IT IS
 * NOT -- CORRECTED THE ROUND AFTER THIS PARK WAS WRITTEN. Compiled with
 * `/opt/agbcc/bin/old_agbcc -mthumb-interwork -O2`, the same C gives the ROM's
 * prologue and epilogue and every branch and store in the ROM's order, but it
 * opens with `add r2, r0, #0` and runs the whole body through r2. That copy is
 * SYSTEMATIC, not incidental:
 *
 *     void F1(int *p) { p[3] = 0; }              -> mov r1, #0 / str r1, [r0, #0xc]
 *     void F2(int *p) { if (p[3]) p[4] = 0; }    -> add r1, r0, #0 / ldr r0, [r1, #0xc] ...
 *     int  F3(int *p) { return p[3] + p[4]; }    -> add r1, r0, #0 / ldr r0, [r1, #0xc] ...
 *
 * **old_agbcc copies an incoming pointer argument to another register whenever
 * it is used more than once.** The ROM uses r0 directly, four times. So the ROM
 * was not built by old_agbcc either.
 *
 * Five spellings were tried against old_agbcc -- a struct with named fields,
 * plain `*(char **)(p + 0x2c)` casts, an early `return` instead of a wrapping
 * `if`, and two orderings of the three loads -- and all five produce the copy.
 *
 * MEASURED INERT, batch 330, against gcc-2.96 (all identical to the body below):
 * the two guarded loads swapped; the three locals declared in the order the ROM
 * assigns them; and an early `return` in place of the wrapping `if`.  None of
 * them moves the register pair and none of them can touch the push, so source
 * reordering is a closed dimension here.
 *
 * WHAT THIS MEANS FOR THE `audio` CLASS. The census keeps 39 functions under
 * `audio` and the reason has never been written down beyond "hand-written
 * assembly". Two things are now established about `asm/rom_f9000`:
 *
 *   * Some of its functions ARE ordinary C -- this one is a textbook unlink,
 *     and `ply_patt` is a three-line dispatcher.
 *   * They were built by old_agbcc, which the Makefile already drives for
 *     `src/lib/m4a/%.o` and three `src/lib/agb_flash` rules.
 *
 * So the class is not "cannot be C" -- this function plainly is C. But it is not
 * "needs a per-file old_agbcc rule" either, which is what this park originally
 * claimed. NEITHER compiler in the tree produces the ROM's form: gcc-2.96 gets
 * the register usage right and the prologue wrong, old_agbcc gets the prologue
 * right and inserts a copy. Whatever built asm/rom_f9000 is a third thing.
 *
 * A CAVEAT ON SCOPE. Not every rom_f9000 body is C. `Func_80f9f3c` opens
 * `ldrb r1, [r4, #0x12]` and ends `bx lr` having never written r4 -- it takes
 * arguments in callee-saved registers and no C signature expresses that.
 * `ply_patt` ends `b ply_goto`, a sibling call gcc-2.96 does not emit. The class
 * needs sorting one by one, and the push test above sorts the first question
 * for free.
 *
 * NEXT: a per-file old_agbcc Makefile rule is NOT the next move -- the copy
 * above refutes it.  The open question is which third toolchain built this bank.
 */
struct Chain {
    unsigned char pad00[0x20];
    struct Chain *f20;
    unsigned char pad24[0x2c - 0x24];
    struct Chain *f2c;
    struct Chain *f30;
    struct Chain *f34;
};

void RealClearChain(struct Chain *p)
{
    struct Chain *x;
    struct Chain *n;
    struct Chain *v;

    x = p->f2c;
    if (x != 0) {
        n = p->f34;
        v = p->f30;
        if (v != 0)
            v->f34 = n;
        else
            x->f20 = n;
        if (n != 0)
            n->f30 = v;
        p->f2c = 0;
    }
}
