extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__MapActor_GetActor(int slot);
extern void __PlaySound(int id);
extern void __Actor_SetAnim(unsigned char *a, int n);

void OvlFunc_881_20097a4(void)
{
    unsigned char *a;
    unsigned char *b;
    unsigned char *p;
    int lim;
    int v;

    a = __MapActor_GetActor(0xf);
    b = __MapActor_GetActor(0xe);
    *(int *)(a + 8) = *(int *)(b + 8);
    lim = 0xa0 << 12;
    *(int *)(a + 0x10) = *(int *)(b + 0x10);
    if (*(int *)(a + 0xc) < lim) {
        v = 0xa0 << 12;
        *(int *)(a + 0xc) = v;
        if (__GetFlag(0x80 << 2) == 0) {
            __PlaySound(0x91);
            __Actor_SetAnim(a, 3);
            __SetFlag(0x80 << 2);
            p = a + 0x64;
            *(short *)p = 1;
        }
    }
}
