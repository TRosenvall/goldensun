/* Func_801f77c (0x0801f77c) -- MATCH.
 *
 * objcmp: OK Func_801f77c -- 156 bytes, 62 encodings and 8 relocations
 * identical, against BOTH asm/rom_15000/rom_1de5c_c_c_c_c_a_a_c_a_c_a.s and a
 * oneref.py cut of it.  Default flag group (-O2 GCC296_CFLAGS); no per-file
 * rule fires for this stem, and every flag group measured WORSE (below).
 *
 * SHAPE IS NOT DIAGNOSIS.  The scan flagged this on "zero interleaved into a
 * shifted build" at one straight-line site:
 *
 *     ldr r3, =gKeyHeld / mov r2, #0x90 / ldr r3, [r3] / b .L / .L: lsl r2, #1
 *
 * That site was never an argument-order problem -- this function makes no call
 * that takes an argument.  The single-instruction insn sitting inside the
 * mov+lsl build is a GLOBAL LOAD, and its position is decided by where gcc
 * dumps the literal pool, not by anything the source can say directly (see
 * "the pool position is downstream" below).  It fell out on its own the moment
 * the preheader was right.
 *
 * There IS a real interleave in this function, and it is the OTHER kind the
 * "zero interleaved" section already records -- a mov+NEG split build:
 *
 *     bl Func_80056cc / mov r6, #0x9 / mov r5, #0x0 / neg r6, r6
 *
 * `n = 0;` lands inside the build of `-9`.  It needed NO pin, NO flag and NO
 * barrier: writing the two initialisers in the order `n = 0; r = -9;` is the
 * whole lever.  Writing `r = -9; n = 0;` puts the `neg` before the `mov r5`
 * (v1, 14 differing).
 *
 * THE ACTUAL BLOCKER WAS THE PREHEADER, and the lever is new (see the
 * write-up at the end).  The ROM's pre-loop block is
 *
 *     ldr r3,=iwram / ldr r1,[r3] / ldr r3,=2002010 / ldr r2,=200200c
 *     strh r5,[r3] / mov r7,r3 / ldr r3,=0x1070 / strh r5,[r2]
 *     ldr r4,=1 / mov r6,r0 / add r1,r3 / mov r0,#2
 *
 * The two things a plainly-written source cannot reach are (a) `mov r7, r3`
 * EARLY, which is what frees r3 so that (b) the 0x1070 constant lands in r3
 * and the `add` comes LAST.  Written with a walking pointer (`p += 0x1070;`
 * before the loop, `p[1]`/`p[2]`, `p += 0x40;`) the copy is a loop-invariant
 * hoist and hoists land at the END of the preheader, so r3 stays live to the
 * end of the block, 0x1070 is allocated r4, and the add floats to the front:
 * 11 differing, and no statement permutation of the block reaches it (the full
 * 60-way sweep below tops out at 11 for the semantically-correct store order).
 *
 * THE FIX: make the +0x1070 a STRENGTH-REDUCED INDUCTION INIT rather than a
 * source statement.  Walk an INT INDEX, not the pointer:
 *
 *     k = 0x1070;  for (i = 2; i >= 0; i--) { p[k+1]; p[k+2]; k += 0x40; }
 *
 * gcc's strength_reduce then creates the cursor itself, and the giv's
 * initialisation (`ldr r3,=0x1070 / add r1,r3`) is emitted at the preheader
 * end AFTER the loop-invariant hoists.  The copy `mov r7,r3` is now the one
 * that comes first, r3 dies there, the constant gets r3, and sched2 does the
 * rest.  Exact on the first screen in all three spellings tried (`p[k+1]`,
 * `q = p + k; q[1]`, `q = &p[k]; q[1]`).
 *
 * THE POOL POSITION IS DOWNSTREAM, and it is worth knowing because it looks
 * like an independent defect.  gcc chooses the mid-function pool site in
 * create_fix_barrier (arm.c): it walks forward from the last fix that fitted
 * and keeps the LAST insn with `count < max_count`, where max_count comes from
 * minipool_vector_head->max_address -- the head being the HImode entry (the
 * `1` stored through the u16), which has the tightest range.  Move the
 * `ldrh r4, =1` two bytes and the pool, the `b` over it, and the alignment pad
 * all move with it.  Ours dumped the pool one insn early, dropped the ROM's
 * two-byte `.align 2, 0` pad, and came out 2 bytes SHORT -- a size defect that
 * three "differing lines" understate.  Do not chase it; fix the block.
 *
 * MEASURED WORSE (all against the same 65-line reference)
 *   spelling                                                      differing
 *   r = -9; n = 0;                                                  14
 *   n = 0; r = -9;  + p += 0x1070 as a statement                    13
 *     ... + call result through temp `u`, `r = u;` last (best walk)   11
 *   p = iwram + 0x1070 as one expression                            17
 *   `u` assigned to r at any of the 5 slots x 6 stmt orders     10..56 (60-way)
 *   named `u16 *` locals for the two ewram globals            64 lines, 54
 *       -- the `mov r7, r3` copy DISAPPEARS; wrong direction
 *   p[0x1071] / p[0x1072] with a walking POINTER              69 lines, 60
 *       -- gcc builds TWO cursors, one per offset
 *   ewram = n; instead of ewram = 0;                                11 (no change)
 *   `short` instead of `unsigned short` globals                     11 (no change)
 *   declaration-order permutations (4)                              11 (no change)
 *   unsigned-int address arithmetic + cast (4 spellings)         11..13
 *   -fno-schedule-insns2                                            19
 *   -fno-rerun-cse-after-loop                                 66 lines, 55
 *   -fno-gcse / -fno-strength-reduce / -fno-rerun-loop-opt /
 *       -fno-strict-aliasing                                        11 (inert)
 *
 * A FALSE MINIMUM worth recording: swapping the two pre-loop zero stores in
 * the source (200200c first) scores 7 -- better than the correct order's 11 --
 * but its two `strh`s come out in the ROM's REVERSE order, so the 7 can never
 * go to 0.  Rank on WHICH lines differ, not how many.
 */
extern int Func_80056cc(void);
extern int Func_8005c68(void);
extern void Func_8005cf8(void);
extern signed char *iwram_3001f1c;
extern unsigned short ewram_2002010;
extern unsigned short ewram_200200c;
extern unsigned int gKeyHeld;

int Func_801f77c(void)
{
    int r;
    int t;
    int n;
    int u;
    signed char *p;
    int k;
    int i;

    t = Func_80056cc();
    n = 0;
    r = -9;
    if (t == 0) {
        u = Func_8005c68();
        p = iwram_3001f1c;
        ewram_2002010 = 0;
        ewram_200200c = 0;
        r = u;
        k = 0x1070;
        for (i = 2; i >= 0; i--) {
            if (p[k + 1] != 0) {
                ewram_2002010 = 1;
                n++;
            }
            if (p[k + 2] != 0) {
                ewram_200200c = 1;
            }
            k += 0x40;
        }
        if ((gKeyHeld & 0x120) != 0x120) {
            ewram_2002010 = 0;
        }
    }
    Func_8005cf8();
    if (r != 0 && n == r) {
        return r + 0x64;
    }
    return r;
}
