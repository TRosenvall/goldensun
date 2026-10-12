/* OvlFunc_954_200804c -- NON-MATCHING, 40 differing STREAM encodings of 104.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/N804c_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 104 / ours 104 stream encodings; 92/92 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7db0c8/200804c.c \
 *     asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_a.s --func OvlFunc_954_200804c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern int L441c __asm__(".L441c");

extern void __Func_8010788(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __MapActor_SetAnim(int a, int b);

void OvlFunc_954_200804c(void)
{
    int a;
    int b;

    switch ((unsigned int)L441c) {
    case 6:
    case 0x42:
        b = 0x26;
        __Func_8010788(0x5c, 0x1f, 2, 2, 0x32, b);
        __Func_8010788(0x5c, 0x1f, 2, 2, 0x36, b);
        __MapActor_SetAnim(0x10, 0xa);
        break;
    case 0x3c:
        a = 0x32;
        b = 0x26;
        __Func_8010788(0x5c, 0x21, 2, 2, a, b);
        __Func_8010788(0x5c, 0x21, 2, 2, 0x36, b);
        __Func_8010704(0x32, 0x19, 6, 1, a, 0xc);
        __MapActor_SetAnim(0x10, 0xb);
        break;
    case 0:
        a = 0x32;
        b = 0x26;
        __Func_8010788(0x5c, 0x1d, 2, 2, a, b);
        __Func_8010788(0x5c, 0x1d, 2, 2, 0x36, b);
        __MapActor_SetAnim(0x10, 0xc);
        __Func_8010704(0x32, 0x18, 6, 1, a, 0xc);
        L441c = 0x78;
        break;
    }
    L441c = L441c - 1;
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_954_200804c -- 0x0200804c,
 * asm/overlays/rom_7db0c8/ovl_30_c_c_a_a_a_a.s (SINGLE-function file)
 *
 * 108 of 108 lines, 41 differing, and the whole residue is that TWO CASE ARMS
 * ARE LAID OUT IN THE OPPOSITE ORDER.  Candidate at scratch/N804c_best.c.
 *
 * SOLVED: the dispatch itself.  Four cases {0, 6, 0x3c, 0x42} with 6 and 0x42
 * sharing a body, and gcc's sorted balanced tree -- `cmp #6 / beq / cmp #6 /
 * bhi / cmp #0 / beq` and a right subtree for 0x3c and 0x42 -- reproduces
 * exactly.  `bhi` says the switch value is UNSIGNED, so the read of the global
 * is cast.  The two named locals follow the batch-149 rule: `a = 0x32` is named
 * in the two arms where the ROM holds it in r6 across two calls and written as
 * a literal in the arm where it is materialised once and stored at once.
 *
 * BLOCKER: THE TREE SENSE AND THE ARM ORDER ARE COUPLED, and the ROM wants one
 * of each.
 *
 *   source order 0x3c, 6/0x42, 0   -- arms laid out the ROM's way, but the tree
 *                                     ends `beq case; b default` where the ROM
 *                                     has `bne default; b case`.  109 lines, 43.
 *   source order 6/0x42, 0x3c, 0   -- tree EXACT, and the two arms swap.
 *                                     108 lines, 41.   <- best
 *
 * There is no third arrangement.  SCREENED: `case 0x42:` before `case 6:` (no
 * change); an explicit `default: break;` on both orderings (no change); the
 * shared body duplicated into two separate arms so the tree sees two leaves and
 * cross-jumping merges them (109 lines, 69 differing -- it does not merge); and
 * a switch whose arms are nothing but `goto`s to labels placed in the ROM's
 * order, which is the batch-148 block-order lever and fails here because gcc
 * folds the gotos back into the switch (109, 43).
 *
 * So this is a layout decision inside gcc's own case expansion, and the source
 * has exactly one bit of influence over it -- the order the arms are written --
 * which is already spent buying the correct tree.
 *)

*/
#endif
