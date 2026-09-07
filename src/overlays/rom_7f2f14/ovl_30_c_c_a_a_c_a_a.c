/* asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.s -- all three functions.
 *
 *   OK OvlFunc_968_200a6f8 -- 532 bytes, 231 encodings and 25 relocations identical
 *   OK OvlFunc_968_200a90c -- 520 bytes, 225 encodings and 24 relocations identical
 *   OK OvlFunc_968_200ab14 -- 976 bytes, 431 encodings and 52 relocations identical
 *
 * 200a6f8 and 200a90c are near-twins: the same sanctum shard cutscene on save
 * bits 0x306 and 0x307.  200a90c was written from the finished 200a6f8 with the
 * constants changed and screened EXACT first try, so every lever below was paid
 * for once.
 *
 * TWO STRUCT TAGS FOR ONE OBJECT.  `struct MapEnt` reads the actor's position
 * through the SHORT at +0xa and +0x12 -- the integer halves of the 16.16 words
 * `struct Actor` calls f08 and f10 -- so the two layouts overlap and cannot be
 * unified.  `__MapActor_GetActor` is therefore declared `void *` once and each
 * caller assigns it into its own pointer type; the casts are free.
 *
 * LEVERS, all measured (differing encodings against the ROM):
 *
 *   200a6f8, from 137 differing:
 *     int locals for the two `ldrsh` reads         137 -> 123  (without them the
 *       range fold stays in HImode and gcc emits ldrh + lsl #16)
 *     PIN3 on both __Func_8012330 call sites       (folded into the above)
 *     `register int n __asm__("r6")`                123 -> 116 ALONE, and it is
 *       what puts the whole callee-saved set right: p->r7, i->r8, z->r9, t->r10
 *     `for (i = 0; i <= 0x13f; i++, t--)`          fixes the loop-bottom pair
 *     `f = 4;` -- a LOCAL for CopyMapTiles' 6th
 *       argument, so both stack-argument constants
 *       are live at once                            13 -> 8
 *     u -> r6 + v -> r2 pins ADDED AS A SET         8 -> 6   (either alone: 8)
 *     `register unsigned int w __asm__("r3")` for
 *       the range temp, so the add is 3-operand     6 -> 4
 *     `a->fc = 0` after the two loads                4 -> EXACT
 *
 *   200ab14, from 71 differing on the first draft (443 lines against 443):
 *     unsigned s/j/k -- the ROM's loop tests are
 *       bhi/bls, so the counters are unsigned        71 -> 51
 *     ONE loop variable for all four 0xf..0x12
 *       loops AND the table search                   41 -> 28
 *     a separate local pair per __Func_8010704
 *       stack-argument site                          28 -> 19
 *     `register int e4 __asm__("r3")` +
 *       `register int f4 __asm__("r2")`              19 -> 3
 *     `void` (not `int`) return on __Func_8010560     3 -> EXACT
 *
 *   MEASURED INERT on 200ab14: L5164 as `[][2]` / flat `[]` / a struct array
 *   (13 each); every one of the 23 non-identity orderings of the four
 *   f22/f55/f48/f44 stores (19..238, none better); pinning the __Func_8010560
 *   argument registers.
 */
struct MapEnt {
    unsigned char pad00[0xa];
    short fa;
    int fc;
    unsigned char pad10[2];
    short f12;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x22 - 0x10];
    unsigned short f22;
    unsigned char pad24[4];
};

extern unsigned char *iwram_3001e70;
extern unsigned char *iwram_3001ebc;
extern unsigned int iwram_3001e40;

extern void *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
extern void __Func_8091e9c(int n);
extern unsigned int __Random(void);
extern void OvlFunc_968_2008118(int a, int b, int c, int d,
                                int e, int f, int g, struct Cfg *s);

#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")
#define PIN4 PIN3; register int q3 __asm__("r3")

