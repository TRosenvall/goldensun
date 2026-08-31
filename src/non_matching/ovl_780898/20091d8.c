/* OvlFunc_883_20091d8  [overlays/rom_780898]
 *
 * Source asm: goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a_c.s
 *
 * BLOCKER CLASS: pool-loads-first. 38 lines against 38, TEN differing, in two
 * clusters that are the same shape twice -- gcc hoists a pool load ABOVE the
 * argument setup where the ROM emits it after.
 *
 *     rom   mov r1,#0x80 / mov r2,#0x80 / lsl r1,#9 / lsl r2,#8 / mov r0,#0
 *             / bl __MapActor_SetSpeed / ldr r5, =0xf4d
 *     ours  ldr r5, =0xf4d / mov r1,#0x80 / ... / bl __MapActor_SetSpeed
 *
 *     rom   mov r0,#0 / mov r1,#0x45 / ldr r2, =0x366 / bl
 *     ours  ldr r2, =0x366 / mov r0,#0 / mov r1,#0x45 / bl
 *
 * Everything else matches: the guard, the batched constant build
 * (`mov`/`mov`/`lsl`/`lsl`), the message id held in a callee-saved register and
 * advanced with `add r5, #2`, and every call.
 *
 * HOW IT WAS PICKED, which is the reusable part: found by scanning UNMATCHED
 * functions for the interleave signature -- a two-instruction constant build
 * with a conditional branch dominating the site. Fifteen candidates in the
 * 25-70 instruction range carry it; this is the smallest. The scan is worth
 * re-running rather than picking candidates by size alone.
 *
 * THE NAMED LOCAL FOR THE MESSAGE ID IS REQUIRED, not optional: writing the
 * two ids as literals at their call sites is 35 lines and 27 differing. The
 * ROM's `add r5, #2` says the id is one value advanced in place.
 *
 * A SYMBOL WAS NOT ADDED FOR IT, deliberately. The pooled-constant tell says a
 * small pooled value means a symbol, but 0xf4d does NOT fit an eight-bit `mov`,
 * so the pool load is the natural encoding for a plain literal too and the tell
 * does not apply. Adding `_MSG_f4d` to message.sym on that evidence would be
 * guessing.
 *
 * MEASURED, all 10 unless noted:
 *   named local for the id, literals for the speeds      10  (best)
 *   both ids as bare literals, no local                  27  (35 lines)
 *   __Func_80921c4 left undeclared                       10
 *   the two speed arguments named in the same block      10
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

    if (__GetFlag(0x808) != 0)
        return;
    __CutsceneStart();
    __MapActor_SetSpeed(0, 0x80 << 9, 0x80 << 8);
    m = 0xf4d;
    __MessageID(m);
    __Func_8093040(0xf, 0, 2);
    m += 2;
    __Func_8093040(0x10, 0, 2);
    __Func_801776c(m, 1);
    __CutsceneWait(6);
    __Func_80921c4(0, 0x45, 0x366);
    __CutsceneEnd();
}
