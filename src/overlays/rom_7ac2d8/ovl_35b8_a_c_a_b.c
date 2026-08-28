extern unsigned char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned char gScript_924__0200de14[];
extern unsigned char *__CreateActor(int kind, int x, int y, int z);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);
extern void __Sprite_SetAnim(unsigned char *s, int n);
extern void __PlaySound(int id);

void OvlFunc_924_200d388(void)
{
    unsigned char *g;
    unsigned char *e;
    unsigned char *a;
    unsigned char *s;
    unsigned char *q;
    unsigned char *p;
    int off;
    int z;
    int m;

    g = iwram_3001ebc;
    p = gState;
    p += 0xfa << 1;
    off = *(int *)p << 2;
    off += 0x14;
    e = *(unsigned char **)(g + off);
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc), *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_924__0200de14);
        q = a + 0x55;
        z = 0;
        *q = z;
        q += 0xf;
        *(short *)q = z;
        *(unsigned char **)(a + 0x68) = e;
        if (s != 0) {
            __Sprite_SetAnim(s, 2);
            s[0x26] = 0;
            m = -13;
            m &= s[9];
            m |= 4;
            s[9] = m;
        }
    }
    a = __CreateActor(0x1a, *(int *)(e + 8), *(int *)(e + 0xc), *(int *)(e + 0x10));
    if (a != 0) {
        *(int *)(a + 0x14) = *(int *)(e + 0x14);
        s = *(unsigned char **)(a + 0x50);
        __Actor_SetScript(a, gScript_924__0200de14);
        q = a + 0x55;
        z = 0;
        *q = z;
        q += 0xf;
        *(short *)q = z;
        *(unsigned char **)(a + 0x68) = e;
        a[0x23] = 2;
        if (s != 0) {
            __Sprite_SetAnim(s, 1);
            s[0x26] = 0;
        }
    }
    __PlaySound(0x82);
}
