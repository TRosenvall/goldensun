/* OvlFunc_883_200d64c -- NON-MATCHING, 55 of 106 encodings; ALIGNED residue 38.
 *   LENGTH GAP CLOSED: instructions 100/100, SIZE 224/224 bytes.
 *   WAS: 65 of 106, instructions 100/102, SIZE 224 against 228.  (batch 345 D)
 *   RELOCATIONS still differ: the SAME symbols at a shifted offset, which this
 *   project treats as a consequence, not a separate blocker (batch 322).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_780898/200d64c.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c.s --func OvlFunc_883_200d64c
 *
 * READ THE QUALIFIER FIRST: 65 -> 55 IS MOSTLY PHASE.  The aligned residue went
 * 39 -> 38, i.e. ONE encoding.  What the change bought is that the figure is now
 * READABLE: instruction counts and byte size both equal, so the 55 is a distance
 * and the next agent is not measuring offset.  Exactly the batch-344 Func_8093af8
 * caution, applied honestly.
 *
 * PER-REGION SPLIT of the old +1 (ref indices):
 *   prologue + first guard        0-29   ref 30 / ours 30   equal
 *   second guard + 200d610 call  30-45   ref 16 / ours 16   equal
 *   atan2 argument setup         46-53   ref  8 / ours  7   OURS -1
 *   atan2 + mask chain           54-68   ref 14 / ours 14   equal
 *   the three mask compares      69-76   ref  8 / ours  9   OURS +1
 *   win/lose tail + epilogue     77-104  ref 28 / ours 29   OURS +1
 * NET +1, FROM THREE SITES, AND THE -1 IS AS INFORMATIVE AS THE +1s: the ROM
 * pays one `mov r2, r8` to get `a + 8` out of a high register for a `ldr`, and
 * we pay one `mov r3, r8` at EACH of two `cmp d, #0` sites plus one at each of
 * the two `strb ..., d` sites.  Same high register, opposite occupant.
 *
 * WHAT THE ANCHOR DID, and why it is not the allocator.  `do { } while (0);`
 * between `r = 1;` and `a[0x62] = 1;` in the win arm (autosweep anchor14 of 21)
 * closes the length gap.  THE ALLOCATION IS BIT-FOR-BIT UNCHANGED -- the
 * .18.greg allocno order and every disposition are identical with and without
 * it (measured, both dumps).  What it stops is a CROSS-JUMP in jump2: with d in
 * r8 both arms end `mov rN, r8 / mov r2, <ptr> / strb r3, [r2]`, the two tails
 * become textually identical, and find_cross_jump merges the trailing `strb`
 * into a shared `.L9`, which the ROM does not do.  So the old +1 and the
 * cross-jump were the SAME defect seen twice.
 *
 * THE REAL BLOCKER IS ONE ALLOCNO ORDER AND IT IS NOW FULLY QUANTIFIED.
 * allocno_compare (global.c:597-621) is
 *     pri = (double)(floor_log2 (n_refs) * n_refs) / live_length * 10000 * size
 * truncated to int, ties on ASCENDING allocno number (:618-620).  Measured out
 * of .17.lreg and .18.greg for this body (tools in scratch_elev/b345/D):
 *
 *   allocno  what       refs/length   pri    ours    ROM
 *      32    a             10/51      5882    r5      r5
 *      56    b + 8          3/12      2500    r6      r7
 *      57    a + 8          3/12      2500    r7      r8
 *      35    d (parm 4)     5/47      2127    r8      r6
 *      86    a + 0x62       5/49      2040    sl      r9
 *      44    a + 0x5b       4/43      1860    r9      sl
 *      33    b              3/20      1500    fp      fp
 *      34    c              2/17      1176   SPILL   SPILL (ROM: [sp,#4])
 *      36    r              4/100      800   SPILL   SPILL (ROM: [sp,#0])
 *
 * The park said "by that formula the parameter genuinely loses ... raising it
 * needs three more references or a much shorter live range".  THE ARITHMETIC IS
 * RIGHT AND THE BOUND IS NOT: it needs ONE more reference, not three.
 *   floor_log2(5)*5 = 10 -> 10/47 = 2127;  floor_log2(6)*6 = 12 -> 12/47 = 2553,
 * and 2553 > 2500 is all that is required.  Equivalently live_length 47 -> 39
 * (10/39 = 2564).  Those are the two exact targets, and neither is "three refs".
 *
 * THE SECOND SWAP NEEDS A SEPARATE LEVER, which is why no single change here
 * lands it: 44 must also pass 86, i.e. 8/L44 > 10/L86.  Sufficient alternatives,
 * all computed from the table above:
 *   44 refs 4 -> 5          10/43 = 2325 > 2040   (and still < 2500)
 *   86 refs 5 -> 4           8/49 = 1632 < 1860
 *   86 length 49 -> >= 54   10/54 = 1851 < 1860
 *   44 length 43 -> <= 39    8/39 = 2051 > 2040
 * 86 carries FIVE refs because it is SET TWICE: `a + 0x62` is computed in the
 * block that reads it and gcse inserts a second computation in the block that
 * reaches the stores without passing the read (the `a[0x5b] != 1` path skips
 * it).  THE ROM HAS THAT SECOND COMPUTATION TOO (ref 30-32
 * `movs r3,#0x62 / adds r3,r3,r5 / mov r9,r3`), so any spelling that removes it
 * is wrong even when the numbers improve -- which is exactly what naming the
 * pointer at the top does, and it is why those rows read -2 instructions below.
 *
 * AND THERE IS A THIRD, CONSISTENT SOLUTION that needs no change to d at all:
 * push BOTH 56 and 57 to live_length >= 15 (3/15 = 2000 < 2127) while taking 86
 * below 44.  Order then reads 32, 35, 56, 57, 44, 86, 33 -- the ROM exactly.
 * live_length 14 is NOT enough (3/14 = 2142 > 2127), so the window is >= 15,
 * and 86 must land under 1860.  No spelling tried here moved the pointers off
 * 3/12; they are defined and dead within one basic block.
 *
 * MEASURED THIS BATCH, with the allocno table for each (ndiff / instr gap /
 * aligned), all against the parked body unless noted:
 *   x1 anchor in the win arm            55 / +0 / 38   <-- THIS BODY
 *   the same anchor in the lose arm     65 / +2 / 39   exactly inert
 *   both arms anchored                  55 / +0 / 38   = x1, no extra value
 *   win AND lose store order reversed   64 / +2 / 37   moves 86 to 5/40=2500
 *                                                      and 44 to 4/52=1538 --
 *                                                      the store order IS the
 *                                                      lever on 44/86, but it
 *                                                      runs the WRONG WAY: the
 *                                                      byte stored FIRST gets
 *                                                      the SHORTER range.
 *   lose order reversed only            65 / +2 / 38
 *   win order reversed only            64 / +2 / 38
 *   both stores before the call         75 / +4 / 44   d 5/44=2272, still < 2500
 *   `a + 0x62` named at the top         97 / -2 / 50   d 5/46, 86 -> 4/52=1538
 *                                                      (the 44/86 swap DOES
 *                                                      happen) but it deletes
 *                                                      the gcse recomputation
 *   `a + 0x5b` named at the top         63 / +0 / 43
 *   both named at the top               94 / +0 / 48
 *   `a + 0x62` named after guard 1      55 / +0 / 38   bit-identical to x1
 *   pointers named at the top           78 / -2 / 43   (the park's recorded
 *                                                      prerequisite: it DOES
 *                                                      put d in r6, confirmed)
 *   pointers named before guard 1       65 / +2 / 39   pointers still 3/12
 *   `c` copied to a local               65 / +2 / 39   exactly inert
 *   first guard as nested ifs           65 / +2 / 39   exactly inert
 *   `d` copied to a local for the
 *     stores only                       65 / +2 / 39   exactly inert
 *   `d` copied to a local for the
 *     tests only                        65 / +2 / 39   exactly inert
 *   `r` removed, two returns            98 / +0 / 62   closes the gap and is
 *                                                      much worse; it also
 *                                                      moves `a` to r6, so the
 *                                                      ROM's stack slot for r
 *                                                      is load-bearing
 * FLAG AXIS on the PARKED body (autosweep, 8 of 8 groups): -fno-strict-aliasing,
 *   -fno-strength-reduce, -fno-rerun-loop-opt all 65 (inert); -fno-schedule-insns2
 *   66; -fno-rerun-cse-after-loop 72; -ffixed-r7 87; -fno-gcse 99; -O1 105.
 * FLAG AXIS RE-CROSSED ON THIS BODY, because a flag inert on the parked body is
 *   not a closed axis: -fno-strict-aliasing, -fno-rerun-loop-opt,
 *   -fno-strength-reduce all 55 (inert); -fno-rerun-cse-after-loop 62;
 *   -fno-schedule-insns2 66; -ffixed-r7 85 at +4; -fno-gcse 99 at -2; -O1 105 at
 *   -14.  Nothing crosses.
 * ANCHOR AXIS: closed, 21 of 21 boundaries, by autosweep.
 *
 * NEXT, in priority order: (1) one more reference to `d` that costs no
 * instruction -- note `r = d;` in place of `r = 0;` IS semantics-preserving
 * here, because `lose:` is reachable only through two `d == 0` tests so d is 0
 * at every point that reads the initial r, but it replaces the ROM's
 * `movs r2,#0` with a register copy and so loses an instruction; something of
 * that shape that does not. (2) anything that takes 56 and 57 to live_length
 * 15 while putting 86 under 1860.  (3) `d` live_length 47 -> 39.
 */
