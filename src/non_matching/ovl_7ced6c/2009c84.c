/* OvlFunc_946_2009c84 -- NON-MATCHING, 11 differing STREAM encodings of 72.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/N9c84_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 72 / ours 72 stream encodings; 60/60 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7ced6c/2009c84.c \
 *     asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s --func OvlFunc_946_2009c84
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

void OvlFunc_946_2009c84(void)
{
    int a;
    int b;
    int c;

    a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;
    b = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    if (b == 7) {
        if (c == 0x18) {
            OvlFunc_946_2009774(8, 0, 0x30);
        } else {
            OvlFunc_946_2009774(8, 0, 0x50);
            OvlFunc_946_2009774(8, 0, 0x70);
        }
    } else if (b == 0xa) {
        if (c == 0x18)
            return;
        OvlFunc_946_2009774(8, 0, 0x90);
    } else if (b == 0xe) {
        OvlFunc_946_2009774(8, 0, 0x50);
    } else {
        return;
    }
    __WaitFrames(2);
    c = *(int *)(__MapActor_GetActor(8) + 0x10) >> 20;
    a -= 1;
    __Func_8010704(a, b, 3, 1, a, c);
    __Func_8010704(0, 0, 3, 1, a, b);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_946_2009c84 -- 0x02009c84,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s
 *
 * 77 lines against 77, and every one of the ELEVEN differing lines is the SAME
 * TWO REGISTERS SWAPPED.  Candidate at scratch/N9c84_best.c.
 *
 *      rom   asr r5, r3, #0x14 ... asr r6, r3, #0x14 ... cmp r6, #0x7
 *      ours  asr r6, r3, #0x14 ... asr r5, r3, #0x14 ... cmp r5, #0x7
 *
 * Two values are shifted out of actor fields at the top and both live to the
 * end.  The ROM puts the FIRST in r5 and the second in r6; we do the reverse.
 * Instruction count, instruction order and every operand other than those two
 * register numbers are exact.
 *
 * SOLVED ON THE WAY, and worth keeping: INLINING THE __MapActor_GetActor CALLS
 * took it from 16 differing to 11.  Written as `e = __MapActor_GetActor(8);
 * a = *(int *)(e + 8) >> 20;` the pointer becomes a fourth pseudo competing for
 * the allocator's attention and the third shifted value moves out of a scratch
 * register into r0 with the three-operand `asr`.  Written as
 * `a = *(int *)(__MapActor_GetActor(8) + 8) >> 20;` there is no pointer
 * variable at all and the two-operand `asr r3, #0x14` comes back.  A named
 * pointer that is dead by the next statement is not free.
 *
 * BLOCKER: ALLOCATION PRIORITY.  gcc's global allocator ranks by references per
 * unit of live range and hands out registers in REG_ALLOC_ORDER; with
 * -fcall-used-r4 the first callee-saved slot is r5, so whichever of the two
 * wins gets it.  Both have the same live range here -- computed at the top,
 * used at the last call -- and similar reference counts, so the two orderings
 * are a near tie that gcc breaks the other way.
 *
 * SCREENED AND UNCHANGED AT 11: all six declaration orders of the three locals
 * (tried on both the pointer and the inlined baseline); the reused third local
 * split into two so each use is short-lived; the decrement written `--a`,
 * `a = a - 1`, and moved to three different statement positions; and both stack
 * arguments named at one call site and then at both, which is the batch-148
 * six-argument lever and adds pseudos without changing the ranking.
 *
 * This is the class docs/elevation.md sizes under REG_ALLOC_ORDER: the note
 * there says the testable next step is rebuilding gcc-2.96 with the order
 * starting at r4 and re-screening. This function is a clean, small member to
 * test that on -- two pseudos, one swap, nothing else wrong.
 *)

*/
#endif
