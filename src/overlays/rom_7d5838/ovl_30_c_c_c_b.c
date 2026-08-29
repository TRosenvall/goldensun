/* Cluster OvlFunc_950_20088cc..OvlFunc_950_20088cc extracted from goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d5838/ovl_30_c_c_c_a.o and
 * asm/overlays/rom_7d5838/ovl_30_c_c_c_c.o in
 * goldensun/overlays/rom_7d5838/overlay.ld.
 *
 * A three-message prompt inside a cutscene, byte-for-byte the shape of
 * src/overlays/rom_7d5838/ovl_30_c_c_a_c_c_b.c one round earlier -- found by
 * tools/match_shapes.py rather than by reading candidates. Both levers are the
 * exemplar's:
 *
 *   - the base id is the symbol `_MSG_23ac`, because the ROM reaches the other two
 *     with `add r0, r5, #1` and `#2` and gcc only emits that for a symbol
 *     address;
 *   - `__Func_8092c40` is deliberately left UNDECLARED, which is what puts its
 *     two arguments in the ROM's order;
 *   - and the base is assigned AFTER `__CutsceneStart()`, where the ROM loads
 *     it. Assigned in the declaration it is hoisted above the call.
 */
extern int _MSG_23ac;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int slot, int b);

void OvlFunc_950_20088cc(int slot)
{
    int base;

    __CutsceneStart();
    base = (int)(&_MSG_23ac);
    __MessageID(base);
    __Func_8092c40(slot, 0);
    if (!__Func_8091c7c(0, 0)) {
        __CutsceneWait(0xa);
        __MessageID(base + 1);
    } else {
        __MessageID(base + 2);
    }
    __ActorMessage(slot, 0);
    __CutsceneEnd();
}
