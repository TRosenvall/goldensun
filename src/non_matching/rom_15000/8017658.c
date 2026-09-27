/* Func_8017658 -- NON-MATCHING, 109 encodings of 127.  SIZE EXACT (276 bytes both), 128
 * instructions against 127, six relocations each off by 2-4 bytes (that one instruction).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8017658.c \
 *     asm/rom_15000/rom_15e8c_c_c_a.s --func Func_8017658
 *
 * ITS .s HOLDS ONLY THIS FUNCTION AND Func_801776c (parked at 6 of 137, next door), IS
 * CLEAN OF `.section` DATA, AND SO CONVERTS WHOLE THE MOMENT BOTH MATCH -- no linker edit.
 * That makes this pair unusually valuable for its size.
 *
 * BLOCKER: THE cse-ZERO CLASS, IN cse1, AND IT IS TRACED IN RTL RATHER THAN INFERRED.  In
 * `.03.cse` insn 23 `(set (reg/v:SI 43) (const_int 0))` is DELETED and every use
 * canonicalised to reg 42 (`opts`); the dump shows `(subreg:HI (reg 42) 0)` as the 0x12f6
 * store and `(ior:SI (reg 42) ...)` four times.  That one merged pseudo is why this runs
 * r4,r5,r6,r7,r8,r10 where the ROM runs r4-r10.
 *
 * THE BATCH-284 BOUND HOLDS AND CLOSES THE OBVIOUS ROUTE: the ROM's shared zero is r6,
 * which is CALLEE-SAVED, so cse1's invalidate_for_call cannot reach it and the
 * pinned-call-clobbered-register lever is unavailable.  The `"i"(0)` asm flush costs more
 * than it saves (125/127/125 against 110).  And `-fno-rerun-cse-after-loop` takes 110 to
 * 60 BUT LEAVES THE MERGE, so CSE_CFLAGS does not reach this either.
 */
