/* OvlFunc_947_2008ddc  --  0x02008ddc, was
 * asm/overlays/rom_7d0e88/ovl_314_c_a_a_a_a.s (this function alone), so it
 * converts whole. Matched from scratch.
 *
 * `s->f8 += A << 16; s->f10 += B << 16; s->f8 >>= 20; s->f10 >>= 20;` -- gcc
 * drops the doubled f10 store and the two-address add lands in the field's
 * register (8 differing -> exact).
 */
struct S {
    unsigned int f0;
    int f4;
    int f8;
    int fc;
    int f10;
};

struct T {
    int a;
    int b;
    int c;
    int d;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x50 - 0x14];
    unsigned char *f50;
};

extern unsigned char *iwram_3001e70;
extern int L2ce0[] __asm__(".L2ce0");
extern struct T gScript_884__0200acf8[];
extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_947_2008ddc(int slot, int *pa, int *pb, struct S *s, int *pc, int *pd)
{
    unsigned char *e;
    struct Actor *a;
    unsigned int i;
    int x, y;

    e = iwram_3001e70;
    a = __MapActor_GetActor(slot);
    for (i = 0; i <= 5; i++) {
        if (**(short **)(a->f50 + 0x28) == L2ce0[i]) {
            s->f0 = i;
            break;
        }
        s->f0 = 7;
    }
    if (s->f0 > 6)
        return 0;
    s->f8 = a->f8;
    s->fc = a->fc;
    s->f10 = a->f10;
    x = gScript_884__0200acf8[s->f0].b;
    if (x < 0)
        x = -x;
    y = gScript_884__0200acf8[s->f0].d;
    if (y < 0)
        y = -y;
    *pb = (x + y) >> 4;
    x = gScript_884__0200acf8[s->f0].a;
    if (x < 0)
        x = -x;
    y = gScript_884__0200acf8[s->f0].c;
    if (y < 0)
        y = -y;
    *pa = (x + y) >> 4;
    s->f8 += gScript_884__0200acf8[s->f0].a << 16;
    s->f10 += gScript_884__0200acf8[s->f0].b << 16;
    s->f8 >>= 20;
    s->f10 >>= 20;
    *pc = *(int *)(e + (0x9e << 1)) >> 20;
    *pd = *(int *)(e + (0xa0 << 1)) >> 20;
    return 1;
}
