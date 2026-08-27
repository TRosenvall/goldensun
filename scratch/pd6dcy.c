extern unsigned char gScript_945__0200e840[];
extern unsigned char gScript_945__0200e8e4[];

extern int OvlFunc_945_200cfa8(int a, int b);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void OvlFunc_945_200b7b4(void);
extern void OvlFunc_945_200d0e4(void);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int a);
extern void __MapActor_SetBehavior(int slot, unsigned char *s);
extern void __DeleteFieldActor(int slot);
extern void __Func_8092950(int a, int b);

void OvlFunc_945_200d6dc(void)
{
    int a;
    unsigned char *s;
    int b;

    a = OvlFunc_945_200cfa8(0, 0);
    b = OvlFunc_945_200cfa8(1, 0);
    __CutsceneStart();
    OvlFunc_945_200c8e8(0x18, 1, 0);
    OvlFunc_945_200c8e8(0x19, 3, 0);
    OvlFunc_945_200b7b4();
    OvlFunc_945_200c8e8(0x13, a, b);
    __MapActor_SetAnim(0xa, 6);
    s = gScript_945__0200e840;
    __MapActor_SetBehavior(a, s);
    __DeleteFieldActor(0xb);
    __MapActor_SetBehavior(b, s);
    __DeleteFieldActor(0xc);
    s = gScript_945__0200e8e4;
    __MapActor_SetBehavior(0x24, s);
    __MapActor_SetBehavior(0x25, s);
    __Func_8092950(0x24, 3);
    __Func_8092950(0x25, 3);
    OvlFunc_945_200d0e4();
    __CutsceneEnd();
}
