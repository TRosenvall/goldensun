/* RealClearChain (0x080f9a30) -- NON-MATCHING, AND NOT BY gcc-2.96.
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
 * Measured on three synthetic leaves: `void A(int **p) { if (p[3]) p[3][2] = 0; }`
 * -- one `if`, no calls, three instructions of work -- comes out
 * `push {lr} / ... / pop {r0} / bx r0`. A leaf with NO branch does not:
 * `int Leaf(int *p) { return p[3] + 1; }` is `ldr / add / bx lr`.
 *
 * It is not an interwork artifact. Without `-mthumb-interwork` the push is
 * still there and only the return changes, to `pop {pc}`.
 *
 * SO THE TEST IS FREE, and needs no screen:
 *
 *   > A ROM function that contains a conditional branch and NO `push` was not
 *   > compiled by gcc-2.96.
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
 * NEXT: try this function with an old_agbcc Makefile rule.
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
