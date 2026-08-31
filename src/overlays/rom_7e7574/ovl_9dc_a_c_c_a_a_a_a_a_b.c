extern unsigned char *__MapActor_GetActor(int slot);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __PlaySound(int id);
extern void __SetFlag(int id);
extern void __Actor_SetSpriteFlags(void *a, int f);

void OvlFunc_959_2008bec(void)
{
    unsigned char *a;
    unsigned char *b;
    int s1;
    int s2;
    int s3;
    int m1;
    int m2;
    int t;
    int e;

    s1 = 0x80 << 11;
    s2 = 0x80 << 11;
    s3 = 0x80 << 9;
    m1 = -1;
    m2 = -1;
    e = 0xe666;
    a = __MapActor_GetActor(0xc);
    if (*(int *)(a + 0x10) >> 20 > 0x16) {
        __Func_8012330(s1, s2, s3);
        __Func_8012330(m1, m2, e);
        __PlaySound(0x90);
        t = 0xf;
        __Func_8010704(0xf, 0x14, 1, 1, t, 0x16);
        __Func_8010704(0x11, 0x17, 1, 3, t, 0x17);
        b = __MapActor_GetActor(0xc);
        if (b != 0) {
            __Actor_SetSpriteFlags(b, 0);
            b[0x23] = 2;
        }
        __SetFlag(0x943);
    }
}
