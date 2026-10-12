/* OvlFunc_952_200bd40 -- NON-MATCHING, 2 differing STREAM encodings of 72.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Lbd40_A.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 72 / ours 72 stream encodings; 52/52 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/200bd40.c \
 *     asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c_c_c_c_c.s --func OvlFunc_952_200bd40
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char *iwram_3001ebc;
extern void __PlaySound(int id);
extern void __Func_80118a8(int n);
extern void __Func_80118c0(int n);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8092208(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_952_200bd40(void)
{
    unsigned char *base;
    int m;
    int v;

    m = -0x10;
    base = iwram_3001ebc;
    __CutsceneStart();
    __PlaySound(0x9e);
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(0, 2);
    v = *(short *)(base + (0xb6 << 1));
    if (v == 0x20) {
        __Func_80118a8(1);
        __CutsceneWait(0xa);
        __Func_809228c(0, 0, m);
    } else if (v == 0x1e) {
        __Func_80118a8(4);
        __CutsceneWait(0xa);
        __Func_8092208(0, 3, m);
    } else {
        __Func_80118a8(2);
        __CutsceneWait(0xa);
        __Func_8092208(0, 3, m);
    }
    __CutsceneWait(0x10);
    __Func_8091e9c(*(short *)(base + (0xb6 << 1)));
    __Func_80118c0(1);
    __Func_80118c0(2);
    __Func_80118c0(4);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_952_200bd40 -- 0x0200bd40,
 * asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *
 * 74 of 74 lines, TWO differing.  Candidate at scratch/Lbd40_A.c.
 * Both are the `mov r0, #0` position at the one __MapActor_SetSpeed call.
 *
 * The batch-127 argument-order lever WORKS in this function -- but only at the
 * `neg` sites.  Naming `m = -0x10;` at the top fixed all three
 * __Func_809228c/__Func_8092208 call sites, which sit after the function's
 * three-way branch.  The SetSpeed call does not move, and the reason sharpens
 * the rule:
 *
 *   THE BRANCH MUST BE BEFORE THE SITE.  This function's SetSpeed is the third
 *   instruction group in the function, before any conditional branch at all.
 *   Naming its two shifted constants at the top then makes gcc keep them in
 *   CALLEE-SAVED registers across the two intervening calls -- ours grew to
 *   `push {r5, r6, r7, r14}` against the ROM's `push {r5, r14}` -- which is the
 *   same failure the three straight-line parks show, for the same reason.
 *   Intervening CALLS are not enough; gcc will happily save and restore across
 *   them. It is the branch that makes rematerialising the cheaper choice.
 *
 * MY OWN CLASSIFICATION WAS TOO COARSE.  The batch-127 sizing counted a
 * function as "guarded" if ANY conditional branch preceded ANY site with the
 * shape.  This function has both kinds at once: guarded `neg` sites and an
 * unguarded `lsl` site.  The split is per-SITE, not per-function, and the
 * 150/98 figures should be read as an upper bound on the workable side.
 *
 * ALSO TRIED: naming only one of the two shifted constants (75 lines, worse in
 * both directions); leaving all three inline (11 differing -- the `neg` sites
 * regress, confirming the lever is doing real work there).
 *)

*/
#endif
