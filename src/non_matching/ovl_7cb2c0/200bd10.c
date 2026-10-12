/* OvlFunc_945_200bd10 -- NON-MATCHING, 13 differing STREAM encodings of 83.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/ubd10.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 83 / ours 83 stream encodings; 55/55 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7cb2c0/200bd10.c \
 *     asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c_c_a_c_a_c.s --func OvlFunc_945_200bd10
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char gScript_945__0200e738[];

extern void __CutsceneStart(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern int __MapActor_SetSpeed(int slot, int x, int y);
extern int __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void *OvlFunc_945_200c5d0(void);
extern void __PlaySound(int id);
extern void __Actor_SetScript(void *a, unsigned char *s);
extern void __MapActor_DoAnim(int slot, int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200bd10(void)
{
    void *a;

    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 0, 1);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __MapActor_SetSpeed(8, 0xcccc, 0x6666);
    __Func_80921c4(8, 0xea << 1, 0x266);
    __Func_80921c4(8, 0xec << 1, 0x95 << 2);
    __Func_8092adc(8, 0x80 << 8, 0x14);
    __MapActor_Jump(8, 4, 0x14);
    a = OvlFunc_945_200c5d0();
    __CutsceneWait(0x14);
    __PlaySound(0xd6);
    __Actor_SetScript(a, gScript_945__0200e738);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_80921c4(8, 0xe9 << 1, 0x9c << 2);
    OvlFunc_945_200c880(8, 0xa0 << 7);
    __Func_809259c(8, 2);
    __MessageID(0x1e3b);
    __Func_8093040(8, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xb, 0);
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_945_200bd10 -- asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_a_c_c.s
 *
 * BLOCKER: POOL-LOADS-FIRST, single-basic-block variant
 *
 * 13 of 77, same length, and 64 lines are exact.  Every difference is the same
 * decision at four call sites whose arguments include a pooled or shifted
 * constant:
 *
 *     rom  mov r0, #0x8 / ldr r1, =0xcccc / ldr r2, =0x6666
 *     ours ldr r1, =0xcccc / ldr r2, =0x6666 / mov r0, #0x8
 *
 * gcc emits the expensive arguments first and the cheap `mov r0` last; the ROM
 * emits r0 first.  The function is one basic block -- `push {r5, lr}`, no
 * labels anywhere -- so the basic-block lever has nothing to bite on.
 *
 * MEASURED (all 77 lines, all 13 differing at position 11 unless noted):
 *   plain literals                                             13
 *   __MapActor_SetSpeed and __Func_80921c4 declared `int`      13
 *     (the return-type lever, which DID close the identical shape on
 *      OvlFunc_881_200b130's __StartTask one round earlier)
 *   both prototypes removed entirely                           13
 *   every shifted/pooled argument hoisted to an `int` local at
 *     the top of the function                        96 of 77, ours 97 lines
 *
 * That last one is the informative negative.  Round-5 agent1 recorded hoisting
 * the argument constants to top-of-function locals as the fourth answer to
 * this shape, and it is -- but only where the locals live in a block that
 * DOMINATES the calls without being their own block.  Here there is only one
 * block, so hoisting nine constants makes them all simultaneously live, gcc
 * spills, and the function grows by twenty instructions.
 *
 * So the rule wants a precondition attached: the hoist lever needs a real
 * dominating block, exactly like the basic-block lever it is a variant of.
 * In a straight-line function it is actively harmful.
 *
 * Best C: scratch/ubd10.c.
 *)

*/
#endif
