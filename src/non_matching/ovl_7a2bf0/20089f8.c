/* OvlFunc_915_20089f8 -- NON-MATCHING, 2 differing STREAM encodings of 74.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/N89f8_best.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 74 / ours 74 stream encodings; 58/58 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7a2bf0/20089f8.c \
 *     asm/overlays/rom_7a2bf0/ovl_30_c_c_a_a_a.s --func OvlFunc_915_20089f8
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
struct T {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
};

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern int OvlFunc_915_2008474(struct T *t);
extern void OvlFunc_915_2008608(struct T t);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_915_2008244(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);

void OvlFunc_915_20089f8(void)
{
    struct T t;
    unsigned char *e;
    int p1, q1, p2, q2;

    __CutsceneStart();
    if (OvlFunc_915_2008474(&t)) {
        OvlFunc_915_2008608(t);
        if (t.f4 == 0xa && (t.f8 >> 20) == 0xc) {
            __MapActor_SetAnim(0xa, 3);
            __Func_809228c(0xa, -0x12, 6);
            __CutsceneWait(0x1e);
            __PlaySound(0xf0);
            __MapActor_SetAnim(0xa, 8);
            e = __MapActor_GetActor(0xa);
            e[0x23] = 2;
            q2 = 0;
            p1 = 0xb;
            q1 = 0x10;
            __Func_8010704(0x20, 0x14, 2, 4, p1, q1);
            p2 = 4;
            OvlFunc_915_2008244(2, 0xc, 0x10, 1, p2, q2);
            __SetFlag(0x201);
            __Actor_SetSpriteFlags(__MapActor_GetActor(0xa), 0);
        }
    }
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_915_20089f8 -- 0x020089f8,
 * asm/overlays/rom_7a2bf0/ovl_30_c_c_a_a.s
 *
 * 74 of 74 lines, TWO differing, and they are one swap inside one call's
 * argument setup.  Candidate at scratch/N89f8_best.c.
 *
 *      rom   mov r3, #1 / mov r5, #0 / mov r0, #2 / str r5, [sp, #4]
 *      ours  mov r3, #1 / mov r0, #2 / mov r5, #0 / str r5, [sp, #4]
 *
 * SOLVED: everything else, and by template.  This is the same frame as
 * OvlFunc_927_2009454 and OvlFunc_927_20099b8 (both elevated) -- a 24-byte
 * struct filled by ..._2008474 and passed BY VALUE to ..._2008608, which is
 * what the `ldmia r2!, {r0,r1} / stmia r3!, {r0,r1}` block copy means.  The
 * batch-149 stack-argument rules carry over exactly: the (0xb, 0x10) pair is
 * built fresh into two registers so it gets its own pair of locals, and the
 * trailing zero is held in r5 across the preceding __Func_8010704 call so its
 * assignment is placed to span that call.  Both are right here; the zero IS in
 * r5, which is the part that usually goes wrong.
 *
 * BLOCKER: the REMATERIALISATION POINT of that zero.  gcc emits it one slot
 * later than the ROM -- after `mov r0, #2` rather than before it.  The value,
 * the register and the store position are all correct; only where reload drops
 * the `mov` differs, and that is chosen after allocation.
 *
 * SCREENED, all still 2: the assignment moved before the pair, after the pair,
 * before the actor store, and before the whole arm; the declaration reordered
 * so the zero is the first local; the same zero shared with the trailing
 * __Actor_SetSpriteFlags argument; and the per-declaration prototype sweep over
 * every callee (tools/protolever.py).  WORSE: both pair members hoisted
 * together above the __Func_8010704 call (10 differing), the pair written
 * adjacent immediately before the call (8), and the zero left as a literal,
 * which drops it out of r5 entirely (6).
 *
 * Note the contrast with OvlFunc_927_2009454, which matched: there the ROM
 * materialises BOTH pair members adjacently at the top of the setup and stores
 * the first at once; here it stores the first at once but materialises the zero
 * late, between the last register argument and r0.  Same rule, different
 * schedule, and no spelling reaches the difference.
 *)

*/
#endif
