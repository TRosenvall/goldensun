extern unsigned char *__MapActor_GetActor(int slot);
extern int OvlFunc_927_2008cd0(int *p);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __WaitFrames(int n);

void OvlFunc_927_2009078(void)
{
    int buf[3];
    unsigned char *e;
    unsigned char *f;
    int saved;
    int d;

    e = __MapActor_GetActor(0);
    saved = e[0x55];
    f = e + 0x55;
    buf[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    buf[1] = *(int *)(e + 0xc);
    buf[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0xa0 << 14);
    if (OvlFunc_927_2008cd0(buf) != 0) {
        __CutsceneStart();
        *f = 0;
        __MapActor_SetAnim(9, 7);
        d = 0xffff0000;
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(2);
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(0xa);
        d = 0x80 << 9;
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        __WaitFrames(4);
        *(int *)(e + 0xc) += d;
        *(int *)(e + 0x14) += d;
        *f = saved;
        __CutsceneEnd();
    }
}
