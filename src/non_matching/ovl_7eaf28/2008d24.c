/* OvlFunc_960_2008d24 -- NON-MATCHING, 9 differing STREAM encodings of 69.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/n8d24.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 69 / ours 69 stream encodings; 56/56 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7eaf28/2008d24.c \
 *     asm/overlays/rom_7eaf28/ovl_314_c_c_a_c.s --func OvlFunc_960_2008d24
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern int _AREA_a5;
extern unsigned char gState[];
extern unsigned short L1a00[] __asm__(".L1a00");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_808edac(int a, int b, int c);
extern void __StopTask(void *f);
extern void OvlFunc_960_2008ce4(void);

void OvlFunc_960_2008d24(void)
{
    unsigned char *g;
    int p0, p1;
    int q0, q1;

    g = gState;
    if (*(short *)(g + (0xe0 << 1)) == (int)(&_AREA_a5)) {
        __MapActor_GetActor(0xe)[0x23] = 2;
        __MapActor_GetActor(0xe)[0x55] = 3;
        __MapActor_SetPos(0xe, 0, 0);
        p0 = 0xf;
        p1 = 0x2c;
        __Func_8010704(0x10, 0x2c, 1, 1, p0, p1);
        __Func_808edac(0x64, 0, 0);
        __Func_8010704(0xc, 0x47, 1, 1, 0x7f, 0x7f);
        q0 = 0xc;
        q1 = 0x47;
        __Func_8010704(0xb, 0x47, 1, 1, q0, q1);
        __StopTask(OvlFunc_960_2008ce4);
        *(unsigned short *)0x500019e = L1a00[0];
    }
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_960_2008d24 -- asm/overlays/rom_7eaf28/ovl_314_c_c_a.s
 *
 * BLOCKER: ARG INTERLEAVE on the LAST call + store ordering
 *
 * 8 of 65 differing, same length.  57 lines are exact.  Best C: scratch/n8d24.c.
 *
 * TWO THINGS THAT WORKED, both worth reusing:
 *
 *  1. The ROM POOLS 0xa5 (`ldr r3, =0xa5 / cmp r2, r3`) where a literal gives
 *     `cmp r2, #0xa5`.  0xa5 fits in a thumb cmp immediate, so the pool load is
 *     the symbol tell.  `_AREA_a5` is already in area.sym; spelling the test as
 *     `== (int)(&_AREA_a5)` took the screen from 62 differing to 17.
 *  2. Two of the four __Func_8010704 sites pass DIFFERENT values in the two
 *     stack slots, and the ROM materialises BOTH before storing either
 *     (`mov r3,#0xf / mov r2,#0x2c / str r3,[sp] / str r2,[sp,#4]`).  A pair of
 *     named locals per site -- not shared between sites -- took 17 to 8.  The
 *     third site passes 0x7f in both slots and matches with plain literals,
 *     which is the control that shows the lever is about the values differing.
 *
 * WHAT IS LEFT (8 lines, two spots):
 *
 *   call 4, __Func_8010704(0xb, 0x47, 1, 1, 0xc, 0x47):
 *     rom  mov r1,#0x47 / mov r2,#1 / mov r3,#1 / mov r0,#0xb
 *     ours mov r3,#1 / mov r2,#1 / mov r0,#0xb / mov r1,#0x47
 *   the tail store:
 *     rom  ldrh r2,[r3] / ldr r3,=0x500019e / strh r2,[r3]
 *     ours ldr r2,=0x500019e / ldrh r3,[r3] / strh r3,[r2]
 *   and the consequent pop {r1}/bx r1 vs pop {r0}/bx r0.
 *
 * Call 3 -- the same callee, same shape, plain literals -- matches exactly, so
 * this is specific to call 4 and not to the callee or the argument count.
 *
 * MEASURED (all 65 lines unless noted):
 *   named pair per stack-arg site                              8   <- best
 *   ... + volatile on the 0x500019e store                      8
 *   ... + arg2 spelled as q1 (sharing the stack arg's local)   8
 *   ... + unsigned short temp for L1a00[0]                     8   (66 lines, worse)
 *   ... + 0xb assigned in the dominating entry block           8   (66 lines)
 *   ... + 0x47 assigned in the dominating entry block         12
 *   ... + q0/q1 hoisted to the dominating entry block         12
 *   ... + the __Func_8010704 prototype removed entirely       14  (worse)
 *
 * The basic-block lever IS available here -- the entry block dominates the
 * whole if-body -- and it makes things WORSE at both positions tried.  That is
 * a useful negative: it separates this from the straight-line arg-interleave
 * parks (2009df8, 20087dc) where the lever is merely unreachable.
 *)

*/
#endif
