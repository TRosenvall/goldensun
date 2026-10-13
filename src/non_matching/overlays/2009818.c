/* OvlFunc_927_2009818 -- NON-MATCHING, 3 differing STREAM encodings of 38.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/L9818.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 38 / ours 38 stream encodings; 24/24 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/overlays/2009818.c \
 *     asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c_b.s --func OvlFunc_927_2009818
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 * BOTH MECHANICAL AXES ARE CLOSED -- measured batch 347 harvest pass, no agent.
 *   base 3 of 38, instruction counts EQUAL, byte size and pool
 *   count equal too (this park is in phase on all three length dimensions, so
 *   its figure IS a real distance).
 *   FLAG AXIS: all eight per-file groups measured, none improves the aligned
 *   residue.  ANCHOR AXIS: all 13 statement boundaries at every brace depth
 *   measured, all inert or worse.  No variant was capped and none failed to
 *   compile.
 *
 *   SO DO NOT RE-SCREEN THESE.  Every brief so far has spent part of a round on
 *   exactly that.  The residue here is NOT a placement or flag question; it is a
 *   register choice, a tie, or a pass-level effect, and it needs a hypothesis.
 *
 *   Read as "inert against THIS body".  A recorded negative expires silently
 *   when the body changes -- six batches running, the decisive lever has turned
 *   up inside a park's own negative list.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008e18(int a);
extern void __Func_8092950(int a, int b);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __SetFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_927_2009818(void)
{
    unsigned char *a;

    __CutsceneStart();
    OvlFunc_927_2008ea8(0xe, 1);
    OvlFunc_927_2008d90(0xe, 0xd4 << 1, 0xf0 << 1, 0x79999);
    __CutsceneWait(2);
    OvlFunc_927_2008e18(0xe);
    __Func_8092950(0xe, 0xf);
    a = __MapActor_GetActor(0xe);
    __Actor_SetSpriteFlags(a, 0);
    __CutsceneWait(0x1e);
    __SetFlag(0x305);
    __MapActor_SetPos(0x11, 0xd4 << 17, 0xf0 << 17);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_927_2009818 -- 0x02009818,
 * asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_c.s
 *
 * 36 of 36 lines, THREE differing.  Candidate at scratch/L9818.c.
 *
 *      rom   mov r1,#0xd4 / mov r2,#0xf0 / mov r0,#0x11 / lsl r1,#17 / lsl r2,#17
 *      ours  mov r1,#0xd4 / mov r2,#0xf0 / lsl r1,#17   / lsl r2,#17 / mov r0,#0x11
 *
 * Same bound as OvlFunc_939_2008b0c: two split builds at the site, no branch in
 * the function.  Naming the four shifted constants is dramatically worse (49
 * lines against 36) -- gcc spills them to r5/r6/r8/r10 and the prologue grows
 * from `push {r14}` to a four-register save.
 *
 * That cost is worth noting on its own: in a straight-line function the naming
 * lever does not merely fail to help, it can add half a dozen instructions.
 * Check the branch count BEFORE reaching for it.
 *)

*/
#endif
