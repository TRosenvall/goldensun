extern int L6968 __asm__(".L6968");

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern unsigned char *__galloc_iwram(int a, int b);
extern void __gfree(int a);
extern void __LoadItemIcon(int id);
extern void __UploadSpriteGFX(int a, int b, void *p);

void *OvlFunc_945_200c5d0(void)
{
    unsigned char *act;
    unsigned char *o;
    unsigned char *buf;
    int f;
    int c1;
    int c2;
    int c3;
    int g1;
    int m;

    c1 = 0x80 << 11;
    c2 = 0x88 << 18;
    c3 = 0x1c70000;
    g1 = 0xc1 << 3;
    m = -0x21;
    f = __GetFlag(0x80 << 2);
    if (f != 0)
        return (void *)L6968;
    act = __CreateActor(0x16, c3, c1, c2);
    act[0x55] = f;
    act[0x5c] = 1;
    o = *(unsigned char **)(act + 0x50);
    o[0x27] = f;
    o[5] &= m;
    o[9] &= 0xf;
    buf = __galloc_iwram(0x11, g1);
    __LoadItemIcon(0xe8);
    buf += 0x80 << 3;
    __UploadSpriteGFX(o[0x1c], 0x80, buf);
    __gfree(0x11);
    __SetFlag(0x80 << 2);
    L6968 = (int)act;
    return act;
}
