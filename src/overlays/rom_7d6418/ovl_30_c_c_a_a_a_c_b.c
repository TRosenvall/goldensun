/* Cluster OvlFunc_951_2008074..OvlFunc_951_2008074 extracted from goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_a_a_a_c_b.s.
 *
 * Split out of that .s; the sibling parts stay as assembly.
 *
 * A three-message prompt: says the opening line, runs a check, and delivers
 * one of two follow-ups at base+1 or base+2. One of seven identical stubs
 * differing only in the message base. See
 * src/overlays/rom_7ebdfc/ovl_30_c_c_a_c_b.c for why the base has to be a
 * symbol and why __Func_8092c40 is left undeclared.
 */
extern void __MessageID(int id);
extern void __CutsceneWait(int frames);
extern void __ActorMessage(int slot, int b);
extern int __Func_8091c7c(int a, int b);
extern int _MSG_e39;

void OvlFunc_951_2008074(int slot)
{
    int base = (int)(&_MSG_e39);

    __MessageID(base);
    __Func_8092c40(slot, 0);
    if (!__Func_8091c7c(0, 0)) {
        __CutsceneWait(0xa);
        __MessageID(base + 1);
    } else {
        __MessageID(base + 2);
    }
    __ActorMessage(slot, 0);
}