/* Func_8017658 (OpenTextBoxAt) -- NON-MATCHING, 109 encodings of 127 against
 * asm/rom_15000/rom_15e8c_c_c_a.s.  SIZE EXACT (276 bytes both).  128 instructions
 * against 127 -- ONE over.  All six relocations sit 2-4 bytes off, which is that
 * one instruction, not six independent errors.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_15000/rom_15e8c_c_c_a.s --func Func_8017658
 *
 * NO SHIM, NO asm, NO PIN.  Plain C throughout.
 *
 * THREE LEVERS LANDED HERE, and two are new and general.
 *
 * 1. FRAME-SLOT ORDER FOR ADDRESS-TAKEN LOCALS IS THE ORDER THE ADDRESSES ARE
 *    TAKEN, AND EARLIER MEANS A HIGHER OFFSET.  Declaration order is INERT --
 *    all six permutations of (w,h,e) and 26 permutations of the full nine-local
 *    block measured IDENTICALLY.  What moved the slots was `ep = e;` as a
 *    statement at the top: that takes e's address before the call does, so e
 *    gets the TOP slot (0x1c) and the call's own &x/&y/&w/&h fill 0x18/0x14/
 *    0x10/0xc in ASCENDING ARGUMENT ORDER downward.  Without it e is allocated
 *    last and lands at 0xc, the bottom -- the ROM's layout inverted.  126 -> 110.
 *
 * 2. A TWO-WORD OUT-PARAMETER IS EIGHT BYTES, AND THE FRAME SIZE IS THE TELL.
 *    ROM `sub sp,#0x24` against our `sub sp,#0x20` with the same seven pushed
 *    registers.  Five slots at 0xc..0x1f is 0x20; the extra word belongs to `e`.
 *    Declaring it `int e[2]` (or `short e[4]`, or `unsigned short e[4]` -- all
 *    three measure IDENTICALLY at 109) makes the frame 0x24 and the prologue AND
 *    epilogue byte-exact.  Cross-check: the Func_80165d8 park says that callee
 *    "copies four halfwords in", i.e. eight bytes.  110 -> 109.
 *
 * 3. THE TAIL ZERO MUST BE A NAMED LOCAL, NOT A LITERAL, TO BUY THE THIRD HIGH
 *    REGISTER.  With `0` written at the three tail stores gcc saves two high
 *    registers (`push {r6,r7}`); with a named `z2` it saves three
 *    (`push {r5,r6,r7}`), the ROM's prologue.  Its ASSIGNMENT POSITION is the
 *    whole lever: `z2 = 0;` after the `if (w == 0 && h == 0)` join label keeps a
 *    separate pseudo, because cse1's table is invalidated at a label with two
 *    predecessors.  Placed after the CreateUIBox call or after the box check it
 *    is commoned with the earlier zero and the third register is lost again
 *    (measured: top 123, afterwh 110, beforecreate 125, aftercreate 120,
 *    afterbox 120).
 *
 * BLOCKER: THE cse-ZERO CLASS, IN cse1, AND IT IS THE BANK'S OWN BLOCKER.
 *
 * The ROM keeps `z` (the 0x12f6 halfword store plus Func_801868c's 7th argument)
 * in r6 and `opts` in r7 -- two registers for two const-0 pseudos whose live
 * ranges overlap, because `opts = 0` is hoisted above the early-return branch
 * while `z` is still live to the call.  gcc-2.96 commons them, and that ONE
 * merged pseudo is why our allocation runs r4,r5,r6,r7,r8,r10 where the ROM runs
 * r4..r10: with z and opts separate, p/ep/n are pushed into r8/r9/r10 and every
 * remaining difference -- `mov r2,r8 / ldrh r3,[r2,r3]` for the register-offset
 * load, `add r3,r8` for the two tail byte stores, the register names throughout
 * -- follows.
 *
 * TRACED IN THE RTL, NOT INFERRED.  In .03.cse (cse1, the FIRST pass) insn 23
 * `(set (reg/v:SI 43) (const_int 0))` is DELETED and every use of reg 43 is
 * canonicalised to reg 42, which is `opts`: the dump shows
 * `(subreg:HI (reg/v:SI 42) 0)` as the 0x12f6 store and `(reg/v:SI 42)` as the
 * stack argument, then `(ior:SI (reg/v:SI 42) ...)` four times.  Dump with
 * `-da` and read `<stem>.c.03.cse`.
 *
 * THE BOUND FROM BATCH 284 HOLDS AND IS RE-MEASURED HERE.  The ROM's shared zero
 * is r6 -- CALLEE-SAVED -- so invalidate_for_call cannot separate them and the
 * pinned call-clobbered-register route is closed.  The `"i"(0)` cse flush, which
 * DOES produce ASM_OPERANDS and DOES clear the table, costs more than it saves,
 * exactly as Func_801c49c measured: 125 with the flush before `opts = 0` against
 * 110 without, 127 with it after `z = 0`, 125 before the call.  A no-operand
 * `asm volatile("")` is inert on cse as documented (111 / 61 with
 * -fno-rerun-cse-after-loop) and only perturbs scheduling.
 *
 * DROP LADDER, all measured against the tree reference:
 *   first candidate                                    126 of 125 vs 127
 *   `int *ep = &e;` at the top (slot order)             110 of 127
 *   `int e[2]` (frame 0x24)                             109 of 127
 *   declaration order: 6 + 26 + 5 permutations          ALL INERT
 *   `opts = z;` / `z = opts;` / reversed order          ALL 109 (cse folds the copy)
 *   `short z` / `unsigned short z` / `char z`           inert
 *   -fno-rerun-cse-after-loop                           60 differing, structure
 *                                                       exact but z/opts STILL
 *                                                       merged -- the flag does
 *                                                       not reach cse1
 *   -fno-cse-follow-jumps / -fno-thread-jumps /
 *     -fno-schedule-insns / -fno-strength-reduce        inert
 *
 * So no per-file Makefile flag reaches this: CSE_CFLAGS (-fno-rerun-cse-after-loop)
 * cleans the downstream noise but leaves the merge, because the merge is cse1.
 */
extern unsigned char *iwram_3001e8c;
extern int BufferString(int id, int mode);
extern void Func_801868c(int n, int *x, int *y, int *w, int *h, int *e, int g);
extern void *CreateUIBox(int x, int y, int w, int h, int opts);
extern int Func_80165d8(void *box, int n, int c, int d, int *e, int f);
extern void CloseUIBox(void *box, int mode);

void *Func_8017658(int id, int x, int y, unsigned int flags)
{
    unsigned char *p;
    int n;
    int w;
    int h;
    int e[2];
    int *ep;
    int opts;
    int z;
    int z2;
    void *box;

    p = iwram_3001e8c;
    z = 0;
    opts = 0;
    ep = e;
    *(short *)(p + 0x12f4) = (flags << 4) >> 20;
    *(short *)(p + 0x12f6) = z;
    flags &= 0xffff;
    n = BufferString(id, 1);
    if (*(unsigned short *)(p + (0xeb << 4) + n * 2) == 0)
        return 0;
    Func_801868c(n, &x, &y, &w, &h, ep, z);
    if (w == 0 && h == 0)
        return 0;
    z2 = 0;
    if (!(flags & 1))
        opts |= 2;
    if (flags & 8)
        opts |= 8;
    if (flags & 0x10)
        opts |= 0x80;
    if (flags & 0x20)
        opts |= 0x100;
    box = CreateUIBox(x, y, w, h, opts);
    if (box == 0)
        return 0;
    if (Func_80165d8(box, n, 0, 0, ep, z2) == 0) {
        CloseUIBox(box, 1);
        return 0;
    }
    *(char *)(p + 0x12fa) = z2;
    *(char *)(p + 0x12fb) = z2;
    return box;
}
