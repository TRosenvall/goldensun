/* Cluster OvlFunc_959_2009038..OvlFunc_959_2009038 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_c_c_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is
 * unchanged.
 *
 * A short cutscene that succeeds or fails: spawn a helper actor, try something
 * with it, and play one of two animations depending on whether the attempt
 * returned -1. Returns whether it succeeded.
 *
 * Three things, all of them levers already in docs/elevation.md, and none of
 * them needing more than one attempt:
 *
 *  1. THE RESULT IS A NAMED LOCAL SET BEFORE THE CUTSCENE. The ROM puts 0 in r8
 *     before the first call and only overwrites it on the success arm:
 *
 *         mov r3, #0 / mov r8, r3 / bl __CutsceneStart ... mov r0, r8
 *
 *     It spends a push and a pop on r8 to keep it. Writing the two arms as
 *     `return 1` / `return 0` loses that entirely.
 *
 *  2. BRANCH POLARITY. The ROM's `beq` jumps AWAY to the __PlaySound arm, so
 *     the != -1 case is the `if` body and the failure case is the `else`.
 *
 *  3. ONE DECLARATION. __MapActor_SetAnim wants r0 first; __Func_808e078 and
 *     __Func_8091a58 want it last, so those two stay implicit. Three calls,
 *     two answers, same as src/overlays/rom_7e7574/ovl_9dc_c_a_a_a_a_a.c next
 *     door -- the declaration lever reporting what the original TU declared.
 *
 * The -1 comparison is built in a register (`mov r3, #1 / neg r3, r3 / cmp
 * r0, r3`) because Thumb cannot encode a negative immediate in `cmp`. That
 * falls out of writing `!= -1` and needs no lever.
 */
extern void __MapActor_SetAnim(int slot, int anim);

int OvlFunc_959_2009038(int a, int b)
{
    int r;
    int h;

    r = 0;
    __CutsceneStart();
    h = __Func_808e078(0, a, b);
    if (__Func_8091a58(b, 0) != -1) {
        __MapActor_SetAnim(a, 2);
        r = 1;
    } else {
        __PlaySound(0x7d);
        __MapActor_SetAnim(a, 5);
    }
    __DeleteActor(h);
    __CutsceneEnd();
    return r;
}
