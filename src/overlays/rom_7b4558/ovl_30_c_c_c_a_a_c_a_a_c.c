/* OvlFunc_927_2009d04 -- MATCHING, landed in batch 338 post-pass.
 *
 * 220 bytes, 86 encodings and 21 relocations identical.  Device-free and pin-free: shimcount prints no rows.
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
 *      `do { } while (0);` at statement boundary 17.
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
extern unsigned char gState[];
extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void OvlFunc_927_2008ea8(int a, int b);
extern void OvlFunc_927_2008d90(int a, int b, int c, int d);
extern void OvlFunc_927_2008ae8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void __SetCameraTarget(int a, int b);
extern void __Func_8092848(int a, int b, int c);
extern void __Func_809259c(int a, int b);
extern void __MapActor_Emote(int a, int b, int c);
extern void __PlaySound(int id);
extern void __Func_8091eb0(int a, int b);
extern void __SetFlag(int id);

void OvlFunc_927_2009d04(void)
{
    unsigned char *e;
    unsigned char *g;
    int z;
    int hx;

    e = __MapActor_GetActor(0xf);
    z = 0x80 << 12;
    __CutsceneStart();
    OvlFunc_927_2008ea8(0xf, 0);
    OvlFunc_927_2008d90(0xf, 0xec << 1, 0x68, z);
    __CutsceneWait(0xa);
    OvlFunc_927_2008ae8(*(int *)(e + 8), *(int *)(e + 0xc),
                        *(int *)(e + 0x10) + z, 0,
                        0, 0, 1, 0);
    __SetCameraTarget(0xf, 1);
    __Func_8092848(0xf, 0, 0);
    __CutsceneWait(0x1e);
    __Func_809259c(0xf, 2);
    __MapActor_Emote(0xf, 0x103, 0);
    __PlaySound(0x93);
    __CutsceneWait(0x3c);
    hx = *(short *)(__MapActor_GetActor(0) + 0xa);
    OvlFunc_927_2008d90(0xf, hx, *(short *)(__MapActor_GetActor(0) + 0x12),
                        0xc0 << 11);
    __CutsceneWait(0xa);
    do { } while (0);
    __SetFlag(0x307);
    g = gState;
    g[0x22b] = 3;
    __Func_8091eb0(0x35, 0);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_927_2009d04 -- 0x02009d04,
 * asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_a_a.s
 *
 * 81 of 81 lines, TWO differing.  Candidate at scratch/Nd04_best.c.
 *
 * The OvlFunc_927 cutscene template at actor slot 0xf, and a near-twin of
 * OvlFunc_927_2009244 (elevated, src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c_c_a.c)
 * -- same two __MapActor_GetActor(0) coordinate reads, same gState byte store
 * through a named base, same __Func_8091eb0 tail.  Everything the twin taught
 * carried over and the whole body came out right on the first screen.
 *
 * BLOCKER: argument-construction interleave at the SECOND OvlFunc_927_2008d90.
 *      rom   mov r3, #0xc0 / lsl r3, #0xb / mov r1, r5
 *      ours  mov r3, #0xc0 / mov r1, r5   / lsl r3, #0xb
 * The ROM keeps the constant's mov/lsl pair CONTIGUOUS and we split it with the
 * copy of the saved coordinate.  This is the same class as
 * src/non_matching/ovl_77a7c8/200b57c.c and it wants the interleave lever run
 * BACKWARDS -- less interleaving, not more -- and the only control over that is
 * where the constant is defined, in a function with no branch to define it in.
 *
 * WORTH NOTING FOR THE CLASS: the FIRST OvlFunc_927_2008d90 call in this same
 * function is interleaved in the ROM (`mov r1, #0xec / mov r3, r6 / lsl r1, #1`)
 * and we reproduce that one exactly.  So the two sites of one callee want
 * opposite treatment, which is the shape the per-call-site declaration lever was
 * written for -- and it does not reach this one.
 *
 * SCREENED AND UNCHANGED AT 2: the fourth argument as a local assigned just
 * before the call (that one is WORSE, 82 lines and 24 differing, because naming
 * it buys a callee-saved register); the second coordinate as a named local;
 * `short hx` instead of `int hx`; the two halfword reads as typed struct fields
 * -- which is what fixed 200b57c and does nothing here; swapping the
 * declaration order of the locals; hoisting `z` above the GetActor call;
 * deleting OvlFunc_927_2008d90's prototype; and an `__asm__` alias with a
 * different return type applied to the second call site, then to the first.
 *)

*/
#endif
