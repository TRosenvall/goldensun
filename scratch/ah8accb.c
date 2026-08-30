extern unsigned char *__MapActor_GetActor(int slot);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MessageID(int id);
extern void __MapActor_SetAnim(int slot, int anim);
extern void OvlFunc_898_200973c(int a, int b, int c);
extern void OvlFunc_898_2009724(int a, int b);
extern void __WaitFrames(int n);

void OvlFunc_898_2008acc(void)
{
    unsigned char *e;
    unsigned short *f;
    int saved;

    e = __MapActor_GetActor(0xf);
    f = (unsigned short *)(e + 0x64);
    saved = *(short *)(e + 6);
    *f |= 2;
    __CutsceneStart();
    __MessageID(0x133b);
    __MapActor_SetAnim(0xf, 0);
    OvlFunc_898_200973c(0xf, 0, 2);
    OvlFunc_898_2009724(0xf, 0xa);
    *(short *)(e + 6) = saved;
    __WaitFrames(1);
    __CutsceneEnd();
    *f &= 1;
}