struct Actor {
    int f00;
    unsigned char pad04[4];
    int f08;
    int f0c;
    int f10;
    unsigned char pad14[0x22 - 0x14];
    unsigned char f22;
    unsigned char f23;
    unsigned char pad24[0x44 - 0x24];
    int f44;
    int f48;
    unsigned char pad4c[4];
    unsigned char *f50;
    unsigned char pad54[1];
    unsigned char f55;
    unsigned char pad56[3];
    unsigned char f59;
    unsigned char pad5a[0x63 - 0x5a];
    unsigned char f63;
};

extern unsigned int L5164[][2] __asm__(".L5164");
extern unsigned char L577c[] __asm__(".L577c");
extern unsigned char L5d3c[] __asm__(".L5d3c");
extern unsigned char gScript_968__0200d7c8[];
extern unsigned char gScript_968__0200dac8[];

extern int __SetFlag(int id);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8010560(void *s, int a, int b);
extern void __Func_8092b08(int a, int b);
extern void __Actor_SetSpriteFlags(struct Actor *a, int f);
extern void __Actor_WaitScript(struct Actor *a);
extern void __Actor_SetScript(struct Actor *a, void *s);
extern void __DeleteActor(void *a);
extern void __Func_80933d4(int a, int b);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80933f8(int a, int b, int c, int d);
extern void *OvlFunc_968_2008098(int a, int b, int c, int d);
extern void OvlFunc_968_200894c(struct Actor *a);
extern struct Actor *OvlFunc_968_2008c5c(int a, int b, void *s);

void OvlFunc_968_200a6f8(void)
{
    struct Cfg s;
    struct MapEnt *a;
    register int u __asm__("r6");
    register unsigned int w __asm__("r3");
    register int v __asm__("r2");
    unsigned char *p;
    unsigned int i;
    int z;
    int t;
    register int n __asm__("r6");
    int f;

    p = iwram_3001e70 + (0xb2 << 1);
    a = __MapActor_GetActor(0);
    u = a->fa;
    v = a->f12;
    a->fc = 0;
    w = u - 0x214;
    if (w <= 7 && v >= 0xa2 << 1 && v < 0xa6 << 1) {
        a->fc = 0xfffe0000;
        if (!__GetFlag(0x306)) {
            __CutsceneStart();
            __CopyMapTiles(0x3f, 0x1d, 0x21, 0x14, 1, 1);
            __PlaySound(0xa1);
            __CopyMapTiles(0x2c, 0x53, 0x2c, 0x50, 3, 3);
            __CutsceneWait(0x1e);
            { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0xef);
            __CutsceneWait(0x14);
            z = 0x9a << 18;
            n = 0;
            t = 0x3c;
            for (i = 0; i <= 0x13f; i++, t--) {
                *(int *)(p + 8) += 0x3333;
                if (z > 0x23fffff && i > 0x28) {
                    z -= 0x3333;
                    s.f00 = 2;
                    s.f08 = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                    s.f0c = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                    s.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
                    OvlFunc_968_2008118(z, 0, 0x90 << 17, 0,
                                        -((iwram_3001e40 & 1) * 3 << 16), 0,
                                        0x8a << 16, &s);
                    if (t == 0) {
                        t = 0x28;
                        n += 4;
                        f = 4;
                        __CopyMapTiles(n, 0x38, 0x24, 0x11, 3, f);
                    }
                }
                __WaitFrames(1);
            }
            *(int *)(p + 8) += 0x80 << 8;
            *(int *)(p + 8) = *(int *)(p + 8) / 0x10000 * 0x10000;
            __PlaySound(0x90 << 1);
            __PlaySound(0xbc);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __Func_8091e9c(0x12);
            __CutsceneEnd();
        }
    }
}

