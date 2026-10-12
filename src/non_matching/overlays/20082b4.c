/* OvlFunc_911_20082b4 -- NON-MATCHING, 5 differing STREAM encodings of 34.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/L82b4.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 34 / ours 34 stream encodings; 28/28 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/20082b4.c \
 *     asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_a.s --func OvlFunc_911_20082b4
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char *iwram_3001ebc;
extern unsigned char *__MapActor_GetActor(int slot);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_809228c(int a, int b, int c);
extern void __Func_8091e9c(int n);

void OvlFunc_911_20082b4(int n)
{
    unsigned char *a;
    unsigned int q;

    a = __MapActor_GetActor(0);
    a[0x55] = 0;
    __MapActor_SetSpeed(0, 0x80 << 8, 0x80 << 7);
    __MapActor_SetAnim(0, 2);
    __Func_809228c(0, 0, -8);
    q = (unsigned int)iwram_3001ebc;
    q += 0xe4 << 1;
    *(int *)q = 0x10;
    __Func_8091e9c(n);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_911_20082b4 -- 0x020082b4,
 * asm/overlays/rom_79e5c0/ovl_30_c_a_a_c_a_a_a_c_a.s
 *
 * 33 of 33 lines, 5 differing.  Candidate at scratch/L82b4.c.
 *
 *      rom   lsl r2, #0x7 / mov r0, #0x0 / lsl r1, #0x8
 *      ours  lsl r2, #0x7 / lsl r1, #0x8 / mov r0, #0x0
 *
 * THIRD instance of the argument-setup-order class, after
 * src/non_matching/overlays/20099a4.c and 2008a80.c.  Everything else is exact.
 *
 * What the three together establish: gcc ALWAYS emits the `mov r0, #0` last in
 * the argument-setup block, and the ROM never does.  Across the three the ROM
 * places it in two different slots -- before both shifted builds in 20099a4,
 * between them in 2008a80 and here -- so there is no fixed convention to
 * imitate, and `--no-sched2` makes all three worse, so the ROM was built with
 * the second scheduling pass running.  The difference is in what that pass was
 * handed, which is the argument evaluation order, and nothing tried reaches it.
 *
 * TRIED across the three: naming the zero as a local; sharing one zero local
 * across the calls that take one; naming the shifted values as locals;
 * --no-sched2; -fno-schedule-insns; -fno-defer-pop; -fomit-frame-pointer.
 *
 * The class is now sized -- see docs/elevation.md.  It is worth detecting
 * BEFORE writing a candidate, because a function whose only defect is this is
 * otherwise indistinguishable from one that is nearly right for a reachable
 * reason.
 *
 * CORRECTION (added later, and it changes the class).  An earlier version of
 * this park said gcc ALWAYS emits the `mov r0, #0` last in an argument block.
 * That is wrong.  A control over the 3411 solved functions in this tree found
 * 15 that carry the exact interleaved shape, one of them the identical
 * `mov r1,#128 / mov r2,#128 / mov r0,#0 / lsl r1,#9 / lsl r2,#8 /
 * bl __MapActor_SetSpeed`.
 *
 * The construct is in src/overlays/rom_79c738/ovl_30_c_c_c_c_c_c_c_b.c: the
 * shifted constants are NAMED LOCALS assigned at the top of the function, and
 * -- this is the part that matters -- TWO EARLY RETURNS sit between the
 * assignments and the calls.  gcc will not keep seven constants live across the
 * guards, so it rematerialises each at its use, and the rematerialised sequence
 * interleaves the way the ROM's does.
 *
 * That is why the lever does not reach THIS function: it is straight-line.
 * Naming the constants at the top with no branch in between makes gcc keep them
 * live instead, which is worse, not better (33 -> 39 lines on
 * OvlFunc_911_20082b4, 26 -> 29 on OvlFunc_899_20099a4).
 *
 * So the blocker here is not "gcc cannot produce this order" but "this function
 * has no dominating block to produce it from".  Sized in docs/elevation.md:
 * 248 remaining functions carry the shape, 150 of them have a conditional
 * branch before the site and are worth trying the lever on; 98, including this
 * one, are straight-line at every site.
 *)

*/
#endif
