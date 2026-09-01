extern char *__MapActor_GetActor(int slot);
extern int __TestCollision(char *a, int *v);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(char *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(char *a, int f);
extern void __Func_8092158(int a, int b, int c);
extern void __MapActor_SetAnim(int slot, int anim);

int OvlFunc_915_2008aac(int *v)
{
    char *a;
    unsigned char *f;
    int saved;
    int r;
    int d;
    int e;

    a = __MapActor_GetActor(0);
    f = (unsigned char *)(a + 0x55);
    saved = *f;
    r = __TestCollision(a, v);
    if (r == 0) {
    __CutsceneStart();
    __Actor_SetAnim(a, 6);
    __WaitFrames(6);
    __PlaySound(0x98);
    __Actor_SetAnim(a, 7);
    *(int *)(a + 0x30) = 0xc0 << 10;
    *(int *)(a + 0x34) = 0x80 << 10;
    *(int *)(a + 0x28) = 0x80 << 11;
    *f = *f & 0x7e;
    __Actor_SetSpriteFlags(a, 0);
    __Func_8092158(0, *(short *)((char *)v + 2), *(short *)((char *)v + 0xa));
    __Actor_SetAnim(a, 6);
    __Actor_SetSpriteFlags(a, 1);
    *f = r;
    __MapActor_SetAnim(0xa, 7);
    d = 0xffff0000;
    *(int *)(a + 0xc) += d;
    *(int *)(a + 0x14) += d;
    __WaitFrames(2);
    *(int *)(a + 0xc) += d;
    *(int *)(a + 0x14) += d;
    __WaitFrames(0xa);
    e = 0x80 << 9;
    *(int *)(a + 0xc) += e;
    *(int *)(a + 0x14) += e;
    __WaitFrames(4);
    *(int *)(a + 0xc) += e;
    *(int *)(a + 0x14) += e;
    *f = saved;
    __CutsceneEnd();
    return 1;
    }
    return 0;
}
