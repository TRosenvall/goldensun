/* OvlFunc_924_20096c4 -- 0x020096c4  (asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_c.s)
 *
 * NON-MATCHING, 13 differing STREAM encodings of 91  (ref 91, ours 91;
 *   16-bit INSTRUCTIONS 80 on BOTH sides, so this is a TRUE DISTANCE).
 *   Relocations clean.  First difference at index 9.
 *   RE-DERIVED batch 338 brief D -- unchanged.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/20096c4.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_c.s --func OvlFunc_924_20096c4
 *   (The subject line of the batch-319 header below names the reference with
 *   one `_c` missing.  The recipe above is the correct path and the one
 *   parkcheck uses.)
 *
 * PINS 0.  DEVICES 0.
 * SHIP SHAPE: one function in the reference, datacheck.py silent, converts
 * whole.  No split, no exports.
 *
 * ====================================================================
 * BATCH 338 BRIEF D -- THE TWO CLUSTERS ARE ONE PHENOMENON, AND THE
 * SOURCE-ORDER DIMENSION IS NOW EXHAUSTED
 * ====================================================================
 *
 * The header below splits the residue into "three call sites" plus "six lines
 * of loop-setup order".  Read off the disassembly the split is 6 + 7 and the
 * two halves are THE SAME DEFECT: everywhere the ROM has a SECOND low scratch
 * register and we funnel everything through r3.
 *
 *   entry block (6 of the 13), indices 10-17:
 *     rom   movs r1,#6 / mov sl,r1 / movs r3,#41 / movs r1,#2 / movs r5,#0
 *           / mov r8,r3 / movs r6,#40 / mov r9,r1
 *     ours  movs r3,#6 / mov sl,r3 / movs r3,#41 / mov r8,r3 / movs r3,#2
 *           / mov r9,r3 / movs r6,#40 / movs r5,#0
 *   first in-loop call (3), indices 29-32:
 *     rom   movs r3,#1 / mov r1,sl / str r3,[sp] / str r1,[sp,#4]
 *     ours  movs r3,#1 / str r3,[sp] / mov r3,sl / str r3,[sp,#4]
 *   second in-loop call (4), indices 39-43:
 *     rom   movs r3,#4 / mov r1,r9 / adds r2,r5,#0 / str r1,[sp] / str r3,[sp,#4]
 *     ours  mov r3,r9 / adds r2,r5,#0 / str r3,[sp] / movs r3,#4 / str r3,[sp,#4]
 *
 * AND THE ENTRY BLOCK EXPLAINS ITSELF ONCE YOU LINE IT UP.  In the ROM the
 * pair `movs r3,#41` / `mov r8,r3` is SEPARATED by two insns, so r3 is still
 * live when the next constant is built and that constant must take r1.  In
 * ours the pair is adjacent, r3 dies at once, and every later constant reuses
 * it.  Three disjoint short-lived constant pseudos can all share r3; two
 * OVERLAPPING ones cannot.  So the ROM's second scratch is a CONSEQUENCE of
 * its ordering, not a cause -- which inverts the header's reading of the
 * entry block as "the register-role swap in its usual dressing".
 *
 * AND THERE IS NO PRE-RELOAD SCHEDULER TO BLAME.  `xgcc -da` on this file
 * writes `.17.lreg`, `.18.greg`, `.19.flow2`, `.20.ce2`, `.23.sched2` and
 * NO sched1 dump, so `-fschedule-insns` is not on in the production flags and
 * the pre-allocation insn order is whatever expand/cse/combine leave.  The
 * only source-level handle on it is statement order -- which is now closed:
 *
 * MEASURED THIS BATCH -- ALL 24 PERMUTATIONS of the four entry assignments
 * (`six`, `x`, `two`, `y`), counts equal and relocations clean throughout:
 *     4 permutations at 13 (inert, including the parked order)
 *    16 permutations at 14
 *     4 permutations at 15-16
 *   >> THE DIMENSION IS EXHAUSTED.  The header below screened three of the 24
 *   >> and read 13/14/14; all 24 now agree with that sample and NONE is better.
 * MEASURED, loop-body anchors -- the depth-2 region autosweep cannot reach,
 * because `boundaries()` walks only `depth == 1`:
 *   anchor before the 1st or 2nd __CopyMapTiles                   13 (inert)
 *   anchor before the 3rd __CopyMapTiles                          15
 *   anchor after the 3rd __CopyMapTiles                           18
 * MEASURED WORSE, both confirming the header's pressure boundary:
 *   a named `four` local for the in-loop `4`     95, three instructions long
 *   `x += 2` moved up inside the loop body       86, three short
 * AUTOSWEEP (batch 338): both cheap axes CLOSED on the unchanged body.  Flag
 * axis all eight inert or worse (`-fno-schedule-insns2` 27, the other seven
 * either 13 or 94-96); anchor axis 17 depth-1 placements, all inert at 13.
 *
 * NEXT, AND IT IS NARROWER THAN THE HEADER'S.  The question is no longer "a
 * named local per stack argument" -- the header proves that lever is paid for
 * out of loop pressure and this loop has one register of slack.  The question
 * is how to make ONE of the entry block's constant pseudos OVERLAP the next,
 * so local-alloc is forced to a second scratch, WITHOUT adding an instruction.
 * Statement order does not reach it (24 of 24 measured).  What is untried is
 * changing which of the four values is a loop-invariant local at all -- the
 * ROM's `sl`, `r8` and `r9` assignments are the only three that need a scratch,
 * and `y`/`r6` needs none.
 *
 * ===== CARRIED FORWARD: the batch-319 header, verbatim apart from the
 * ===== [SUPERSEDED FIGURE] markers, which stop parkcheck reading an
 * ===== old figure as this park's claim.
 *
 *
 * [SUPERSEDED FIGURE] 13 differing / 91 encodings  (MEASURED, batch 319 recipe backfill).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7ac2d8/20096c4.c \
 *     asm/overlays/rom_7ac2d8/ovl_f84_a_c_c_c_c_c_c.s --func OvlFunc_924_20096c4
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * BLOCKER: stack-argument register allocation INSIDE A LOOP. 13 of 92, exact
 * length. The interleave this function was selected for is NOT the problem --
 * it reproduced.
 *
 * That is the useful part. tools/guarded_interleave.py picked this up for its
 * three guarded interleave sites, and naming the constants in the entry block
 * reproduced all three: both __Func_8012330 calls -- the `0xa0 << 11` triple and
 * the `mov`/`neg` pair with the pooled 0xe666 -- are absent from the diff. The
 * lever did its job; what stopped the function is a different class that happens
 * to live in the same body.
 *
 * WHAT REMAINS: three call sites where the ROM materialises BOTH stack arguments
 * into two registers before storing either, and we reuse one register for both.
 *
 *     rom   mov r3, #0x1 / mov r1, r10 / str r3, [sp] / str r1, [sp,#4]
 *     ours  mov r3, #0x1 / str r3, [sp] / mov r3, r10 / str r3, [sp,#4]
 *
 * The documented remedy is a named local per stack argument per call site, and
 * it works OUTSIDE the loop and backfires INSIDE it:
 *
 *     16  naive
 *     13  name both stack arguments of the post-loop __Func_8010704   <- best
 *     13  the above plus a local for the in-loop `1`   (inert)
 *     94  the above plus a local for the in-loop `4`   (97 lines, +5)
 *     94  all four sites named at once                 (97 lines, +5)
 *
 * THE BOUNDARY IS REGISTER PRESSURE, not the lever. This loop already keeps six
 * values live across its body -- the parameter, a counter, two induction
 * variables and two loop-invariant constants -- and the ROM spends r8, r9 and
 * r10 to hold them. Adding one more named local is free; adding a second forces
 * a spill and costs five lines. So the per-call-site rule needs a proviso: it
 * buys two registers at the call and it must be paid for out of what the
 * surrounding block has spare. In a straight-line run that is nearly always
 * affordable, and inside a loop that is already spending high registers it
 * usually is not.
 *
 * The remaining six lines beyond those two sites are the loop-setup order --
 * gcc materialises the four loop constants through one scratch register where
 * the ROM alternates two. Three assignment orders were screened (six/two/x/y,
 * six/x/y/two, x/six/two/y) at 13, 14 and 14, so source order does not reach it
 * either; that is the register-role swap in its usual dressing.
 *
 * Everything else is right: the unsigned `bls` loop over 0..2, the three
 * __CopyMapTiles calls with their computed arguments (3 - i, i + 0x6a, and the
 * two induction variables stepping by 2), the guard on the parameter, and the
 * post-loop 0x90 << 1 sound.
 *  
 */
