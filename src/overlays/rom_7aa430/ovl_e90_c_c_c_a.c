/* OvlFunc_923_2008fd8  --  0x02008fd8, was asm/overlays/rom_7aa430/ovl_e90_c_c_c_a.s
 * (this function alone), so it converts whole. Matched from scratch.
 *
 * FAKEMATCH -- the r3/r2 register pins on the last two stack arguments of
 * __CopyMapTiles / __Func_8010704, copied from its sibling OvlFunc_923_20092e0
 * (ovl_1150_c_c_c_a_a.c, already booked); booked in fakematch.txt. __StartTask
 * declared `int` took the last 2 (batch 286's callee-return-type lever).
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    int y;
    int z;
};

struct Cfg {
    int f00;
    int f04;
    int f08;
    int f0c;
    int f10;
    int f14;
    unsigned short f18;
    unsigned short f1a;
    void *f1c;
    int f20;
    void (*f24)(void);
};

extern unsigned char L2f4c[] __asm__(".L2f4c");

extern struct Actor *__MapActor_GetActor(int slot);
extern void __SetFlag(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __PlaySound(int id);
extern unsigned int __Random(void);
extern int __StartTask(void (*f)(void), int prio);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Func_8091200(int a, int b);
extern void __Func_8091254(int a);
extern void OvlFunc_923_2008d98(void);
extern void OvlFunc_common0_10c(int x, int y, int z, int a, int b, int c,
                                int d, struct Cfg *s);

void OvlFunc_923_2008fd8(void)
{
    struct Cfg s;
    unsigned int i;
    int x, z, t, v;
    int ax, az;

    ax = __MapActor_GetActor(0)->x / 0x100000;
    az = __MapActor_GetActor(0)->z / 0x100000;
    if (ax != 0xc || az != 0x20)
        return;
    __CutsceneStart();
    __Func_8091200(0x10000, 0);
    __Func_8091254(0x3c);
    __CutsceneWait(0x78);
    __Func_8091200(0x10005, 1);
    __Func_8091254(0x3c);
    __CutsceneWait(0x28);
    for (i = 0; i <= 0xe; i++) {
        s.f00 = 1;
        s.f18 = 0x11e;
        s.f1c = L2f4c;
        __PlaySound(0xf6);
        x = 0xd0 - ((__Random() << 4) >> 16);
        z = 0x230 - ((__Random() << 4) >> 16);
        t = (__Random() << 2) >> 16;
        v = ((t * 15 << 16) + (0xf0 << 14)) / 100;
        OvlFunc_common0_10c(x << 16, 0, z << 16, 0, v, 0, 0x320001, &s);
        __CutsceneWait(4);
    }
    __PlaySound(0xdc);
    __CutsceneWait(0x3c);
    __SetFlag(0x875);
    __StartTask(OvlFunc_923_2008d98, 0xc8 << 4);
    { register int a5 __asm__("r3"); register int a6 __asm__("r2");
      a5 = 5; a6 = 3; __CopyMapTiles(0x25, 0x62, 0xa, 0x61, a5, a6); }
    { register int a5 __asm__("r3"); register int a6 __asm__("r2");
      a5 = 6; a6 = 0x20; __Func_8010704(0x46, 0x20, 0xd, 7, a5, a6); }
    __Func_8091200(0x10000, 0);
    __Func_8091254(0x3c);
    __CutsceneWait(0x78);
    __CutsceneEnd();
}
