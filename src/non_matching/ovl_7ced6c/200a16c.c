/* OvlFunc_946_200a16c -- NON-MATCHING, 13 differing STREAM encodings of 65.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Na16c_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 65 / ours 65 stream encodings; 56/56 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ced6c/200a16c.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_a_c.s --func OvlFunc_946_200a16c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_946_2009774(int a, int b, int c);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_946_200a16c(void)
{
    int b;
    int c;
    int d;

    b = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 0x10) >> 20;
    __MapActor_GetActor(0xf);
    if (b == 0x19) {
        OvlFunc_946_2009774(0xd, 0x60, 0);
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x1f) {
        OvlFunc_946_2009774(0xd, 0x50, 0);
    } else if (b == 0x22) {
        OvlFunc_946_2009774(0xd, 0x20, 0);
    } else if (b == 0x23) {
        OvlFunc_946_2009774(0xd, 0x10, 0);
    } else if (b == 0x24) {
        return;
    }
    __WaitFrames(2);
    d = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    c -= 1;
    __Func_8010704(b, c, 1, 3, d, c);
    __Func_8010704(0, 0, 1, 3, b, c);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_946_200a16c -- 0x0200a16c,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_a.s
 *
 * 72 of 72 lines, THIRTEEN differing, and every one of the thirteen is the same
 * two values in swapped registers.  Candidate at scratch/Na16c_best.c.
 *
 *      rom   asr r6, r3, #0x14 ... asr r5, r3, #0x14 ... cmp r6, #0x19
 *      ours  asr r5, r3, #0x14 ... asr r6, r3, #0x14 ... cmp r5, #0x19
 *
 * Identical residue to src/non_matching/ovl_7ced6c/2009c84.c, two functions
 * over in the same overlay: two `>> 20` values live to the end of the function,
 * similar reference counts, and which of them wins r5 is a near tie that gcc
 * breaks the other way.  Thirteen spellings are now screened across the two --
 * all six declaration orders, the reused local split, the decrement in three
 * positions and two forms, the stack arguments named at one site and at both,
 * and here the fetches written through a named pointer instead of inlined.
 * None of them moves it.
 *
 * WORTH NOTING, because it rules out the obvious rule: the two functions
 * disagree about WHICH value the ROM favours.  On 2009c84 the ROM puts the
 * FIRST-computed value in r5; here it puts the first-computed in r6.  In both
 * cases we produce the opposite of the ROM.  So there is no "first one gets r5"
 * to encode -- it is the allocator's density ranking, and the source has no
 * handle on it.
 *
 * NOT re-spent here.  This is the second data point for the same coin flip and
 * a clean two-pseudo test case for the REG_ALLOC_ORDER hypothesis that
 * docs/elevation.md says needs a rebuilt compiler to settle.  Retry both
 * together when it is.
 *
 * Everything else screens exact, including the shared call block at .L21ba that
 * four arms branch into -- which is NOT the shared-call-tail class of
 * src/non_matching/ovl_common/4cc.c: here gcc reproduces the shared call and the
 * line count matches, because the arms set two argument registers rather than
 * one value gcc can hoist above the compare.
 *)

*/
#endif
