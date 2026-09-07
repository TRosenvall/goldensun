/* asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_c_a_c_a_c.s -- the file's only function.
 *
 *   OK OvlFunc_964_2009744 -- 436 bytes, 180 encodings and 32 relocations identical
 *
 * A NEAR-TWIN of OvlFunc_964_2009550 in ovl_30_a_c_c_a_c_a_a_c_c.s: the same
 * summon cutscene on a different actor pair.  That one was written from this
 * finished file with the constants substituted and screened EXACT first try, so
 * every lever below was paid for once.  Its only structural difference is that
 * it has no __GetFlag guard.
 *
 * The struct layouts and the __Func_8012330 PIN3 idiom come from
 * src/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.c; the `struct Cfg *c = &s`
 * pointer local from src/non_matching/ovl_7ed0a0/20090c4.c.
 *
 * LEVERS, all measured (differing encodings against the ROM), from 190:
 *
 *   PIN3 on ALL FOUR __Func_8012330 sites,
 *     ADDED AS A SET                              190 -> 144, and it is what
 *     fixes the PUSH MASK: unaided gcc commons 0xa0<<11, 0x80<<9, 0xe666, -1,
 *     1 and 3 into r8/r9/r10/r11 where the ROM holds exactly one high register.
 *     The set is not decomposable -- see the table below.
 *     It also reproduces BOTH of the ROM's two DIFFERENT neg/pool interleaves
 *     (`neg r1 / ldr r2 / neg r0` at the first site, `neg r0 / neg r1 / ldr r2`
 *     at the second) with no per-site spelling.
 *
 *   a separate `struct Actor *` per __MapActor_GetActor
 *     site -- a2 and a4, NOT one reused `a`       144 -> 5
 *     Only the two sites whose field offset is beyond the load/store immediate
 *     (+0x55 and +0x23, which must build the address with a destructive
 *     `add r0, #imm`) need their own local.  Merging the +8 and +0xc sites back
 *     into one shared `a` is BYTE-IDENTICAL; merging the +0x55 and +0x23 sites
 *     costs one `mov rN, r0` each and goes straight back to 144.
 *
 *   the loop counter bumped AFTER the call                5 -> EXACT
 *     (`for (i = 0; i <= 0xf; i++)`; a `do {} while (i <= 0xf)` with `i++`
 *     written after the call is byte-identical.)
 *
 *   MEASURED WORSE/INERT: `i++` before the call, 5 differing.  Every one-pin
 *   subset of the PIN3 set -- see the table.
 *
 * THE PIN SET IS NOT DECOMPOSABLE.  Measured on two bases:
 *
 *   |                    | on the first draft | on the finished base |
 *   |--------------------|--------------------|----------------------|
 *   | no pins            | 190                | 188                  |
 *   | (0, a0<<11, 80<<9) | 183                | 181                  |
 *   | (-1, -1, 0xe666)   | 185                | 183                  |
 *   | **both**           | **144**            | **EXACT**            |
 *
 * A greedy one-at-a-time ADD pass sees at best 7 of 188 for either pin alone
 * and would report this function blocked.
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
extern int __GetFlag(int id);
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

void OvlFunc_964_2009744(void)
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
    a = __MapActor_GetActor(0x13);
    if (a->x >> 20 == 0x30) {
        if (__GetFlag(0x202)) {
            __CutsceneWait(0x1e);
            b = OvlFunc_964_20089f4(0x3020000, 0, 0x89 << 17, 0xdf);
            c = &s;
            c->f08 = 0x9999;
            c->f0c = 0x9999;
            c->f04 = 7;
            a2 = __MapActor_GetActor(0x13);
            a2->f55 = 0;
            __PlaySound(0xb9);
            for (i = 0; i <= 0xf; i++) {
                __WaitFrames(3);
                a3 = __MapActor_GetActor(0x13);
                a3->y -= 0x10000;
                px = (__Random() * 0x10 >> 16) * 0x10000 + (0xc0 << 18);
                pz = (__Random() * 0x12 >> 16) * 0x10000 + (0xe0 << 16);
                OvlFunc_964_2008ae8(px, 0, pz, 0, 0, 0, 0x90 << 12, c);
            }
            e = 0xe;
            __Func_8010704(0x33, 8, 1, 1, 0x2d, e);
            __CutsceneWait(0x1e);
            a4 = __MapActor_GetActor(0x13);
            a4->f23 |= 2;
            __MapActor_SetAnim(0x13, 3);
            __DeleteActor(b);
            __Func_8010704(0x2d, 4, 1, 1, 0x30, e);
            __MapActor_SetPos(0x15, 0xc2 << 18, 0xe8 << 16);
            __PlaySound(0xbc);
            g = 1;
            h = 3;
            __CopyMapTiles(0x3a, 8, 0x2d, 0xe, g, h);
            { PIN3; q0 = 0; q1 = 0xa0 << 11; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
            __CutsceneWait(0x14);
            __PlaySound(0xbc);
            __CopyMapTiles(0x3b, 8, 0x2d, 0xe, g, h);
            { PIN3; q0 = 0; q1 = 0xa0 << 11; q2 = 0x80 << 9; __Func_8012330(q0, q1, q2); }
            { PIN3; q0 = -1; q1 = -1; q2 = 0xe666; __Func_8012330(q0, q1, q2); }
            __Func_8012350();
            __CutsceneWait(0xa);
            __SetFlag(0x972);
        }
    }
    __CutsceneEnd();
}
