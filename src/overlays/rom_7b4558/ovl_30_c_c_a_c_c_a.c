/* Cluster OvlFunc_927_2009454..OvlFunc_927_2009454 extracted from
 * goldensun/asm/overlays/rom_7b4558/ovl_30_c_c_a_c_c_a.s.
 *
 * The same frame as OvlFunc_927_20099b8: a 24-byte struct filled by
 * OvlFunc_927_2008474 and passed BY VALUE to OvlFunc_927_2008608, which is why
 * the ROM block-copies the tail two words with `ldmia r2!, {r0,r1} / stmia r3!,
 * {r0,r1}` instead of loading them into spare registers.  See that file's
 * header for the tell.
 *
 * WHAT THIS ONE ADDS: a stack argument lands in a CALLEE-SAVED register only if
 * its live range CROSSES A CALL.
 *
 * The last call passes (4, 0) on the stack and the ROM builds them as
 *      mov r3, #4 / mov r5, #0 / str r3, [sp] / <r0-r3 set up> / str r5, [sp,#4]
 * -- the zero sits in r5 across the whole register-argument setup and is stored
 * last.  Declaring the pair immediately before the call, which is what the
 * batch-149 rule asks for, gives the zero a SCRATCH register (r2) and stores it
 * straight away: 88 lines, 6 differing.  Moving `q3 = 0;` up so that it spans
 * the preceding __Func_8010704 call makes gcc spend r5 on it and the function
 * matches exactly.
 *
 * So the per-site pair rule sets WHICH values get names, and where the
 * assignment goes sets WHICH REGISTER CLASS they get.  Screened: the pair
 * written in the other order (7 differing) and the zero hoisted to the top of
 * the function (7).  Only the placement that crosses exactly one call works.
 *
 * The two earlier six-argument sites take their pairs beside the call, as
 * usual -- neither of their values needs to survive anything.
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
extern int OvlFunc_927_2008474(struct T *t);
extern void OvlFunc_927_2008608(struct T t);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);
extern void __MapActor_SetAnim(int a, int b);
extern void __Func_809228c(int a, int b, int c);
extern void __PlaySound(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void OvlFunc_927_2008244(int a, int b, int c, int d, int e, int f);

void OvlFunc_927_2009454(void)
{
    struct T t;
    unsigned char *e;
    int p1, q1, p2, q2, p3, q3;

    __CutsceneStart();
    if (OvlFunc_927_2008474(&t)) {
        OvlFunc_927_2008608(t);
        if (t.f4 == 8 && (t.f10 >> 20) == 0x17) {
            p1 = 0x23;
            q1 = 0x44;
            __Func_8010704(0x23, 0x43, 4, 1, p1, q1);
        } else if (t.f4 == 0xa && (t.f8 >> 20) == 0x23) {
            __SetFlag(0x311);
            __MapActor_SetAnim(0xa, 3);
            __Func_809228c(0xa, -0x10, 6);
            __CutsceneWait(0x1e);
            __MapActor_SetAnim(0xa, 8);
            __PlaySound(0xf0);
            e = __MapActor_GetActor(0xa);
            e[0x23] = 2;
            q3 = 0;
            p2 = 0x22;
            q2 = 0x1e;
            __Func_8010704(0x2c, 0x1e, 2, 4, p2, q2);
            p3 = 4;
            OvlFunc_927_2008244(2, 0x23, 0x1e, 1, p3, q3);
        }
    }
    __CutsceneEnd();
}
