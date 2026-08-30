extern unsigned int iwram_3001e40;
extern unsigned char *__MapActor_GetActor(int slot);
extern unsigned int __Random(void);
extern void OvlFunc_common0_10c(int a, int b, int c, int d, int e, int f, int g, int *h);
extern void __Func_8092950(int a, int b);

void OvlFunc_926_200a5b8(void)
{
    int v[10];
    unsigned char *e;
    int m;
    int n;

    e = __MapActor_GetActor(0);
    m = 7;
    v[1] = m;
    if (((__Random() * 7 >> 16) & m) == 0)
        v[1] = 5;
    v[2] = 0xb333;
    v[3] = 0xcccc;
    n = (__Random() * 8 >> 16) * 13107;
    OvlFunc_common0_10c(*(int *)(e + 8) + ((8 - (int)(iwram_3001e40 & 0xf)) << 16),
                        *(int *)(e + 0xc) + (0xc0 << 13),
                        *(int *)(e + 0x10),
                        0, -n, 0, 0x90 << 12, v);
    if (iwram_3001e40 & 1)
        __Func_8092950(0, 0xf);
    else
        __Func_8092950(0, 1);
}