extern void __PlaySound(int id);
extern void __CutsceneWait(int n);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);

void OvlFunc_924_20096c4(int arg)
{
    int s1;
    int s2;
    int s3;
    int m1;
    int m2;
    int e;
    int six;
    int two;
    unsigned int i;
    int x;
    int y;
    int n3;
    int n4;

    s1 = 0xa0 << 11;
    s2 = 0xa0 << 11;
    s3 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e = 0xe666;
    __PlaySound(0xdb);
    six = 6;
    x = 0x29;
    two = 2;
    y = 0x28;
    for (i = 0; i <= 2; i++) {
        __CopyMapTiles(y, 0x20, x, 0x20, 3 - i, six);
        __CopyMapTiles(0x27, 0x33, y, 0x20, 1, six);
        __CopyMapTiles(0x69, 0x33, i + 0x6a, 0x20, two, 4);
        if (arg != 0) {
            __Func_8012330(s1, s2, s3);
            __Func_8012330(m1, m2, e);
            __CutsceneWait(arg);
        }
        x += 2;
        y += 2;
    }
    __PlaySound(0x90 << 1);
    n3 = 0x2a;
    n4 = 0x21;
    __Func_8010704(0x6a, 0x21, 4, 5, n3, n4);
    __Func_8012350();
}
