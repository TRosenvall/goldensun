/* OvlFunc_898_2009754 and OvlFunc_898_20097ac  --  0x02009754 / 0x020097ac, the
 * code of asm/overlays/rom_793768/ovl_314_c_c_c_c_c.s; its .data tail stays in
 * ovl_314_c_c_c_c_c_c.s. The split left the two functions in _a/_b pieces, merged
 * back into this one object. Matched from scratch (2009754 was a bonus
 * neighbour); verified from this combined file with objcmp --whole.
 *
 * - `(dx << 16) + g->x` in that operand order; `r = __Random() & 1; if (r == 1)`
 *   keeps the ROM's `cmp r0,#1`.
 * - FOLD PUTS CONSTANTS OUTERMOST: `r + K + x` becomes (r + x) + K, so where the
 *   ROM adds K first write `r = f() + K; r += x;`; `(r + K) * C` distributes into
 *   r*C + C unless r + K is a named unsigned temp.
 * - In 2009754, `a->f3c = a->y += ...` in each arm; a shared `a->f3c = a->y;`
 *   after the if/else reloads y (18 differing).
 */
struct Spr {
    unsigned char pad0[9];
    unsigned char f9_a : 2;
    unsigned char f9_b : 2;
    unsigned char pad0a[0x1c];
    unsigned char f26;
};

struct Actor {
    unsigned char pad0[8];
    int x;
    int y;
    int z;
    unsigned char pad14[0x1c];
    int f30;
    int f34;
    int f38;
    int f3c;
    int f40;
    unsigned char pad44[0xc];
    struct Spr *f50;
    unsigned char pad54;
    unsigned char f55;
    unsigned char pad56[0xe];
    short f64;
    unsigned char pad66[6];
    void (*f6c)(struct Actor *);
};

extern unsigned char gScript_898__0200a8c4[];
extern unsigned char gScript_898__0200a8dc[];

extern struct Actor *__MapActor_GetActor(int id);
extern struct Actor *__CreateActor(int id, int x, int y, int z);
extern unsigned int __Random(void);
extern void __Actor_SetAnim(struct Actor *a, int n);
extern void __Actor_SetScript(struct Actor *a, unsigned char *s);

void OvlFunc_898_2009754(struct Actor *a)
{
    a->x += a->f30;
    a->f38 = a->x;
    if (a->f64 != 0) {
        a->f3c = a->y += a->f34;
    } else {
        a->z += a->f34;
        a->f40 = a->z;
        a->f3c = a->y += 0x400;
    }
    a->f30 -= a->f30 / 28;
    a->f34 -= a->f34 / 28;
}

void OvlFunc_898_20097ac(int arg)
{
    struct Actor *g;
    struct Actor *a;
    struct Spr *s;
    int dx, dz;
    unsigned int r;

    g = __MapActor_GetActor(0x13);
    if (g == 0)
        return;
    dx = ((__Random() << 3) >> 16) - 4;
    dz = ((__Random() << 3) >> 16) - 4;
    a = __CreateActor(0xac, (dx << 16) + g->x, g->y, (dz << 16) + g->z);
    if (a == 0)
        return;
    s = a->f50;
    r = __Random() & 1;
    if (r == 1) {
        __Actor_SetAnim(a, 3);
        __Actor_SetScript(a, gScript_898__0200a8c4);
    } else {
        __Actor_SetAnim(a, 2);
        __Actor_SetScript(a, gScript_898__0200a8dc);
    }
    a->f55 = 0;
    if (arg & 2) {
        arg &= 1;
        r = __Random() % 10 + 5;
        r += (arg ^ 1) * 4;
        a->f34 = r * (arg * 0x3332 - 0x1999);
        a->f30 = (__Random() % 15 - 7) * 0x1999;
        a->f64 = 0;
    } else {
        a->f30 = (__Random() % 10 + 8) * (arg * 0x3332 - 0x1999);
        r = __Random() % 14 + 1;
        a->f34 = r * 0x1999;
        a->f64 = 1;
    }
    a->f6c = OvlFunc_898_2009754;
    s->f26 = 0;
    s->f9_b = g->f50->f9_b;
}