extern int OvlFunc_883_200d610(int *p, int *q);
extern int __atan2(int y, int x);
extern void __Actor_SetAnim(unsigned char *a, int n);

int OvlFunc_883_200d64c(unsigned char *a, unsigned char *b, int c, int d)
{
    int r;
    int t;
    int m;
    int h;
    int v0;
    int v1;
    int v2;

    r = 0;
    if (a[0x5b] == 1 && a[0x62] == 0) {
        __Actor_SetAnim(a, 1);
        return 1;
    }
    if (OvlFunc_883_200d610((int *)(b + 8), (int *)(a + 8)) >= c && d == 0)
        goto lose;
    t = (unsigned short)__atan2(*(int *)(b + 0x10) - *(int *)(a + 0x10),
                                *(int *)(b + 8) - *(int *)(a + 8));
    m = 0xf0 << 8;
    v2 = (t + 0xfffff000) & m;
    v1 = (t + (0x80 << 5)) & m;
    v0 = t & m;
    h = m & *(unsigned short *)(a + 6);
    if (v0 != h && v1 != h && v2 != h && d == 0)
        goto lose;
    a[0x5b] = 1;
    __Actor_SetAnim(a, 1);
    r = 1;
    do { } while (0);
    a[0x62] = 1;
    goto done;
lose:
    a[0x5b] = d;
    __Actor_SetAnim(a, 2);
    a[0x62] = d;
done:
    return r;
}
