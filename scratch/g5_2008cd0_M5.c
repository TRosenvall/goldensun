extern unsigned char *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);
extern int __TestCollision(unsigned char *e, int *v);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Actor_SetAnim(unsigned char *e, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(unsigned char *e, int f);
extern void __Func_8092158(int a, int b, int c);

int OvlFunc_964_2008cd0(int *a)
{
    int v[3];
    int *p;
    unsigned char *e;
    unsigned char *f;
    int saved;
    int n0;
    int n1;

    e = __MapActor_GetActor(0);
    f = e + 0x55;
    saved = *f;
    p = v;
    p[0] = (*(int *)(e + 8) & 0xfff00000) + (0x80 << 12);
    p[1] = *(int *)(e + 0xc);
    p[2] = (*(int *)(e + 0x10) & 0xfff00000) + (0x80 << 12);
    n1 = *(unsigned short *)(e + 6) + (0x80 << 6);
    __vec3_translate(0x80 << 13, n1 & (0xc0 << 8), p);
    if (__TestCollision(e, p) != 1) {
    if (__TestCollision(e, a) == 0) {
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
    __Func_8092158(0, ((a[0] >> 20) << 4) + 8, ((a[2] >> 20) << 4) + 8);
    __Actor_SetAnim(e, 6);
    __Actor_SetSpriteFlags(e, 1);
    __WaitFrames(6);
    *f = saved;
    __CutsceneEnd();
    return 0;
    }
    }
    return 1;
}
