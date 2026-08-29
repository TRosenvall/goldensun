extern unsigned char gScript_945__0200e738[];

extern void __CutsceneStart(void);
extern void OvlFunc_945_200c8e8(int a, int b, int c);
extern void __Func_80925cc(int a, int b);
extern void __CutsceneWait(int n);
extern int __MapActor_SetSpeed(int slot, int x, int y);
extern int __Func_80921c4(int a, int b, int c);
extern void __Func_8092adc(int a, int b, int c);
extern void __MapActor_Jump(int a, int b, int c);
extern void *OvlFunc_945_200c5d0(void);
extern void __PlaySound(int id);
extern void __Actor_SetScript(void *a, unsigned char *s);
extern void __MapActor_DoAnim(int slot, int a);
extern void OvlFunc_945_200c880(int a, int b);
extern void __Func_809259c(int a, int b);
extern void __MessageID(int id);
extern void __Func_8093040(int a, int b, int c);

void OvlFunc_945_200bd10(void)
{
    void *a;
    int sx, sy, p1, p2, p3, p4, e1, e2, jm;

    sx = 0xcccc;
    sy = 0x6666;
    p1 = 0xea << 1;
    p2 = 0x266;
    p3 = 0xec << 1;
    p4 = 0x95 << 2;
    e1 = 0x80 << 8;
    e2 = 0x9c << 2;
    jm = 0xa0 << 7;
    __CutsceneStart();
    OvlFunc_945_200c8e8(0xf, 0, 1);
    __Func_80925cc(8, 1);
    __CutsceneWait(0x14);
    __MapActor_SetSpeed(8, sx, sy);
    __Func_80921c4(8, p1, p2);
    __Func_80921c4(8, p3, p4);
    __Func_8092adc(8, e1, 0x14);
    __MapActor_Jump(8, 4, 0x14);
    a = OvlFunc_945_200c5d0();
    __CutsceneWait(0x14);
    __PlaySound(0xd6);
    __Actor_SetScript(a, gScript_945__0200e738);
    __CutsceneWait(0x28);
    __MapActor_DoAnim(8, 3);
    __CutsceneWait(0x14);
    __Func_80921c4(8, 0xe9 << 1, e2);
    OvlFunc_945_200c880(8, jm);
    __Func_809259c(8, 2);
    __MessageID(0x1e3b);
    __Func_8093040(8, 0, 0x14);
    OvlFunc_945_200c8e8(9, 0xb, 0);
}