void OvlFunc_968_200a90c(void)
{
    struct Cfg s;
    struct MapEnt *a;
    register int u __asm__("r6");
    register unsigned int w __asm__("r3");
    register int v __asm__("r2");
    unsigned char *p;
    unsigned int i;
    int z;
    int t;
    register int n __asm__("r6");
    int f;

    p = iwram_3001e70 + (0xb2 << 1);
    a = __MapActor_GetActor(0);
    u = a->fa;
    v = a->f12;
    a->fc = 0;
    w = u - 0x314;
    if (w <= 7 && v >= 0xa2 << 1 && v < 0xa6 << 1) {
        a->fc = 0xfffe0000;
        if (!__GetFlag(0x307)) {
            __CutsceneStart();
            __CopyMapTiles(0x3f, 0x1d, 0x31, 0x14, 1, 1);
            __PlaySound(0xa1);
            __CutsceneWait(0x1e);
            { PIN3; q0 = 0x80 << 9; q1 = 0x80 << 9; q2 = 0x80 << 9;
              __Func_8012330(q0, q1, q2); }
            __PlaySound(0xef);
            __CutsceneWait(0x14);
            z = 0xb0 << 18;
            n = 0x3d;
            t = 0x3c;
            for (i = 0; i <= 0x13f; i++, t--) {
                *(int *)(p + 8) -= 0x3333;
                z += 0x3333;
                if (z >= 0xb2 << 18 && z < 0xbc << 18) {
                    s.f00 = 2;
                    s.f08 = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                    s.f0c = (__Random() * 3 >> 16) * 0x3333 + 0xcccc;
                    s.f22 = (__Random() * 0x1000 >> 16) + (0xf8 << 8);
                    OvlFunc_968_2008118(z, 0, 0x90 << 17, 0,
                                        -((iwram_3001e40 & 1) * 3 << 16), 0,
                                        0x8a << 16, &s);
                    if (t == 0) {
                        t = 0x28;
                        n -= 4;
                        f = 4;
                        __CopyMapTiles(n, 0x38, 0x2c, 0x11, 3, f);
                    }
                }
                __WaitFrames(1);
            }
            *(int *)(p + 8) += 0x80 << 8;
            *(int *)(p + 8) = *(int *)(p + 8) / 0x10000 * 0x10000;
            __PlaySound(0x90 << 1);
            __PlaySound(0xbc);
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666;
              __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            *(int *)(iwram_3001ebc + (0xe0 << 1)) = 0x202;
            __Func_8091e9c(0x13);
            __CutsceneEnd();
        }
    }
}

