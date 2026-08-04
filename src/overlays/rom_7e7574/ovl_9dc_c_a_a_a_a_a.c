/* Cluster OvlFunc_959_20090a8..OvlFunc_959_20090a8 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is
 * unchanged.
 *
 * A straight-line cutscene beat: walk the actor, wait, play an animation twice
 * with a sound between, set a flag, and reposition. No branches at all.
 *
 * THE ONLY THING IT NEEDS IS THREE DECLARATIONS, and which three is read
 * straight off the ROM. Six calls take more than one argument and they do NOT
 * agree about where r0 goes:
 *
 *     r0 LAST   __Func_809218c, __Func_8092adc, __Func_8091a58
 *     r0 FIRST  __MapActor_SetAnim, __Func_808f1c0, __MapActor_SetPos
 *
 * gcc fills r0 last for an implicitly declared callee, so the three that want
 * it first are declared and the three that want it last are left implicit.
 * Matched on the first screen.
 *
 * That mix inside one function is the clearest small demonstration of what the
 * declaration lever actually reports: not a property of the callee, and not a
 * house style, but whether the ORIGINAL translation unit had a prototype in
 * scope for that particular function. Six calls, two answers, and the ROM says
 * which is which.
 *
 * The two shifted arguments are written as `0x84 << 1` and `0xc6 << 2` rather
 * than folded, since gcc builds them with mov+lsl either way and the shift
 * keeps the relationship to the underlying value readable.
 */
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_808f1c0(int a, int b);
extern void __MapActor_SetPos(int slot, int x, int z);

void OvlFunc_959_20090a8(void)
{
    __Func_809218c(0, 0x84 << 1, 0xc6 << 2);
    __MapActor_WaitMovement(0);
    __Func_8092adc(0, 0x80 << 7, 0);
    __CutsceneWait(0xa);
    __MapActor_SetAnim(0, 1);
    __Func_808f1c0(0xea, 3);
    __MapActor_SetAnim(0, 1);
    __Func_8091a58(0xea, 0);
    __SetFlag(0xf2e);
    __MapActor_SetPos(8, 0, 0);
}
