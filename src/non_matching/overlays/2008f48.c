/* OvlFunc_923_2008f48  --  0x02008f48, asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.s
 *
 * Source asm: goldensun/asm/overlays/rom_7aa430/ovl_e90_c_c_a_a_c_c.s
 *
 * BLOCKER CLASS: argument precompute. 36 lines against 36, THIRTY-FOUR
 * identical, and the two that differ are a transposition:
 *
 *     rom    ldr r2, =0x3333 / mov r0, #0    / ldr r1, =0x6666
 *     ours   ldr r2, =0x3333 / ldr r1, =0x6666 / mov r0, #0
 *
 * `precompute_register_parameters` (calls.c:805) copies every argument whose
 * rtx_cost > 2 into a pseudo before any hard register is loaded. Both 0x6666
 * and 0x3333 are pool loads and both exceed the threshold, so ours precomputes
 * both and the cheap `mov r0, #0` lands last. The ROM precomputed only the
 * third argument -- identical in shape and in arm_rtx_costs to the second, so
 * no C expression separates them. Same bind as OvlFunc_898_2008ef4.
 *
 * MEASURED, all 2 of 36:
 *   all callees prototyped (this file)
 *   __PlaySound implicit                    (the preceding-call lever)
 *   __PlaySound given an `int` return type
 *   __MapActor_SetSpeed implicit            (the mismatching-call lever)
 *   both implicit
 *   the zero passed through a named `int` local
 *
 * and two that are worse: naming 0x6666 as a local (37 lines), naming 0x3333 as
 * a local (37 lines). Both declaration levers were tried in both directions;
 * neither reaches a transposition that is not about r0's position relative to
 * the OTHER register arguments.
 */
extern void *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern void __MapActor_SetSpeed(int slot, int vx, int vz);
extern void __MapActor_SetPos(int slot, int x, int z);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092b08(int slot, int n);
extern void __Func_809228c(int a, int b, int c);

void OvlFunc_923_2008f48(int a)
{
    __CutsceneStart();
    __PlaySound(0xe4);
    __MapActor_SetSpeed(0, 0x6666, 0x3333);
    __Func_8092b08(0, 2);
    __Func_809228c(0, 0, -8);
    __Actor_SetSpriteFlags(__MapActor_GetActor(0), 0);
    __CutsceneWait(8);
    __MapActor_SetPos(0, (a << 19) + (0x80 << 12), 0);
    __CutsceneWait(0x1e);
}
