extern unsigned int iwram_3001e40;
extern unsigned char gScript_932__0200c01c[];

extern unsigned int __Random(void);
extern void __PlaySound(int id);
extern unsigned char *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetSpriteFlags(unsigned char *a, int f);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern void __Actor_SetScript(unsigned char *a, unsigned char *s);

void OvlFunc_932_200abe0(void)
{
    unsigned char *a;
    unsigned char *p;
    unsigned int r;

    if (iwram_3001e40 % 3 != 0)
        return;
    r = ((__Random() * 3) << 4) >> 16 << 16;
    a = __CreateActor(0xc8, r + 0x2fd0000, 0xffc00000, 0x98 << 18);
    if (a == 0)
        return;
    if (iwram_3001e40 % 9 == 0) {
        if ((__Random() << 1) >> 16)
            __PlaySound(0x91);
        else
            __PlaySound(0x90);
    }
    a[0x55] = 0;
    r = ((__Random() << 15) >> 16) + 0x4ccc;
    *(int *)(a + 0x48) = 0x6666;
    a[0x61] = 1;
    *(int *)(a + 0x1c) = r;
    *(int *)(a + 0x18) = r;
    __Actor_SetSpriteFlags(a, 0);
    a[0x23] &= 0xfe;
    p = *(unsigned char **)(a + 0x50);
    p[9] = (p[9] & -13) | 4;
    __Actor_SetAnim(a, 1);
    __Actor_SetScript(a, gScript_932__0200c01c);
    *(int *)(a + 0x24) = ((((__Random() * 3) << 1) >> 16) - 3) << 16;
    *(int *)(a + 0x28) = 0x80 << 12;
    *(int *)(a + 0x2c) = (((__Random() * 3) << 9) >> 16) + 0xfffffd00;
}
