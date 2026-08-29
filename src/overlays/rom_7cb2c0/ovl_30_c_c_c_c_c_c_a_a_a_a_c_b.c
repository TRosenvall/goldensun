/* Cluster OvlFunc_945_200c198..OvlFunc_945_200c198 extracted from goldensun/asm/overlays/rom_7cb2c0/ovl_30_c_c_c_c_c_c_a_a_a_a_c.s.
 *
 * Split out of that .s; the sibling part stays as assembly.
 *
 * A staging cutscene: two slots configured, a helper run, a third slot
 * configured, one animation set, then the same script installed on slots 0x24,
 * 0x25 and 0x26 and each given the same follow-up. Fifteen calls in forty-five
 * instructions.
 *
 * ONE OBSERVATION WORTH RECORDING, because it bounds the declaration lever
 * from a second direction. __MapActor_SetBehavior is called four times here
 * and the ROM fills its r0 FIRST for slot 0x24 and LAST for 0xc, 0x25 and
 * 0x26 -- the same callee, both orders, inside one function.
 *
 * The declaration lever cannot explain that: a callee is either declared in
 * this translation unit or it is not, so it would have to be one order
 * throughout. Whatever decides the individual call here is something else, and
 * writing the calls plainly reproduces all four correctly without naming it.
 *
 * The lever is real -- it has decided functions in nine batches -- but it is
 * a default, not a law, and this function is the counter-example that says so.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int slot, int n);
extern void OvlFunc_945_200c8e8(int slot, int a, int b);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200d0e4(void);
extern unsigned char gScript_945__0200e840[];
extern unsigned char gScript_945__0200e8e4[];

void OvlFunc_945_200c198(void)
{
    unsigned char *s;

    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 1, 0);
    OvlFunc_945_200c8e8(0x19, 1, 0);
    OvlFunc_945_200b7b4();
    OvlFunc_945_200c8e8(0x13, 0xb, 0xc);
    __MapActor_SetAnim(0xa, 6);
    __MapActor_SetBehavior(0xc, gScript_945__0200e840);
    s = gScript_945__0200e8e4;
    __MapActor_SetBehavior(0x24, s);
    __MapActor_SetBehavior(0x25, s);
    __MapActor_SetBehavior(0x26, s);
    __Func_8092950(0x24, 3);
    __Func_8092950(0x25, 3);
    __Func_8092950(0x26, 3);
    OvlFunc_945_200d0e4();
    __CutsceneEnd();
}
