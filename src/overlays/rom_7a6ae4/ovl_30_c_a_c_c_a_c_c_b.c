/* Overlay 920: react-and-turn cutscene stub for slot 0x11.
 *
 * Split out of asm/overlays/rom_7a6ae4/ovl_30_c_a_c_c_a.s. One of three
 * identical stubs differing only in the slot -- 0xF, 0x10 and 0x11 -- which
 * are the three party members trailing the player. Duplicated rather than
 * parameterised, like most of this corpus.
 */

extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int frames);
extern void __PlaySound(int id);
extern void __Func_8092950(int slot, int arg);
extern void __MapActor_DoAnim(int slot, int anim);

void OvlFunc_920_20082d8(void)
{
    __CutsceneStart();
    __Func_8092950(0x11, 0);
    __CutsceneWait(0x28);
    __PlaySound(0xd2);
    __MapActor_DoAnim(0x11, 6);
    __CutsceneEnd();
}
