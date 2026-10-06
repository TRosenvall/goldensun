/* p3 -- StartRain -- PARK at 4 differing encodings of 104.  PIN-FREE.
 *
 * Figure I measured (not inherited): 4 differing encodings of 104 (ref 104, ours
 * 104), first differing index 80 -- ref 3302 (add r3,#2) against ours 21c8
 * (mov r1,#0xc8).  No SIZE line, no INSTRUCTION COUNT line, no RELOCATIONS line:
 * 244 bytes against 244, 103 instructions against 103.  PINS 0.
 *
 * Verify with: docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/StartRain.c asm/rom_8a000/rom_944ec_a_a_a_a_c_c_a_a_c_a_c.s --func StartRain
 *
 * SPLIT SHAPE: unchanged from the park.
 *
 * THE RESIDUE is one instruction displaced by four slots, the second argument's
 * high half hoisted ahead of the pointer-walk add and the two stores.  One run,
 * one cause.
 *
 * rank_for_schedule READ IN FULL (haifa-sched.c:4029-4112) -- the park's rung
 * list is confirmed complete.  It is a qsort comparator and schedule_block issues
 * ready[--n_ready], so the array sorts ASCENDING and the LAST element wins:
 * highest priority, then highest class.  One detail worth carrying forward: the
 * class test is `if (link == 0 || insn_cost (...) == 1) class = 3`, so a DATA
 * dependent of last_scheduled_insn whose cost is exactly 1 ALSO gets class 3 --
 * only a cost that is not 1 demotes an insn.  That is precisely why
 * arm_adjust_cost returning 0 for anti/output costs the pointer-walk add the tie.
 *
 * THE ESCAPE THE PARK NEVER CONSIDERED, AND IT IS CLOSED BY INVARIANCE.
 * The park bounds priority(281) = cost(281,282) + priority(282) = 1 + 66 and
 * rules out the two ways the 1 can become 0.  It never asks whether the 66 can
 * become 65.  From its own region table:
 *     priority(bl)              = 65
 *     priority(lsl)             = 1 + 65 = 66    ONE link from the call
 *     priority(mov high half)   = 1 + 66 = 67    TWO links from the call
 *     priority(ldr =Task_Rain)  = 1 + 65 = 66    ONE link
 *     priority(strh)            = 1 + 65 = 66    ONE link
 * The mov sits TWO dependence links from the call and every insn it must lose to
 * sits ONE, so any change to the call's priority shifts both sides equally and
 * the one-unit gap is INVARIANT under it.  Reaching 66 therefore requires
 * shortening the mov's distance to the call to one link, and the K-split puts the
 * shift between them unconditionally -- arm.md:3844 alternative 3 emits a split
 * marker for any thumb K constant and split_all_insns runs at toplev.c:3376,
 * before sched2 at :3481.  So the park's "no spelling of the second argument
 * reaches the 4" GENERALISES: the blocker is a path LENGTH in the dependence
 * graph, not a cost, and no spelling of anything reaches it.
 *
 * MEASURED THIS ROUND -- a dimension the park never varied.  The park swept nine
 * TAIL spellings; the blend-register store block is a different dimension and it
 * is the one that supplies the anti-dependence deciding the class rung.  All
 * worse, all at 104 encodings unless noted:
 *   both constants assigned before any store ............... 6
 *   the zero store between the other two ................... 6
 *   the zero store first ................................... 9
 *   the alpha block before the control block ............... 12
 *   the call before the zero store ... 84 of 105, 248 bytes, +2 insns, RELOCDIFF
 * So the park's store order is confirmed load-bearing and this dimension is a
 * measured dead end.
 *
 * STILL REFUTED, not re-proposed: a no-sched2 flag group (35 of 95).
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
extern unsigned char Data_9ff58[];

extern unsigned char *galloc_ewram(int tag, int size);
extern void Func_8091ff0(int a);
extern void DecompressLZ1(void *src, void *dst);
extern int AllocSpriteSlot(void);
extern int UploadSpriteGFX(int slot, int size, void *gfx);
extern void gfree(int tag);
extern int _Func_8011f54(int a, int b, int c);
extern void StartTask(void *f, int pri);
extern void Task_Rain(void);

void StartRain(void)
{
    unsigned char *p;
    int *g;
    struct Ent *e;
    unsigned int i;
    int t;
    int *w;
    int x;
    int y;
    int c1;
    int z;
    int c2;

    p = galloc_ewram(0x1d, 0x82 << 3);
    z = 0;
    w = *iwram_3001e70;
    Func_8091ff0(0xaa);
    e = (struct Ent *)(p + 8);
    DMA3_FILL(p, z, 0x82 << 3);
    g = (int *)galloc_ewram(0xe, 0x80 << 3);
    DecompressLZ1(Data_9ff58, g);
    t = AllocSpriteSlot();
    *(int *)p = t;
    *(int *)(p + 4) = UploadSpriteGFX(t, 0xc0 << 2, g);
    gfree(0xe);
    i = 0;
    do {
        g = (int *)e;
        *g++ = 0;
        *g++ = 0x40000400;
        *g = 0xd4 << 8;
        x = w[0];
        y = w[2];
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
    StartTask(Task_Rain, 0xc8 << 4);
}
