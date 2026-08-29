extern unsigned int iwram_3001e70;
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __Func_8012330(int a, int b, int c);
extern void __CutsceneWait(int frames);
extern void __WaitFrames(int frames);
extern void __SetIntrHandler(int a, int b, void *h);
extern void __SetFlag(int id);
extern void OvlFunc_932_20086a0(void);
extern unsigned short L5238 __asm__(".L5238");

void OvlFunc_932_20087e8(void)
{
    unsigned char *p;
    int i;

    p = *(unsigned char **)&iwram_3001e70;
    __Func_80105d4(0x5d, 0x29, 0x10, 4, 0x4d, 0x1c);
    __PlaySound(0xe6);
    __Func_8012330(0x80 << 10, 0x80 << 10, 0x80 << 9);
    __CutsceneWait(0xa);
    p += 0xb2 << 1;
    i = 0x17;
    do {
        *(int *)(p + 0xc) += 0xffff0000;
        __WaitFrames(4);
        i--;
    } while (i >= 0);
    __SetIntrHandler(1, 0, OvlFunc_932_20086a0);
    L5238 = 0;
    do {
        __WaitFrames(1);
        L5238 = L5238 + 1;
    } while (L5238 <= 0x64);
    __WaitFrames(1);
    __SetIntrHandler(1, 0, 0);
    __PlaySound(0x121);
    __Func_8012330(-1, -1, 0xe666);
    __CutsceneWait(0x1e);
    __Func_80105d4(0x4d, 0x29, 0x10, 4, 0x4d, 0x1c);
    __SetFlag(0x8fe);
}
