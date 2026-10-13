/* Func_8017658 (OpenTextBoxAt) -- NON-MATCHING.
 * 109 of 127 encodings.  THREE DIMENSIONS, RE-DERIVED batch 348 brief E:
 *   16-bit instructions   116 ref / 118 ours   (+2)
 *   32-bit entries        11 ref  / 10  ours   (-1)   [5 of these are `bl`s,
 *                                                      so 6 pool words / 5]
 *   bytes                 276 / 276            (EXACT -- the +4 from the two
 *                                               instructions and the -4 from the
 *                                               missing pool word CANCEL)
 * The old header's "128 instructions against 127" was the STREAM length, which
 * counts pool words; it is not the instruction count and it hid the pool
 * dimension entirely.  SIZE EXACT HERE MEANS NOTHING -- two dimensions moved in
 * opposite directions.
 * Aligned residue (tools/aligncmp.py): 60 of 127, in 29 hunks.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_15000/8017658.c \
 *     asm/rom_15000/rom_15e8c_c_c_a_a.s --func Func_8017658
 *
 * ============================================================
 * THE +2 IS FOUR REGIONS OF OPPOSITE SIGN, not one site
 * ============================================================
 * Measured off the six relocation offsets, which are the only landmarks both
 * streams share (ref -> ours):
 *   start .. BufferString      0x30 -> 0x32    ours +1 instruction
 *   .. Func_801868c            0x5e -> 0x5c    ours -2
 *   .. CreateUIBox             0xae -> 0xb0    ours +2
 *   .. Func_80165d8            0xca -> 0xcc    ours  0
 *   .. CloseUIBox              0xd6 -> 0xd8    ours  0
 *   .. first pool word         0xfc -> 0x100   ours +1
 * Net +2.  A brief that treats this as "one site of two" is looking in the
 * wrong four places.
 *
 * ============================================================
 * THE MISSING POOL WORD IS NAMED, AND IT CONFIRMS THE PARK'S DIAGNOSIS
 * ============================================================
 * The ROM's pool is  iwram_3001e8c, 0x12f4, 0x12f6, 0xffff, 0x12fa, 0x12fb.
 * Ours is the same WITHOUT 0x12fb.  The tail stores read:
 *     rom   ldr r3,=0x12fa / add r3, r8 / strb r5,[r3] /
 *           ldr r3,=0x12fb / add r3, r8 / strb r5,[r3]      7 insns, 2 pool words
 *     ours  ldr r0,=0x12fa / mov r2, sl / adds r3, r7, r0 /
 *           adds r0, #1    / strb r2,[r3] / adds r3, r7, r0 / strb r2,[r3]
 *                                                            8 insns, 1 pool word
 * That `adds r0, #1` is reload_cse_move2add (reload1.c:8840-8990), rewriting the
 * second constant as an add off the same hard register (:8884-8893).  IT FIRES
 * FOR US AND NOT FOR THE ROM FOR ONE REASON: the ROM's `p` is in r8, a HIGH
 * register, so the address add is the two-operand Thumb hi form `add r3, r8`
 * which CLOBBERS r3 and kills `reg_set_luid` for the constant; ours has `p` in
 * r7, a LOW register, so the add is the three-operand `adds r3, r7, r0` and r0
 * survives holding 0x12fa for move2add to find.
 * So the pool dimension is NOT an independent defect and NOT the HImode-store
 * class -- it is DOWNSTREAM of the register rotation the park already names.
 * The same rotation produces the region-B -1: `mov r2,r8 / ldrh r3,[r2,r3]`
 * (register-offset load off a high base) against our `adds r3,r3,r7 /
 * ldrh r3,[r3,#0]`.
 *
 * REGION C's +2 IS `z2`.  Ours emits `movs r3,#0 / mov sl,r3` at the join --
 * z2 is materialised into sl, a HIGH register, needing a low-register carrier --
 * where the ROM has a plain `movs r5,#0` in a low register just before
 * Func_80165d8.  The park's lever 3 (z2 assigned after the `w==0 && h==0` join)
 * bought the third pushed high register and is still right about the prologue,
 * but it put z2 itself in sl.  That is the two instructions of region C.
 *
 * ============================================================
 * THE cse1 MERGE REPRODUCES EXACTLY -- the park's diagnosis SURVIVES
 * ============================================================
 * Confirmed in the emitted stream, not just the dump: `movs r6,#0` feeds the
 * 0x12f6 `strh`, the `str r6,[sp,#8]` seventh argument AND all four
 * `orrs r6, ...` -- ONE pseudo for z and opts.  The ROM has `movs r6,#0` for z
 * and a separate `movs r7,#0` for opts.  Register roles:
 *     rom   r5 flags, r6 z, r7 opts, r8 p,  r9 ep, sl n
 *     ours  r5 flags/box, r6 z+opts, r7 p,  r8 n,  r9 ep, sl z2
 * One merged pseudo frees one register, z2 takes it, and every role shifts down
 * one.  Everything above -- the pool word, region B, region C -- follows from
 * that single shift.  This is the rare park whose inherited verdict is RIGHT.
 *
 * ============================================================
 * BOTH MECHANICAL AXES ARE NOW CLOSED, measured batch 348 (autosweep)
 * ============================================================
 *   FLAG AXIS, 8 groups: -fno-strength-reduce and -fno-rerun-loop-opt
 *     bit-identical at 109; -fno-gcse 103 (aligned 74, worse, +1 insn);
 *     -fno-rerun-cse-after-loop 108 (aligned 62, worse, +3 insn);
 *     -fno-strict-aliasing 115; -O1 119 (-1 insn); -fno-schedule-insns2 120;
 *     -ffixed-r7 132.
 *   ANCHOR AXIS, 23 of 23 boundaries (NOT capped): all inert, best 109 = base.
 *
 * AND ONE FLAG ROW IS EVIDENCE, NOT NOISE: -ffixed-r7 is the ONLY row whose
 * ALIGNED residue improves, 60 -> 53, while ndiff rises to 132 and the
 * instruction count goes +8.  It improves because it takes r7 away from `p`,
 * which is exactly the mechanism above.  It is unusable alone (+8) and is the
 * obvious CROSSING PARTNER for any source change that separates z from opts.
 *
 * NEXT.  Unchanged in direction, sharpened in target: separate the two const-0
 * pseudos in cse1.  The batch-284 bound still holds (the shared zero is r6,
 * callee-saved, so invalidate_for_call cannot reach it) and the `"i"(0)` flush
 * still costs more than it saves.  What has NOT been tried is a source form in
 * which `opts`'s first definition is not a bare `const_int 0` at all -- the ROM
 * sets `movs r7,#0` before the early-return branch, so opts is live there, and
 * any spelling whose initial value cse cannot equate to z's zero would do.
 * Cross whatever that is with FIXEDR7_CFLAGS.
 */
/* Func_8017658 -- NON-MATCHING, 109 encodings of 127.  SIZE EXACT (276 bytes both), 128
 * instructions against 127, six relocations each off by 2-4 bytes (that one instruction).
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_15000/8017658.c \
 *     asm/rom_15000/rom_15e8c_c_c_a_a.s --func Func_8017658
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
 * asm/rom_15000/rom_15e8c_c_c_a_a.s.  SIZE EXACT (276 bytes both).  128 instructions
 * against 127 -- ONE over.  All six relocations sit 2-4 bytes off, which is that
 * one instruction, not six independent errors.
 *
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_15000/rom_15e8c_c_c_a_a.s --func Func_8017658
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
