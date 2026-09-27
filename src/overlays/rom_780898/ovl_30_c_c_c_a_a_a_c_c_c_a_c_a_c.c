/* OvlFunc_883_20091d8  --  0x020091d8, was
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_c.s (this function
 * alone), so it converts whole.
 *
 * Parked at 10 differing. Two edits closed it:
 *  - `k = 0x366;` assigned BEFORE the __GetFlag guard and passed as the third
 *    argument -- the basic-block lever (reports/arg-interleave.md). 10 -> 7.
 *  - `do { } while (0);` after __MapActor_SetSpeed(...): sched2 had lifted the
 *    callee-saved `ldr r5` above the call, and the loop notes are a total
 *    scheduling barrier (batch 282, haifa-sched.c:3714).
 */
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int a);
extern void __MapActor_SetSpeed(int who, int a, int b);
extern void __Func_8093040(int a, int b, int c);
extern void __Func_801776c();
extern void __Func_80921c4(int a, int b, int c);

void OvlFunc_883_20091d8(void)
{
    int m;
    int k;

    k = 0x366;
    if (__GetFlag(0x808) != 0)
        return;
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    do { } while (0);
    m = 0xf4d;
    __MessageID(m);
    __Func_8093040(0xf, 0, 2);
    m += 2;
    __Func_8093040(0x10, 0, 2);
    __Func_801776c(m, 1);
    __CutsceneWait(6);
    __Func_80921c4(0, 0x45, k);
    __CutsceneEnd();
}
