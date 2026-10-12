/* OvlFunc_915_20089f8 -- MATCHING, landed in batch 338 post-pass.
 *
 * 180 bytes, 74 encodings and 15 relocations identical.  Device-free and pin-free: shimcount prints no rows.
 *
 * HOW IT WAS FOUND, because the route matters more than the function.  This
 * was a PROSE park -- a header, a diagnosis, no body -- and therefore had no
 * figure and was invisible to every targeting instrument.  Three zero-token
 * steps got it here, no agent involved:
 *
 *   1. tools/scratchfill.py found that the park's own header named a candidate
 *      in the GITIGNORED scratch/ directory, and that the file still existed
 *      and still compiled.  70 of 133 prose parks were in that state.
 *   2. Measuring that recovered body gave it a figure for the first time: 2.
 *   3. tools/autosweep.py's ANCHOR axis took it 2 -> 0 on a single
 *      `do { } while (0);` at statement boundary 16.
 *
 * Step 3 only works since batch 338 fixed that axis: it had walked
 * brace-depth-1 boundaries ONLY, so nothing inside an `if` or loop was ever
 * anchored, and a park whose residue is nested got a full "both cheap axes are
 * CLOSED" verdict on half an axis.
 *
 * The anchor is not a stronger barrier -- it works through LOOP NOTES
 * (haifa-sched.c:3714 gates the depend-on-everything block on
 * schedule_barrier_found, which only NOTE_INSN_LOOP_BEG/_END and friends set;
 * a bare `__asm__ volatile ("")` sets none).  :3735-3736 hangs those notes on
 * the FOLLOWING insn, so it fences immediately before the next insn.  It emits
 * nothing and shimcount does not count it, so this landing carries no pass-3
 * debt.
 *
 * The original prose park is preserved at the end of this file.
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
    do { } while (0);
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
