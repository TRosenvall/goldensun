extern void *__MapActor_GetActor(int slot);
extern void __vec3_translate(unsigned int a, unsigned int b, int *c);
extern int __TestCollision(void *a, int *v);
extern void __ClearFlag(int id);
extern int OvlFunc_969_20086c0(void);
extern void __Actor_SetAnim(void *a, int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);
extern void __Func_8092158(int a, int b, int c);

void OvlFunc_969_2008518(void)
{
    unsigned char *a;
    unsigned char *fl;
    int saved;
    unsigned int dir;
    int v[3];
    int *p;

    a = (unsigned char *)__MapActor_GetActor(0);
    dir = (*(unsigned short *)(a + 6) + 0x1000) & 0xe000;
    fl = a + 0x55;
    saved = *fl;
    p = v;
    p[0] = (*(int *)(a + 8) & 0xfff00000) + 0x80000;
    p[1] = *(int *)(a + 0xc);
    p[2] = (*(int *)(a + 0x10) & 0xfff00000) + 0x80000;
    __vec3_translate(0x200000, dir, p);
    if (__TestCollision(a, p) == 0) {
        __ClearFlag(0x94 << 2);
        OvlFunc_969_20086c0();
        __Actor_SetAnim(a, 6);
        __WaitFrames(6);
        __Actor_SetAnim(a, 7);
        *(int *)(a + 0x30) = 0xc0 << 10;
        *(int *)(a + 0x34) = 0x80 << 10;
        __PlaySound(0x98);
        *(int *)(a + 0x28) = 0x80 << 11;
        *fl &= 0x7e;
        __Actor_SetSpriteFlags(a, 0);
        __Func_8092158(0, *(short *)((char *)p + 2), *(short *)((char *)p + 0xa));
        __Actor_SetAnim(a, 6);
        __Actor_SetSpriteFlags(a, 1);
        *fl = saved;
    }
}