void OvlFunc_968_200ab14(void)
{
    struct Actor *e;
    struct Actor *o;
    struct Actor *c1;
    struct Actor *c2;
    void *n;
    struct Actor *b;
    unsigned int s;
    unsigned int j;
    unsigned int k;
    int d;
    int e1, f1, e2, f2, e3, f3, e5, f5, e6, f6, e7, f7;
    register int e4 __asm__("r3");
    register int f4 __asm__("r2");

    n = 0;
    b = __MapActor_GetActor(0);
    __CutsceneStart();
    e1 = 5;
    f1 = 0x30;
    __Func_8010704(0x45, 0x30, 4, 2, e1, f1);
    e2 = 9;
    f2 = 0x25;
    __Func_8010704(0x49, 0x25, 9, 0xd, e2, f2);
    for (s = 0xf; s <= 0x12; s++) {
        e = __MapActor_GetActor(s);
        if (e->f23 != 2) {
            e3 = e->f08 >> 20;
            f3 = e->f10 >> 20;
            __Func_8010704(0x48, 0x30, 1, 1, e3, f3);
        } else {
            e7 = e->f08 >> 20;
            f7 = e->f10 >> 20;
            __Func_8010704(0x49, 0x30, 1, 1, e7, f7);
        }
        k = 8;
        for (j = 0; j <= 7; j++) {
            if ((e->f08 >> 20) == L5164[j][0] && (e->f10 >> 20) == L5164[j][1]
                && e->f0c >= 0) {
                k = j;
                break;
            }
        }
        if (k == 8)
            continue;
        for (j = 0xf; j <= 0x12; j++) {
            o = __MapActor_GetActor(j);
            if (s != j && (e->f08 >> 20) == (o->f08 >> 20)
                && (e->f10 >> 20) == (o->f10 >> 20)) {
                k = 8;
                break;
            }
        }
        if (k == 8)
            continue;
        d = ((unsigned int)b->f50[9] << 28) >> 30;
        if ((b->f10 >> 20) <= L5164[k][1]) {
            n = OvlFunc_968_2008098(e->f08, e->f0c, e->f10 - 0x40000, 0x14);
            __Func_8092b08(0, 3);
        }
        for (j = 0xf; j <= 0x12; j++) {
            o = __MapActor_GetActor(j);
            if (s != j && (e->f08 >> 20) == (o->f08 >> 20)
                && (e->f10 >> 20) - 1 == (o->f10 >> 20))
                __Func_8092b08(j, 3);
        }
        __Actor_SetSpriteFlags(__MapActor_GetActor(s), 0);
        e->f22 = 0;
        e->f55 = 3;
        e->f48 = 0x1999;
        e->f44 = 0;
        e4 = L5164[k][0];
        f4 = L5164[k][1];
        __Func_8010704(6, 0x2c, 1, 1, e4, f4);
        OvlFunc_968_200894c(e);
        __PlaySound(0xbc);
        e->f59 = 0;
        e->f55 = 0;
        e->f0c = 0xfff00000;
        __Func_8092b08(s, 3);
        e->f23 = 2;
        e5 = L5164[k][0];
        f5 = L5164[k][1];
        __Func_8010704(0x49, 0x30, 1, 1, e5, f5);
        __Func_8092b08(0, d);
        ((struct Actor *)__MapActor_GetActor(0))->f23 |= 1;
        for (j = 0xf; j <= 0x12; j++) {
            o = __MapActor_GetActor(j);
            if (s != j && (e->f08 >> 20) == (o->f08 >> 20)
                && (e->f10 >> 20) - 1 == (o->f10 >> 20)) {
                __Func_8092b08(j, 1);
                ((struct Actor *)__MapActor_GetActor(j))->f23 |= 1;
            }
        }
        __DeleteActor(n);
        if (__GetFlag(0xc2 << 2)) {
            __CutsceneEnd();
            return;
        }
        if (((((struct Actor *)__MapActor_GetActor(0xf))->f23 & ((struct Actor *)__MapActor_GetActor(0x10))->f23
              & ((struct Actor *)__MapActor_GetActor(0x11))->f23 & ((struct Actor *)__MapActor_GetActor(0x12))->f23) & 2) == 0)
            continue;
        __Func_80933d4(0x80 << 9, 0x80 << 6);
        __Func_8093500(0xe, 1);
        __Func_8093530();
        c1 = OvlFunc_968_2008c5c(0x88, 0xc2 << 2, L577c);
        __CutsceneWait(0x1e);
        __Func_80933d4(0x6666, 0xccc);
        __Func_80933f8(0xd8 << 16, -1, 0x9e << 18, 1);
        __Actor_WaitScript(c1);
        __Actor_SetScript(c1, gScript_968__0200d7c8);
        c2 = OvlFunc_968_2008c5c(0xd8, 0xbe << 2, gScript_968__0200dac8);
        while (c1->f00 != 0 || c2->f00 != 0) {
            if (c1->f63 != 0 || c2->f63 != 0) {
                __CutsceneWait(0x1e);
                __Func_8010560(L5d3c, 0x4d, 0x23);
                e6 = 0xd;
                f6 = 0x24;
                __Func_8010704(0xd, 0x23, 1, 1, e6, f6);
                __SetFlag(0xc2 << 2);
                break;
            }
            __WaitFrames(1);
        }
    }
    __CutsceneEnd();
}
