/* Cluster OvlFunc_957_200b518..OvlFunc_957_200b518 extracted from goldensun/asm/overlays/rom_7e3e08/ovl_30_c_c_c_a.s.
 *
 * Split out of that .s; the sibling parts stay as assembly.
 *
 * A one-shot scene guarded by two flags: it runs only if 0x960 is set and
 * 0x962 is not, and sets 0x961 on the way in so it cannot repeat. Fifteen
 * calls in forty-three instructions.
 *
 * The two guards are one `&&`. gcc emits the short-circuit as two compares
 * with both branches going to the same exit, which is what the ROM has.
 *
 * __Func_80925cc and __Func_809280c are left undeclared so their r0 is filled
 * last; __ActorMessage and __MapActor_DoAnim are declared so theirs comes
 * first.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __MessageID(int id);
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __ActorMessage(int slot, int b);
extern void __MapActor_DoAnim(int slot, int anim);

void OvlFunc_957_200b518(void)
{
    if (__GetFlag(0x96 << 4) && !__GetFlag(0x962)) {
        __SetFlag(0x961);
        __CutsceneStart();
        __MessageID(0x217d);
        __ActorMessage(8, 0);
        __CutsceneWait(0xa);
        __Func_80925cc(0, 2);
        __CutsceneWait(0x1e);
        __Func_809280c(0, 8, 0);
        __CutsceneWait(0x1e);
        __ActorMessage(8, 0);
        __MapActor_DoAnim(0, 3);
        __CutsceneWait(0x14);
        __CutsceneEnd();
    }
}
