/* OvlFunc_960_2008dc8 -- NOT MATCHING, a LENGTH gap: ref 52 instructions against ours 51 (-1).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L8dc8.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 1 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2008dc8.c \
 *     asm/overlays/rom_7eaf28/ovl_314_c_c_a_c.s --func OvlFunc_960_2008dc8
 *
 * The original prose park is preserved below.
 */
extern unsigned int gState;
extern int _AREA_a5;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __Func_808ee0c(void);
extern void __StartTask(void *fn, int pri);
extern void OvlFunc_960_2008ce4(void);

void OvlFunc_960_2008dc8(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned int q;
    unsigned int off;
    int z;
    int x, y;
    int m1, m2;
    int e1, f1, e2, f2;
    int pri;

    x = 0xf8 << 16;
    y = 0xb2 << 18;
    m1 = -1;
    m2 = -1;
    e1 = 0xf;
    f1 = 0x2c;
    e2 = 0xc;
    f2 = 0x47;
    pri = 0xc8 << 4;
    q = (unsigned int)&gState;
    off = 0xe0;
    off <<= 1;
    q += off;
    if (*(short *)q == (int)(&_AREA_a5)) {
        a = __MapActor_GetActor(0xe);
        a[0x23] = 2;
        b = __MapActor_GetActor(0xe);
        z = 0;
        b[0x55] = z;
        __MapActor_SetPos(0xe, x, y);
        __Func_8010704(0x1f, 0x5f, 1, 1, e1, f1);
        __Func_808edac(0x64, m1, m2);
        __Func_808ee0c();
        __Func_8010704(0x7f, 0x7f, 1, 1, e2, f2);
        __StartTask(OvlFunc_960_2008ce4, pri);
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_960_2008dc8 -- 0x02008dc8, asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
 *
 * 60 vs 61 lines, 14 differing.  Candidate at scratch/L8dc8.c.
 *
 * SOLVED:
 *   - `_AREA_a5` for the area comparison (the ROM pool-loads 0xa5 where a `cmp`
 *     immediate would do; the symbol was already in area.sym).
 *   - TWO SEPARATE POINTER VARIABLES for the two __MapActor_GetActor results.
 *     Reusing one variable made gcc preserve the returned pointer with
 *     `mov r2, r0 / add r2, #0x23` where the ROM destroys it with
 *     `add r0, #0x23`.  50 differing -> 14 on that change alone.
 *   - Named locals for both stack-argument pairs and for the shifted position
 *     constants.
 *
 * BLOCKER: the ROM spends a CALLEE-SAVED register on a short-lived zero.
 *      rom   push {r5, r14} ... mov r5, #0x0 / ... / strb r5, [r0, #0x0]
 *      ours  push {r14}     ... mov r3, #0x0 / ... / strb r3, [r0, #0x0]
 * r5 is not live across any call there, so nothing requires it to be
 * callee-saved -- gcc simply had r0-r3 committed to the upcoming SetPos
 * arguments and reached past them.  Ours orders the strb and those argument
 * builds differently, leaves r3 free, and never needs the fifth register.
 *
 * The remaining differences are that missing push plus the two stack-argument
 * pairs, where the ROM materialises both values before storing either
 * (`mov r3,#0xf / mov r2,#0x2c / str r3,[sp] / str r2,[sp,#4]`) and ours reuses
 * one register (`str r3,[sp] / mov r3,#0x2c / str r3,[sp,#4]`).  Naming the
 * pairs as locals -- which fixed exactly this on OvlFunc_924_200b6ac -- does not
 * reach it here, because there the pair was the only pressure and here the
 * interleaved strb competes for the same registers.
 *
 * TRIED: hoisting `z = 0;` into the dominating block to make gcc spill it to a
 * callee-saved register (no change, 14); --no-rerun-cse (no change).
 *)

*/
#endif
