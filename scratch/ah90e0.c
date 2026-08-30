extern unsigned char *__MapActor_GetActor(int slot);
extern int __TestCollision(unsigned char *a, int *b);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *a, int n);
extern void __Func_8092158(int a, int b, int c);

int OvlFunc_934_20090e0(void)
{
    int buf[3];
    unsigned char *e;
    unsigned char *f;
    int saved;

    e = __MapActor_GetActor(0);
    f = e + 0x55;
    saved = *f;
    buf[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    buf[1] = *(int *)(e + 0xc);
    buf[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0xa0 << 14);
    if (__TestCollision(e, buf) == 0) {
        __CutsceneStart();
        __Actor_SetAnim(e, 6);
        __WaitFrames(6);
        __PlaySound(0x98);
        __Actor_SetAnim(e, 7);
        *(int *)(e + 0x30) = 0xc0 << 10;
        *(int *)(e + 0x34) = 0x80 << 10;
        *(int *)(e + 0x28) = 0x80 << 11;
        *f = *f & 0x7e;
        __Actor_SetSpriteFlags(e, 0);
        __Func_8092158(0, *(short *)((char *)buf + 2), *(short *)((char *)buf + 0xa));
        __Actor_SetAnim(e, 6);
        __Actor_SetSpriteFlags(e, 1);
        *f = saved;
        __CutsceneEnd();
        return 1;
    } else {
        return 0;
    }
}
