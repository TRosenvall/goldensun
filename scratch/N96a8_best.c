struct Ent {
    int f0;
    int f4;
    int f8;
    short fc;
    short fe;
    short f10;
    short f12;
    short f14;
    unsigned char pad16[2];
};

struct Hdr {
    unsigned char pad00[4];
    int f4;
    int f8;
    int fc;
    unsigned char pad10[0x40 - 0x10];
    int f40;
    int f44;
    int f48;
    int f4c;
};

extern unsigned char L205a[] __asm__(".L205a");
extern unsigned char L205e[] __asm__(".L205e");
extern unsigned short L2062[] __asm__(".L2062");
extern struct Hdr L2070 __asm__(".L2070");
extern struct Ent L20d0[] __asm__(".L20d0");

extern unsigned char *__MapActor_GetActor(int slot);
extern void __Actor_SetAnim(unsigned char *a, int n);
extern int __StartTask(void *f, int pri);
extern void OvlFunc_951_2008e5c(void);

void OvlFunc_951_20096a8(void)
{
    struct Hdr *h;
    unsigned char *a;
    unsigned char *b;
    unsigned short *c;
    struct Ent *e;
    int i;
    int z;

    h = &L2070;
    a = L205a;
    e = L20d0;
    c = L2062;
    b = L205e;
    i = 0;
    z = 0;
    do {
        e->f0 = *a << 16;
        e->f8 = *b << 16;
        i += 1;
        e->f4 = z;
        e->fc = *c;
        e->fe = z;
        e->f10 = z;
        e->f12 = z;
        e->f14 = z;
        a += 1;
        b += 1;
        c += 1;
        e += 1;
    } while (i != 4);
    h->f4 = 0xffe20000;
    h->f8 = z;
    h->fc = 0xc8 << 15;
    h->f40 = z;
    h->f44 = z;
    h->f48 = z;
    h->f4c = z;
    __Actor_SetAnim(__MapActor_GetActor(0x14), 2);
    __Actor_SetAnim(__MapActor_GetActor(0x15), 2);
    __StartTask(OvlFunc_951_2008e5c, 0xc83);
}
