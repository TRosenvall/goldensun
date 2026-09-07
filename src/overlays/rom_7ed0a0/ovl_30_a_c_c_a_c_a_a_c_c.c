/* asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_a_c_c.s -- the file's only function.
 *
 *   OK OvlFunc_964_2009550 -- 420 bytes, 175 encodings and 31 relocations identical
 *
 * THE NEAR-TWIN of OvlFunc_964_2009744 in ovl_30_a_c_c_a_c_a_c_a_c.s, which
 * carries the measured lever table for both.  Written from that finished file
 * with the actor slots, the position/tile constants and the save bit
 * substituted, and with the __GetFlag guard dropped -- this one has none.
 * Screened EXACT on the first try; no lever was re-derived.
 *
 * Note the one spelling difference the constants force by themselves: 0x3020000
 * is not reachable by a shifted 8-bit value so the twin pools it, while this
 * function's 0x2e80000 is `mov r0, #0xba / lsl r0, #18`.  Both fall out of the
 * plain `0xba << 18` / `0x3020000` argument with no help.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    unsigned char pad10[0x23 - 0x10];
    unsigned char f23;
    unsigned char pad24[0x55 - 0x24];
    unsigned char f55;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    unsigned char pad10[0x28 - 0x10];
};

extern struct Actor *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __WaitFrames(int n);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __DeleteActor(void *a);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8012330(int a, int b, int c);
extern void __Func_8012350(void);
#define PIN1 register int q0 __asm__("r0")
#define PIN2 PIN1; register int q1 __asm__("r1")
#define PIN3 PIN2; register int q2 __asm__("r2")

extern void *OvlFunc_964_20089f4(int a, int b, int c, int d);
extern void OvlFunc_964_2008ae8(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

void OvlFunc_964_2009550(void)
{
    struct Cfg s;
    struct Cfg *c;
    struct Actor *a;
    struct Actor *a2;
    struct Actor *a3;
    struct Actor *a4;
    void *b;
    unsigned int i;
    int px;
    int pz;
    int e;
    int g;
    int h;

    __CutsceneStart();
    a = __MapActor_GetActor(0x12);
    if (a->x >> 20 == 0x2e) {
        __CutsceneWait(0x1e);
        b = OvlFunc_964_20089f4(0xba << 18, 0, 0xb8 << 16, 0xfd);
        c = &s;
        c->f08 = 0x9999;
        c->f0c = 0x9999;
        c->f04 = 7;
        a2 = __MapActor_GetActor(0x12);
        a2->f55 = 0;
        __PlaySound(0xb9);
        for (i = 0; i <= 0xf; i++) {
            __WaitFrames(3);
            a3 = __MapActor_GetActor(0x12);
            a3->y -= 0x10000;
            px = (__Random() * 0x10 >> 16) * 0x10000 + (0xb8 << 18);
            pz = (__Random() * 0x12 >> 16) * 0x10000 + (0x80 << 16);
            OvlFunc_964_2008ae8(px, 0, pz, 0, 0, 0, 0x90 << 12, c);
        }
        e = 8;
        __Func_8010704(0x33, 8, 1, 1, 0x31, e);
        __CutsceneWait(0x1e);
        a4 = __MapActor_GetActor(0x12);
        a4->f23 |= 2;
        __MapActor_SetAnim(0x12, 3);
        __DeleteActor(b);
        __Func_8010704(0x2d, 4, 1, 1, 0x2e, e);
        __MapActor_SetPos(0x14, 0xba << 18, 0x88 << 16);
        __PlaySound(0xbc);
        g = 1;
        h = 3;
        __CopyMapTiles(0x3a, 8, 0x31, 8, g, h);
        { PIN3; q0 = 0; q1 = 0xa0 << 11; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
        __CutsceneWait(0x14);
        __PlaySound(0xbc);
        __CopyMapTiles(0x3b, 8, 0x31, 8, g, h);
        { PIN3; q0 = 0; q1 = 0xa0 << 11; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
        { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
        __Func_8012350();
        __CutsceneWait(0xa);
        __SetFlag(0x971);
    }
    __CutsceneEnd();
}
