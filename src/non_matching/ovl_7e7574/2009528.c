/* OvlFunc_959_2009528 -- NON-MATCHING at -O2: 132 encodings of 133 differ (ours 137).
 * EXACT UNDER CSE_CFLAGS (-fno-rerun-cse-after-loop): 296 bytes, 133 encodings and
 * 13 relocations identical.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/ovl_7e7574/2009528.c asm/overlays/rom_7e7574/ovl_9dc_c_a_a_c_a_c.s --func OvlFunc_959_2009528
 * (the exact result needs -fno-rerun-cse-after-loop added to objcmp's flags).
 *
 * At -O2 the flag id 0x214 is commoned into r8 and costs an extra push. Giving each
 * of its four uses its own local is inert (all 15 combinations score 136). NOT LANDED:
 * it needs a per-file CSE_CFLAGS Makefile row, which docs/elevation.md admits only on
 * "the spellings provably cannot differ" -- the same decision as ColorCycleVFXPalette
 * (batch 286). Its .s holds only this function, so it would convert whole.
 *
 * Levers that got it exact under the flag: the gState tests as an if/else, each arm with
 * its own `g = gState;` and the second arm `goto check` (the ROM's threaded shape); 0x2092
 * through an `int w` assigned before the inner test; `p = b + 0x182; v = 0x5d; *p = v;`
 * to stop cross-jumping sinking an add into the shared strh tail.
 *
 * COMPILER CRASH NOTE: under -fno-rerun-cse-after-loop, gcc-2.96 ICEs (decode_rtx_const,
 * varasm.c:3421) on `off = 0x93 << 2; *(short *)(gState + off)` -- REG_EQUIV notes carry
 * a doubly wrapped (const (const (plus gState 588))). Use per-arm `g = gState` locals.
 */
struct Actor {
    unsigned char pad00[8];
    int f8;
    int fc;
    int f10;
    unsigned char pad14[0x5b - 0x14];
    unsigned char f5b;
};

struct S {
    unsigned char pad00[0x18];
    int f18;
    int f1c;
    int f20;
    int f24;
};

extern unsigned char gState[];
extern unsigned char *iwram_3001e70[];
extern volatile int iwram_3001e40;
extern struct Actor *__MapActor_GetActor(int slot);
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern unsigned int OvlFunc_959_20094cc(void);
extern int OvlFunc_959_2009918(int slot);
extern unsigned int OvlFunc_959_20098e4(unsigned int slot);

void OvlFunc_959_2009528(void)
{
    struct Actor *a;
    struct S *s;
    unsigned char *b;
    unsigned char *g;
    int v;
    unsigned short *p;
    int w;

    a = __MapActor_GetActor(0x12);
    s = (struct S *)(iwram_3001e70[0] + (0xb2 << 1));
    b = iwram_3001e70[0x13];
    if (iwram_3001e40 & 1) {
        s->f18 = 1;
        s->f1c = 1;
    } else {
        s->f18 = -1;
        s->f1c = -1;
    }
    if (__GetFlag(0x83 << 1) != 0 || *(short *)(b + (0xbf << 1)) != 0
        || *(short *)(b + (0xc0 << 1)) != 0) {
        a->f5b = 1;
        return;
    }
    if (__GetFlag(0x85 << 2) != 0)
        return;
    a->f5b = 0;
    if (__GetFlag(0x85 << 2) == 0 && a->f5b == 0) {
        s->f20 = (0xbc << 18) - a->f8;
        s->f24 = (0xf8 << 17) - a->f10;
    }
    if (OvlFunc_959_20094cc() != 0)
        return;
    if (OvlFunc_959_2009918(0x12) != 0) {
        w = 0x2092;
        g = gState;
        if (*(short *)(g + (0x93 << 2)) != 0) {
            *(unsigned short *)(b + (0xbf << 1)) = w;
            return;
        }
    } else {
        g = gState;
        if (*(short *)(g + (0x93 << 2)) != 0)
            goto check;
    }
    if (OvlFunc_959_20098e4(0x12) != 0) {
        __SetFlag(0x215);
        __SetFlag(0x85 << 2);
    }
check:
    if (__GetFlag(0x85 << 2) != 0) {
        p = (unsigned short *)(b + (0xc1 << 1));
        v = 0x5d;
        *p = v;
    }
}
