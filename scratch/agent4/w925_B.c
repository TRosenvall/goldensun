extern unsigned char *__MapActor_GetActor(int);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __SetFlag(int);
extern void __WaitFrames(int);
extern int OvlFunc_925_20088cc(void);
extern int slot __asm__(".L3c50");

void OvlFunc_925_20089fc(void)
{
    unsigned char *a;
    unsigned char *b;
    int d;
    int m;

    a = __MapActor_GetActor(0);
    __CutsceneStart();
    slot = OvlFunc_925_20088cc();
    if (slot != 0) {
        __SetFlag(0x250);
        b = __MapActor_GetActor(slot);
        b[0x55] = 0;
        m = 0xfe;
        m &= a[0x55];
        a[0x55] = m;
        *(int *)(b + 0xc) += 0xfffe0000;
        *(int *)(a + 0xc) += 0xfffe0000;
        *(int *)(a + 0x14) += 0xfffe0000;
        __WaitFrames(2);
        *(int *)(b + 0xc) += d;
        *(int *)(a + 0xc) += d;
        *(int *)(a + 0x14) += d;
        __WaitFrames(0xa);
        d = 0x20000;
        d = 0x20000;
        *(int *)(b + 0xc) += d;
        *(int *)(a + 0xc) += d;
        *(int *)(a + 0x14) += d;
        __WaitFrames(4);
        *(int *)(b + 0xc) += 0x10000;
        *(int *)(a + 0xc) += 0x10000;
        *(int *)(a + 0x14) += 0x10000;
        __WaitFrames(4);
        *(int *)(b + 0xc) += d;
        *(int *)(a + 0xc) += d;
        *(int *)(a + 0x14) += d;
    }
    __CutsceneEnd();
}
