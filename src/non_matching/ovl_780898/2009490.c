/* OvlFunc_883_2009490 -- NON-MATCHING, 6 differing STREAM encodings of 75.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/u9490.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 75 / ours 75 stream encodings; 51/51 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_780898/2009490.c \
 *     asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_a.s --func OvlFunc_883_2009490
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern unsigned char gScript_883__0200e248[];
extern void OvlFunc_883_200d72c(void);

struct Actor { unsigned char pad00[0x18]; int f18; int f1c; unsigned char pad20[0x4c]; void *f6c; };

extern struct Actor *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809259c(int a, int b);
extern int __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __CutsceneWait(int n);
extern void __MapActor_RunScript(int slot, unsigned char *s);
extern void __MapActor_WaitScript(int slot);
extern void __Func_80925cc(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093054(int a, int b);

void OvlFunc_883_2009490(unsigned char *beh, unsigned char *scr)
{
    struct Actor *a;
    struct Actor *q;
    int v;

    a = __MapActor_GetActor(0x16);
    __CutsceneStart();
    __Func_809259c(0x16, 2);
    __MapActor_Emote(0x16, 0x80 << 1, 0x14);
    __Func_809259c(0, 2);
    __MapActor_Emote(0, 0x81 << 1, 0x28);
    __MapActor_SetBehavior(0, beh);
    __CutsceneWait(0xa);
    __MapActor_Emote(0x16, 0x103, 0);
    __MapActor_RunScript(0x16, scr);
    __MapActor_WaitScript(0);
    v = 0x80 << 9;
    __CutsceneWait(0x14);
    __Func_80925cc(0x16, 2);
    a->f18 = v;
    a->f1c = v;
    q = __MapActor_GetActor(0);
    q->f18 = v;
    q->f1c = v;
    __MessageID(0xfce);
    __Func_8093054(0x16, 0);
    __MapActor_GetActor(0x16)->f6c = OvlFunc_883_200d72c;
    __MapActor_SetBehavior(0x16, gScript_883__0200e248);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_883_2009490 -- asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_c_a.s
 *
 * BLOCKER: POOL-LOADS-FIRST, single-basic-block variant
 * (the same blocker as src/non_matching/ovl_7cb2c0/200bd10.c, same round)
 *
 * 6 of 70, same length, 64 lines exact.  All six are the SAME callee,
 * __MapActor_Emote, at three sites:
 *
 *     rom  mov r2, #0x14 / mov r0, #0x16 / lsl r1, #0x1
 *     ours mov r2, #0x14 / lsl r1, #0x1  / mov r0, #0x16
 *
 * The ROM emits the cheap `mov r0` BEFORE finishing the shifted or pooled
 * argument; gcc finishes the expensive argument first.  The function has no
 * labels at all, so there is no block that dominates the calls without being
 * their own block, and the basic-block lever is unreachable.
 *
 * MEASURED (all 70 lines, all 6 differing at position 14):
 *   plain literals                                     6
 *   __MapActor_Emote declared `int` (return-type lever) 6
 *
 * The return-type lever is worth noting as a negative here because it closed
 * the byte-for-byte identical shape on OvlFunc_881_200b130 in the same round
 * (2 of 70 -> exact, on __StartTask).  So the lever is real and it is not
 * general: it moved a pool-loaded SYMBOL address past a shift there, and does
 * nothing for a shifted integer argument here.  That distinction is worth
 * keeping -- it matches the round-5 measurement that the symbol tell governs
 * hoisting across a call rather than setup order within one.
 *
 * Best C: scratch/u9490.c.
 *)

*/
#endif
