/* StartSnow  [rom_8a000]  --  asm/rom_8a000/rom_944ec_a_a_a_a_c_c.s
 *
 * BLOCKER CLASS: register roles and one scheduling hoist. 13 of 85.
 * NOT pool placement -- see below.
 *
 * THIS FUNCTION WAS SKIPPED ENTIRELY LAST BATCH on the belief that its
 * branch-over-pool shape made it unreachable. That belief is now retired
 * twice over: old_agbcc is not the compiler here, and gcc emits the pool
 * branch itself. In this function the ROM's `b` over its `0xf` pool is
 * present in OUR stream and MATCHES from the first screen. The shape was
 * never the problem.
 *
 * TWO LEVERS THAT PAID, both reusable:
 *   - `*q++ = v;` emits a single-register `stmia`, which the sibling park
 *     src/non_matching/rom_8a000/809509c.c wrongly claimed had no C form.
 *     33 -> 20 differing.
 *   - A CONSTANT BOUND FOR A 16-BIT MMIO STORE is pooled as `ldrh` by
 *     default; routing it through an `int` local makes gcc BUILD it
 *     (`mov`/`lsl`), which is what this ROM has. 20 -> 14. But read the ROM
 *     first: where it genuinely shows a pool load for a halfword store, the
 *     direct assignment with no local is the correct spelling.
 *   - `DMA3_FILL(p, 0, size)` and `DMA3_CLEAR(p, size)` are NOT
 *     interchangeable; FILL gives this ROM's operand order.
 *
 * WHAT IS LEFT, all three register or schedule:
 *   - 2 lines: the DMA3 zero lands in r3 where the ROM uses r1. The matched
 *     sibling in this same cluster uses DMA3_CLEAR, gets r3, and MATCHES, so
 *     this is pressure-dependent rather than a header problem.
 *   - 6 lines: the loop swaps two hard registers. 246 source variants were
 *     measured -- declaration order x24, statement order x18, `*q++`
 *     spellings x4, load/store interleave -- and EVERY ONE puts the pointer
 *     in r2. The swap never inverts.
 *   - 4 lines: sched2 hoists a `mov` three slots. Five prototypes and six
 *     call spellings all collapse to identical output, and the matched
 *     sibling Func_8094730 has the SAME hoist and matches, so this is a
 *     downstream tie-break rather than the call site.
 *
 * Declaration-order permutation was INERT here: all 24 orderings of the loop
 * locals gave byte-identical counts. It moves frame layout, not register
 * placement; do not spend rounds on it for the latter.
 */
#include "dma.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    int fc;
    int f10;
    int f14;
    unsigned char pad18[0x1c - 0x18];
    short f1c;
    unsigned char pad1e[0x20 - 0x1e];
};

extern int **iwram_3001e70;
extern unsigned char Data_a001e[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *f, int pri);
extern void Task_Snow(void);

void StartSnow(void)
{
    unsigned char *p;
    unsigned char *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int *q;
    int x;
    int y;
    int c1;
    int c2;

    p = galloc_ewram(0x1d, 0x82 << 3);
    e = (struct Ent *)(p + 8);
    DMA3_FILL(p, 0, 0x82 << 3);
    g = galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_a001e, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
        w = *iwram_3001e70;
        q = (int *)e;
        *q++ = 0;
        *q++ = 0x40000400;
        *q = 0xd4 << 8;
        y = w[2];
        x = w[0];
        e->fc = x;
        e->f14 = y;
        e->f10 = _Func_8011f54(0, x >> 16, y >> 16) << 16;
        e->f1c = (i & 0xf) + 1;
        i += 1;
        e = e + 1;
    } while (i <= 0x1f);
    c1 = 0xfc << 6;
    REG_BLDCNT = c1;
    c2 = 0x1008;
    REG_BLDALPHA = c2;
    REG_BLDY = 0;
    StartTask(Task_Snow, 0xc8 << 4);
}
