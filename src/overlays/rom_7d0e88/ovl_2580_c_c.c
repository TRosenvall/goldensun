/* OvlFunc_947_200a74c -- 284 instructions, 736 bytes, 294 encodings and 68
 * relocations identical.  Stable over three repeat compiles, DEFAULT FLAGS.
 *
 * Its file's 816 bytes of .data across six .incbin blocks were REHOMED to
 * asm/overlays/rom_7d0e88/ovl_2580_c_c_dat.s and overlay.ld's .data line repointed --
 * behaviour-script and table data, well past docs/elevation.md's "a handful of readable
 * words" allowance.  All six labels were ALREADY .global, so no export had to be added,
 * and the rehome was gated byte-neutral on its own before this .c was written.
 *
 * `_AREA_74`, `_AREA_77`, `_AREA_79` and `_AREA_7a` are all already in area.sym.
 *
 * ================================================================
 * DROP THE SIBLING'S `off = 0;` SCAFFOLD -- IT IS NOT FREE, AND COPYING IT BLINDLY
 * COSTS 15 OF 294
 * ================================================================
 *
 * The batch-282 landing src/overlays/rom_7b7f1c/ovl_30_c_c_c_c_b.c writes
 * `base += off; off = 0; *(short *)((char *)base + off)`.  Thumb `ldrsh` is
 * register-offset-only, SO gcc GENERATES THAT ZERO ITSELF.  Supplying it as a NAMED
 * LOCAL makes it a CSE candidate that PRE hoists into the dominator block -- and the
 * body's own shared zero (three stores: f55 twice and f6c) then rides the same register
 * and moves with it, landing `mov r5, #0` before `cmp r2, r3`.
 *
 * Writing `*(short *)p` and letting gcc make its own zero: 0 of 294.  So that idiom is a
 * lever for the function it was found on, NOT a transcription convention -- and the same
 * rule was worth 235 -> 157 on this agent's OvlFunc_952_200c0b4, where the shared zero
 * was additionally being used as the `n > 0` compare operand.
 *
 * ONE COUNTER PER LOOP, ONE OFFSET VARIABLE PER REGION.  The two loops in the _AREA_79
 * arm live in DIFFERENT REGISTER CLASSES -- r1, caller-saved, for the call-free
 * bss_36d0 loop, and r5, callee-saved, for the loop that calls __MapActor_GetActor --
 * so sharing one `i` costs 10 of 294.  Sharing one `off` across the prologue and the
 * _AREA_7a arm stretched its live range across the whole function and pushed it from r2
 * to r5.
 *
 * AND THAT SECOND POINT CORRECTED A MISATTRIBUTION WORTH RECORDING: the gState pool-load
 * placement had been blamed on alias analysis, and `-fno-strict-aliasing` was thought to
 * be needed here.  It is not -- a separate `off2` for the second region fixed the
 * prologue AND the pool-load placement together.  Check live ranges before reaching for
 * an aliasing flag.
 *
 * `unsigned int` loop counters: both loops compare bls/bhi, and `int` gives ble/bgt.
 * Reverting costs 2 of 294.
 *
 * No per-file Makefile flag override applies to this stem.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern unsigned char *iwram_3001ebc;
extern int _AREA_74;
extern int _AREA_77;
extern int _AREA_79;
extern int _AREA_7a;

struct Spr {
    unsigned char pad00[9];
    unsigned char f9;
};

struct Actor {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[0x10 - 0xc];
    int f10;
    unsigned char pad14[0x18 - 0x14];
    int f18;
    int f1c;
    unsigned char pad20[0x50 - 0x20];
    struct Spr *f50;
    unsigned char pad54[0x55 - 0x54];
    unsigned char f55;
    unsigned char pad56[0x59 - 0x56];
    unsigned char f59;
    unsigned char pad5a[0x62 - 0x5a];
    unsigned char f62;
    unsigned char pad63[0x6c - 0x63];
    int f6c;
};

struct E {
    int f0;
    int f4;
    int f8;
    unsigned char padc[0x10 - 0xc];
    int f10;
};

extern struct E bss_36d0[];

extern int __GetFlag(int id);
extern struct Actor *__MapActor_GetActor(int slot);
extern void __MapActor_SetAnim(int slot, int anim);
extern void __Func_8092950(int a, int b);
extern void __Func_8091494(int a);
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __CopyMapTiles(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __CutsceneWait(int n);
extern void __StartTask(void (*f)(void), int n);
extern void OvlFunc_common0_70(int a, int b, int c, int d);
extern void OvlFunc_947_2008ba4(int slot);
extern void OvlFunc_947_2008ec8(int slot);
extern void OvlFunc_947_200a694(int slot);
extern void OvlFunc_947_200a5f8(int slot);
extern void OvlFunc_947_200a63c(int slot);
extern void OvlFunc_947_200a09c(void);
extern void OvlFunc_947_2009d84(void);
extern void OvlFunc_947_200a4cc(void);
extern void OvlFunc_947_200a6b8(void);

int OvlFunc_947_200a74c(void)
{
    unsigned char *iw;
    unsigned int gs;
    unsigned int p;
    unsigned int off;
    unsigned int off2;
    int area;
    unsigned int i;
    unsigned int j;
    int x;
    int y;
    int m;
    int u;
    int w;
    int zero;
    int big;
    int twelve;
    struct Actor *a;
    struct E *e;

    iw = iwram_3001ebc;
    off = 0xe0;
    off <<= 1;
    *(int *)(iw + off) = 0x81 << 2;
    gs = (unsigned int)&gState;
    area = *(short *)((char *)gs + off);
    if (area == (int)(&_AREA_74)) {
        OvlFunc_947_2008ba4(8);
        OvlFunc_947_2008ba4(9);
        OvlFunc_947_2008ba4(0xa);
        OvlFunc_947_2008ba4(0xb);
        OvlFunc_947_2008ba4(0xc);
    } else if (area == (int)(&_AREA_77)) {
        u = 0;
        w = 0x40;
        __Func_80105d4(0x20, 0, w, 0x20, u, w);
        OvlFunc_947_2008ba4(8);
        OvlFunc_947_2008ba4(9);
        OvlFunc_947_2008ba4(0xa);
        OvlFunc_947_2008ba4(0xb);
        OvlFunc_947_2008ba4(0xc);
        OvlFunc_947_2008ba4(0xd);
        OvlFunc_947_2008ba4(0xe);
        OvlFunc_947_2008ba4(0xf);
        if (__GetFlag(0x109) && __GetFlag(0x80 << 2)) {
            u = 1;
            w = 2;
            __CopyMapTiles(0x4f, 0x22, 0x54, 0x18, u, w);
            m = 0x20;
            __CopyMapTiles(0, 0x20, 0x20, 0, m, m);
            __CopyMapTiles(0x20, 0x20, 0x40, 0, m, m);
            OvlFunc_947_2008ec8(9);
            OvlFunc_947_2008ec8(0xa);
            OvlFunc_947_2008ec8(0xb);
            OvlFunc_947_2008ec8(0xc);
            OvlFunc_947_2008ec8(0xd);
            OvlFunc_947_2008ec8(0xe);
            OvlFunc_947_2008ec8(0xf);
            u = 0x18;
            w = 8;
            __Func_8010704(0x18, 3, 1, 1, u, w);
        }
    } else if (area == (int)(&_AREA_79)) {
        OvlFunc_common0_70(0x92 << 18, 0, 0xc8 << 16, 0xdf);
        if (!__GetFlag(0x109))
            __MapActor_GetActor(0)->f62 = 1;
        __Func_8091494(0);
        if (__MapActor_GetActor(0)->f62 == 0)
            OvlFunc_947_200a09c();
        OvlFunc_947_200a694(8);
        OvlFunc_947_200a694(9);
        OvlFunc_947_200a694(0xa);
        OvlFunc_947_200a694(0xb);
        e = bss_36d0;
        for (i = 0; i <= 3; i++) {
            e->f0 = 0;
            e->f4 = 0;
            e->f8 = 0;
            e->f10 = i + (0x80 << 2);
            e++;
        }
        OvlFunc_947_2009d84();
        __CutsceneWait(1);
        __StartTask(OvlFunc_947_200a6b8, 0xc8 << 4);
        if (__GetFlag(0x109)) {
            for (j = 8; j <= 0xb; j++) {
                a = __MapActor_GetActor(j);
                x = a->f8 >> 20;
                if (x != 0x25)
                    continue;
                y = a->f10 >> 20;
                if (y != 9)
                    continue;
                __Func_8010704(0x1b, 8, 1, 1, x, y);
                break;
            }
        }
    } else {
        off2 = 0xe0;
        off2 <<= 1;
        p = gs + off2;
        if (*(short *)p == (int)(&_AREA_7a)) {
            __MapActor_SetAnim(0xa, 2);
            __Func_8092950(0xa, 6);
            OvlFunc_947_2008ba4(8);
            OvlFunc_947_2008ba4(9);
            zero = 0;
            __MapActor_GetActor(8)->f55 = zero;
            __MapActor_GetActor(9)->f55 = zero;
            OvlFunc_947_200a4cc();
            OvlFunc_947_200a5f8(0xb);
            OvlFunc_947_200a5f8(0xc);
            OvlFunc_947_200a5f8(0xd);
            OvlFunc_947_200a63c(0xb);
            OvlFunc_947_200a63c(0xc);
            OvlFunc_947_200a63c(0xd);
            __MapActor_GetActor(0xd)->f6c = zero;
            OvlFunc_947_200a5f8(0xe);
            __MapActor_GetActor(0xe)->f59 |= 8;
            if (!__GetFlag(0x202)) {
                big = 0xc0 << 9;
                __MapActor_GetActor(0xd)->f18 = big;
                __MapActor_GetActor(0xd)->f1c = big;
                twelve = 0xc;
                __MapActor_GetActor(0xd)->f50->f9 |= twelve;
                __MapActor_GetActor(0xe)->f50->f9 |= twelve;
                u = 0x16;
                w = 0x10;
                __Func_8010704(0x1a, 0xc, 1, 1, u, w);
            }
        }
    }
    return 0;
}
