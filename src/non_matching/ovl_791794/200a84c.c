/* OvlFunc_897_200a84c -- NON-MATCHING, 16 differing STREAM encodings of 64.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/za84c.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 64 / ours 64 stream encodings; 55/55 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_791794/200a84c.c \
 *     asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s --func OvlFunc_897_200a84c
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_800fe9c(void);

void OvlFunc_897_200a84c(int f)
{
    int one;
    int two;

    if (f != 0) {
        one = 1;
        __CopyMapTiles(8, 0x2f, 0x40, 7, one, one);
        two = 2;
        __CopyMapTiles(7, 0x30, 0x3f, 8, two, one);
        __CopyMapTiles(7, 0x31, 0x3f, 9, two, one);
    } else {
        one = 1;
        __CopyMapTiles(0x38, 0, 0x40, 7, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 8, one, one);
        __CopyMapTiles(0x38, 0, 0x3f, 9, 2, one);
        __CopyMapTiles(0x3a, 0x19, 0x40, 8, one, one);
    }
    __Func_800fe9c();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_897_200a84c -- asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION -- a clean r5/r6 swap, 16 of 65, same length
 *
 * Two arms of __CopyMapTiles calls.  Every instruction is right; the two
 * carried constants land in the opposite registers:
 *
 *     rom   one (1) -> r5,  two (2) -> r6
 *     ours  one (1) -> r6,  two (2) -> r5
 *
 * `one` is used seven times across BOTH arms, `two` twice in the then-arm
 * only, and the ROM gives the more-used value the lower register.  gcc does
 * the reverse.  Note the else-arm's single `2` is a plain literal in the ROM
 * (`mov r3, #2 / str r3, [sp]`), not the carried r6 -- so the source really
 * does name it in one arm and not the other, and that is reproduced.
 *
 * MEASURED (all 65 lines, all 16 differing at position 4):
 *   int one; int two;  (both function-scope)              16
 *   int two; int one;  (declarations swapped)             16
 *   `two` block-scoped inside the then-arm                16
 *
 * The declaration-order lever is inert here, as it is on most functions --
 * see the CONFLICT note in docs/elevation.md.  Nothing in the source reaches
 * the allocator's priority order for two values with these live ranges.
 *
 * Best C: scratch/za84c.c.
 *)

*/
#endif
