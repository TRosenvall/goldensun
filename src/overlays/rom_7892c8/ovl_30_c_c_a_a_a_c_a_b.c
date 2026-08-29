/* Cluster OvlFunc_888_20086e8..OvlFunc_888_20086e8 extracted from goldensun/asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_a.o and the rest of the overlay
 * in goldensun/overlays/rom_7892c8/overlay.ld.
 *
 * A thirteen-call cutscene, straight-line, and a clean example of the
 * declaration lever used SUBTRACTIVELY four times in one function.
 *
 * Only __Func_80933d4 is declared, because it is the only callee whose r0 the
 * ROM fills first. The other four -- __Func_8093500, __Func_809280c,
 * __MapActor_DoAnim and __ActorMessage -- all have r1 (or r2) before r0 in the
 * ROM, so their prototypes are withheld:
 *
 *     mov r1, #1  / mov r0, #1  / bl __Func_8093500
 *     mov r2, #0  / mov r1, #0  / mov r0, #8 / bl __Func_809280c
 *     mov r1, #4  / mov r0, #8  / bl __MapActor_DoAnim
 *     mov r1, #0  / mov r0, #8  / bl __ActorMessage
 *
 * Reading the argument order off the ROM before writing anything is what makes
 * a function like this a single screen rather than a sweep. See
 * docs/elevation.md: declare the ones whose r0 comes first, withhold from the
 * ones whose r0 comes last.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern void __SetFlag(int id);

void OvlFunc_888_20086e8(void)
{
    __CutsceneStart();
    __Func_80933d4(0x80 << 9, 0x80 << 6);
    __Func_8093500(1, 1);
    __Func_8093530();
    __CutsceneWait(0x14);
    __Func_809280c(8, 0, 0);
    __CutsceneWait(0xa);
    __MapActor_DoAnim(8, 4);
    __CutsceneWait(0x14);
    __MessageID(0x116c);
    __ActorMessage(8, 0);
    __SetFlag(0x80 << 2);
    __CutsceneEnd();
}
