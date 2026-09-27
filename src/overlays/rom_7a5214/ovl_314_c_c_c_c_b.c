/* OvlFunc_918_200962c  --  0x0200962c, split out of
 * asm/overlays/rom_7a5214/ovl_314_c_c_c_c.s; the .data (gScript_918_...) that
 * followed stays in _c.s. Written from its sibling OvlFunc_917_20095a0's idiom:
 * `unsigned int q = t / 10;` with the bound inline as `i < 6 - q` (not a named n),
 * reused in `(0xb4 << 1) / (6 - q) * i`; literal zeros so loop.c hoists them after
 * the entry test.
 */
struct Sprite {
    unsigned char pad00[9];
    unsigned char b0 : 2,
                  b2 : 2,
                  b4 : 4;
};

struct Actor {
    unsigned char pad00[0x30];
    int f30;
    unsigned char pad34[0x38 - 0x34];
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0x50 - 0x44];
    struct Sprite *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0x64 - 0x56];
    short f64;
    short f66;
    unsigned char pad68[0x6c - 0x68];
    void *f6c;
};

extern int L2dc0[3] __asm__(".L2dc0");
extern int L2dcc __asm__(".L2dcc");

extern void __PlaySound(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern int __Func_8096c48(struct Sprite *s, int prev);
extern void __Actor_SetSpriteFlags(struct Actor *a, int n);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void OvlFunc_918_20095ac(struct Actor *a);

void OvlFunc_918_200962c(void)
{
    struct Actor *a;
    unsigned int i;
    int prev;
    int t;
    unsigned int q;

    t = L2dcc;
    prev = 0;
    q = t / 10;
    switch (t) {
    case 0:
    case 10:
    case 20:
    case 30:
    case 40:
        __PlaySound(0xdc);
        i = 0;
        for (; i < 6 - q; i++) {
            a = __CreateActor(0x11d, L2dc0[0], L2dc0[1], L2dc0[2]);
            if (a != 0) {
                prev = __Func_8096c48(a->f50, prev);
                a->f55 = 0;
                a->f50->b2 = 0;
                __Actor_SetSpriteFlags(a, 0);
                __Actor_SetAnim(a, 1);
                a->f64 = 0;
                a->f66 = (((0xb4 << 1) / (6 - q) * i) << 16) / (0xb4 << 1);
                a->f38 = L2dc0[0];
                a->f3c = L2dc0[1];
                a->f40 = L2dc0[2];
                a->f30 = 0x19999;
                a->f6c = OvlFunc_918_20095ac;
            }
        }
    case 44:
        __PlaySound(0x121);
        break;
    }
    L2dcc += 1;
    if (L2dcc > 0x78)
        L2dcc = 0;
}
