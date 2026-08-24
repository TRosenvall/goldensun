/* Cluster OvlFunc_950_2008760..OvlFunc_950_2008760 extracted from goldensun/asm/overlays/rom_7d5838/ovl_30_c_c_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d5838/ovl_30_c_c_a_c_c_c.o and the rest of the overlay in
 * goldensun/overlays/rom_7d5838/overlay.ld.
 *
 * A three-message prompt wrapped in a cutscene, with a ten-frame wait on the
 * yes arm. Same two levers as src/overlays/rom_7d768c/ovl_30_c_a_a_a_b.c: the
 * base id is the symbol `_MSG_1fbb`, and `__Func_8092c40` is left UNDECLARED.
 *
 * The base is assigned AFTER `__CutsceneStart()` because that is where the ROM
 * loads it -- `mov r6, r0 / bl __CutsceneStart / ldr r5, =0x1fbb`. Assigned in
 * the declaration it is hoisted above the call and the prologue differs.
 */
extern int _MSG_1fbb;

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern int __Func_8091c7c(int a, int b);
extern void __ActorMessage(int slot, int b);

void OvlFunc_950_2008760(int slot)
{
    int base;

    __CutsceneStart();
    base = (int)(&_MSG_1fbb);
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
