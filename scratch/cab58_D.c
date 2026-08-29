struct A {
    unsigned char pad00[0x64];
    unsigned short f64;
    unsigned short f66;
    int f68;
    void *f6c;
};

extern int iwram_3001e40;
extern int L5240[] __asm__(".L5240");
extern struct A *__CreateActor(int a, int b, int c, int d);
extern void __Actor_SetAnim(struct A *a, int anim);
extern void OvlFunc_932_200aa10(void);
extern void OvlFunc_932_200aa48(void);

void OvlFunc_932_200ab58(void)
{
    struct A *a;
    unsigned short *p;
    int t;

    if ((iwram_3001e40 & 3) == 0) {
        a = __CreateActor(0xde, L5240[0], L5240[1], L5240[2]);
        if (a != 0) {
            p = (unsigned short *)((char *)a + 0x64);
            t = 0x1e;
            *p = t;
            p += 1;
            t = 1;
            *p = t;
            a->f68 = 0x14;
            OvlFunc_932_200aa10();
            a->f6c = OvlFunc_932_200aa48;
            __Actor_SetAnim(a, 1);
        }
    }
}
