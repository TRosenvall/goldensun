/* Func_80123f4 (BuildPerspectiveScanlineTable) -- NON-MATCHING, 59 encodings of 136
 * differ (same length; aligned screen: 33 instructions in disagreeing regions, of 133).
 * Needs a split once it matches: asm/rom_9000/rom_1219c_a_c_c_a_c.s also holds
 * Func_8012388 and Debug_SpriteTest.
 *
 * Verify with:
 *   python3 tools/objcmp.py src/non_matching/rom_9000/80123f4.c \
 *     asm/rom_9000/rom_1219c_a_c_c_a_c.s --func Func_80123f4
 *
 * `.call_via rN` is the fixed-multiply Func_8000888 as inline asm (landed pattern: see
 * src/rom_8a000/rom_97384_c_c_a_b.c), here with an UNPINNED function register ("r") and no
 * r3 clobber -- the ROM keeps r3 live across one (`mov r3, r0 / .call_via r6 / add r3, r0`),
 * and gcc picks r3 for the first call and r6 (callee-saved) inside the loop, as the ROM.
 * The sum of squares and the sqrt result must be DIFFERENT locals (s, v): one variable
 * gives r2 for both (42 -> 33).
 *
 * BLOCKER: which call-crossing pseudo spills.  Seven callee-saved registers, eleven
 * pseudos cross calls; the ROM spills h, t, neg and the b pointer bp (slot [sp]) and keeps
 * the counter i in r11, g in r9.  Ours spills i ([sp]) and keeps bp in r9, g in r11:
 * .17.lreg priorities bp 3*8/89 = 0.27 > i 3*9/138 = 0.20.  One ref fewer on bp would
 * flip it; b.y/b.z direct (CSE turns them back into bp), bp before/after the transform
 * call, out[i] indexing (worse, 67), loop bound spellings, declaration order: inert.
 * Also left: the vec init stores (ROM stores y=0 first, then x, z).
 */
#include "gba/types.h"

struct Projection {
    fx32 focal;
    fx32 zMin;
    fx32 zMax;
    s32 originX;
    s32 originY;
};

struct ScanRow {
    s32 a;
    s32 b;
    s32 c;
    s32 d;
    s32 e;
};

extern struct Projection gPhysVec;
extern fx32 Func_80008ac(fx32 num, fx32 denom);
extern fx32 Func_8000888(fx32 a, fx32 b);
extern fx32 Func_8000948(fx32 v);
extern void Func_80009c0(vec3_t *a, vec3_t *b);

static inline fx32 FxMul(fx32 (*f)(fx32, fx32), fx32 a, fx32 b)
{
    register fx32 _a __asm__("r0") = a;
    register fx32 _b __asm__("r1") = b;
    __asm__ volatile (
        "\t.align\t2, 0\n"
        "\tmov\tr12, pc\n"
        "\tbx\t%1"
        : "=r" (_a)
        : "r" (f), "0" (_a), "r" (_b)
        : "memory", "r12"
    );
    return _a;
}

void Func_80123f4(fx32 h, vec3_t *cam, struct ScanRow *out)
{
    vec3_t a;
    vec3_t b;
    vec3_t *bp;
    fx32 t, neg;
    s32 i;
    fx32 m, d, r;
    void (*transform)(vec3_t *, vec3_t *);
    fx32 (*div)(fx32, fx32);
    fx32 (*mul)(fx32, fx32);
    fx32 (*sq)(fx32);
    struct Projection *g;

    bp = &b;
    a.x = cam->x;
    a.y = 0;
    a.z = cam->z;
    transform = Func_80009c0;
    transform(&a, bp);
    t = bp->y - FxMul(Func_8000888, bp->z, h);
    g = &gPhysVec;
    neg = -g->focal;
    for (i = 0; i <= 0x9f; i++, out++) {
        g = &gPhysVec;
        div = Func_80008ac;
        m = div(neg, (g->originY - i) << 16);
        d = m - h;
        if (d == 0)
            d = 1;
        r = div(d, t);
        if (r < 0) {
            fx32 u, w, s, v;
            mul = Func_8000888;
            out->a = div(g->focal, FxMul(mul, -r, 0x8000));
            s = FxMul(mul, r, m);
            u = (bp->z - r) >> 4;
            w = (s - bp->y) >> 4;
            s = FxMul(mul, u, u);
            s += FxMul(mul, w, w);
            sq = Func_8000948;
            v = sq(s) << 12;
            if (w < 0)
                v = -v;
            out->b = FxMul(mul, v, 0x8000);
        } else {
            out->a = 0;
            out->b = 0;
        }
        out->c = 0;
        out->d = 0;
    }
}
