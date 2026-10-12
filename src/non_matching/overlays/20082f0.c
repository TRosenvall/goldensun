/* OvlFunc_949_20082f0 -- NOT MATCHING, a LENGTH gap: ref 68 instructions against ours 66 (-2).
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE and had no
 * figure; its own header named scratch/L82f0.c, that file still
 * existed and it compiles.  70 of 133 prose parks were in that state --
 * work done, measured, and left in a GITIGNORED directory instead of installed.
 *
 * THE LENGTH-GAP CRITERION APPLIES, and it is this tree's most productive
 * targeting instrument -- four batches run on it closed 8 of 15, 7 of 25,
 * 8 of 25 and 10 of 32, and LOCATED the missing-or-extra instruction on
 * essentially every target.  Our side is 2 instruction(s) SHORTER, so the
 * search is which statement is missing -- see docs/elevation.md, 'OUR STREAM BEING
 * SHORTER IS A SIGNATURE'.
 * No encoding figure exists until the lengths agree.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20082f0.c \
 *     asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s --func OvlFunc_949_20082f0
 *
 * The original prose park is preserved below.
 */
extern char *iwram_3001ebc;

extern int __GetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __WaitFrames(int n);
extern char *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_949_20082f0(void)
{
    char *q;
    char *p;
    unsigned short *f;
    int saved;
    int id;

    q = iwram_3001ebc;
    p = __MapActor_GetActor(0x10);
    saved = *(short *)(p + 6);
    f = (unsigned short *)(p + 0x64);
    __CutsceneStart();
    *f |= 2;
    if (*(short *)(q + (0xbf << 1)) == 0) {
        if (__GetFlag(0x95 << 4))
            id = 0x2365;
        else if (__GetFlag(0x962))
            id = 0x21e2;
        else
            id = 0x1f95;
    } else {
        if (__GetFlag(0x95 << 4))
            id = 0x2371;
        else if (__GetFlag(0x962))
            id = 0x21f5;
        else
            id = 0x1faa;
    }
    __MessageID(id);
    __MapActor_SetAnim(0x10, 0);
    __Func_8092848(0x10, 0, 2);
    __Func_8093040(0x10, 0, 0xa);
    *(short *)(p + 6) = saved;
    __WaitFrames(1);
    *f &= 1;
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_949_20082f0 -- 0x020082f0, asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s
 * Twin of OvlFunc_949_20083d0 -- 0x020083d0, SAME .s file.
 *
 * A clean twin family (neither member previously parked).  They differ only in
 * constants -- 0x10 against 0x11 and six message ids each one higher -- so one
 * solution elevates both.  Candidate: scratch/L82f0.c.
 *
 * THIS TU IS ALMOST CERTAINLY -O1, AND THAT IS THE FINDING.
 *
 *      default (-O2)   81 lines against the ROM's 86, 62 differing
 *      --O1            86 against 86, FIFTEEN differing
 *
 * The length agreeing only at O1 is the signal: the ROM's five extra
 * instructions are work that -O2 optimises away here, not something a source
 * spelling can add back.
 *
 * CORRECTION.  An earlier version of this note claimed the Makefile already
 * carries an O1 rule for a sibling in this directory.  It does not -- the one
 * explicit rule for rom_7d4af4 (ovl_30_c_c_a_c_c_c_c_c_c_b) uses CSE_CFLAGS.
 * That claim was asserted from memory and is withdrawn.
 *
 * What the Makefile actually shows, measured: O1 is applied at OVERLAY scale,
 * not per file -- 720 O1 rules live in just 13 directories, in blocks of 30 to
 * 180.  Ten of those 13 directories also carry a handful of single-file
 * exceptions in another group, so per-file deviation is normal.  But rom_7d4af4
 * is not an O1 directory at all; its only rule is CSE.  A lone O1 file here
 * would run against the grain, so the line-count evidence below has to carry
 * the hypothesis ON ITS OWN rather than being corroborated by neighbours.
 *
 * WHY -O2 IS FIVE SHORT, since it explains the shape rather than just the count:
 * the six message-id arms each end by assigning one id, and the ROM loads each
 * id straight into r0 and branches to a single `bl __MessageID`.  At -O2 gcc
 * hoists the pool load ABOVE the `cmp r0, #0` that tests the __GetFlag result,
 * so r0 is still live and the merged value has to live in r3 instead, changing
 * the branch structure.  At -O1 the hoist does not happen.
 *
 * CONFIRMED ON THE WAY (worth keeping even though the function is parked):
 * at -O2 the compound `*f |= 2;` reproduces the ROM's POOLED constant
 * (`ldr r3, =0x2`) exactly as docs/elevation.md's HImode-literal rule predicts.
 * At -O1 the same source gives `mov r2, #0x2`, so that rule is -O2-specific --
 * which the doc does not currently say.
 *
 * REMAINING AT -O1, 15 differing: adjacency swaps in the prologue (the iwram
 * load against the GetActor argument, and `mov r8, r2` against the r6 setup)
 * and the register roles on the halfword OR.  TRIED, all 16 -- i.e. slightly
 * worse: swapping the source order of the `saved` and `f` assignments,
 * `*f = 2 | *f`, and an int intermediate for the OR.
 *
 * The OR role swap is the same signature as 200a5c0.c in this directory, which
 * was parked this round after seven spellings.  Do not re-spend a round on it.
 *
 * LATER: the -O1 hypothesis above is CONFIRMED as far as it goes -- 86 against
 * 86 with 15 differing, reproduced -- and at least one of the remaining 15 is
 * now identified and is NOT reachable from C.
 *
 * The first difference is argument precompute:
 *
 *     rom    ldr r3, =0x3001ebc / mov r0, #0x10 / ldr r5, [r3] / bl GetActor
 *     ours   ldr r3, =0x3001ebc / ldr r5, [r3] / mov r0, #0x10 / bl GetActor
 *
 * The cheap `mov r0, #0x10` is the call's argument; the memory load is
 * expensive work beside it. gcc emits the expensive work first and lets the
 * cheap constant land last, which is exactly HANDOFF.md's "Argument
 * precompute: DIAGNOSED" section -- calls.c:805, not fixable from C.
 *
 * Swapping the two source statements so the call comes first makes it WORSE,
 * 18 differing: gcc then does the whole call before touching iwram_3001ebc,
 * where the ROM reads it before the call. The 15-differing form above is the
 * better one and is kept.
 *
 * SO THE -O1 QUESTION IS NOT SETTLED BY CLOSING THIS FUNCTION, and it cannot
 * be. Some of the residue is a documented compiler difference that would
 * remain at any optimisation level. Adding an O1 rule for this TU on the
 * strength of a line count that still leaves 15 differing -- against the grain
 * of a directory whose only explicit rule is CSE -- is not justified, and none
 * was added. The line-count evidence stands where the note left it.
 *)

*/
#endif
