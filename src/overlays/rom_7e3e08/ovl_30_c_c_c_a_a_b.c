/* Cluster OvlFunc_957_200b4bc..OvlFunc_957_200b4bc extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7e3e08/ovl_30_c_c_c_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7e3e08/overlay.ld.
 *
 * A three-message prompt, the variant where BOTH arms repeat the wait, the
 * line and the actor speech rather than joining first. Two levers, both the
 * family's:
 *
 *   * the base id is the symbol `_MSG_217f`, because the ROM reaches the other
 *     two with `add r0, r5, #1` and `#2`;
 *   * `__Func_8092c40` is left UNDECLARED so its two arguments come out r1 then
 *     r0.
 *
 * The base is assigned AFTER `__CutsceneStart()`, where the ROM loads it.
 */
extern int _MSG_217f;
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int a, int b);

void OvlFunc_957_200b4bc(void)
{
    int base;

    __CutsceneStart();
    base = (int)(&_MSG_217f);
    __MessageID(base);
    __Func_8092c40(8, 0);
    if (!__Func_8091c7c(0, 0)) {
        __CutsceneWait(0x14);
        __MessageID(base + 1);
        __ActorMessage(8, 0);
    } else {
        __CutsceneWait(0x14);
        __MessageID(base + 2);
        __ActorMessage(8, 0);
    }
    __CutsceneEnd();
}
