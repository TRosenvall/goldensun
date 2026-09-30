/* Func_8018efc -- NON-MATCHING, 2 of 119 encodings differ.
 * Reference asm/rom_15000/rom_18cac_a_c.s.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/8018efc.c \
 *       asm/rom_15000/rom_18cac_a_c.s --func Func_8018efc
 *
 * IMPROVED IN BATCH 305 FROM 17 TO 2.  SIZE AND COUNT BOTH MATCH (119 == 119),
 * so 2 IS A TRUE DISTANCE.  aligncmp 99.2% aligned-equal, 2 differing in 2
 * hunks.  PIN-FREE -- tools/shimcount.py reports nothing at all.
 *
 * TWO LEVERS LANDED THE 15, both from batch 305's variable-reuse family:
 *
 * (1) MERGE `base` AND `q` INTO ONE VARIABLE -- worth 14 of the 17.
 *     The park declared `unsigned char *base` and `unsigned short *q` and wrote
 *     `q = (unsigned short *)(base + 0x12b6)`.  The ROM does NOT keep two
 *     quantities: it emits `add r6, r2` (r2 = 0x12b6), i.e. it ADVANCES the
 *     base pointer IN PLACE, so `base` and `q` are ONE variable in the source.
 *     Writing them as one -- `q = (unsigned short *)p;` at the top, `q += 0x95b;`
 *     at the point of use, and `(unsigned char *)q + 0x698` for the slot
 *     subtraction -- collapses two allocnos into one, and that ONE allocno wins
 *     the r6/r7 race against the `win` parameter that the two separate ones lost.
 *     17 -> 3.  This is the r6/r7 role swap the previous park blamed for 13 of
 *     its 17: it was never a scheduling or declaration-order problem, it was
 *     TWO VARIABLES WHERE THE ROM HAS ONE.
 *
 *     The three probes the old park recorded as inert (swapping the `base`/`p`
 *     declaration order, `*(unsigned short *)(p + pos * 2)`, both together) were
 *     inert for exactly this reason -- none of them changes the allocno COUNT.
 *
 * (2) A SEPARATE INDEX LOCAL FOR THE HALFWORD STORE -- worth 1 more.
 *     `idx = pos * 2; *(unsigned short *)(p + idx) = ch | 0xf000;` gives the
 *     ROM's `strh r1, [r5, r2]`; the array form `((unsigned short *)p)[pos]`
 *     gives `strh r1, [r2, r5]`.  With the scaled index still inside the address
 *     expression gcc canonicalises the PLUS with the more complex operand first,
 *     so the base/index order inverts.  Hoisting the multiply into its own
 *     declared local leaves two plain registers and the source order survives.
 *     `*(unsigned short *)(p + pos * 2)` WITHOUT the extra local is inert at 3 --
 *     the local, not the pointer spelling, is the lever.  3 -> 2.
 *
 * THE REMAINING 2 ARE ONE ADJACENT SCHEDULE SWAP AT THE HEAD OF BLOCK L2:
 *     rom   ldrh r3, [r7, #0x8]   /  ldr r1, =0xfffe
 *     ours  ldr r1, =0xfffe       /  ldrh r3, [r7, #0x8]
 * Two independent loads that both feed the following `add r3, r1`, i.e. an exact
 * sched2 priority tie.  Pre-sched2 (tools/tryc.py --no-sched2) our RTL order is
 * `ldrh x / ldrh w / ldr const`, and sched2 hoists the constant load to the front
 * of the block; the ROM's output equals a pre-sched2 order of
 * `ldrh w / ldr const / ldrh x`, i.e. the ROM's sched2 sank the `win->x` load
 * instead.  Note the file-mate pair at `ldr r1, =0xfffffe00 / ldrh r3, [r4, #0x6]`
 * puts the constant FIRST in both ROM and ours, so "constant first" is gcc's
 * normal answer for a two-insn tie and this three-insn group is the anomaly.
 *
 * MEASURED INERT ON THE REMAINING 2 -- do not re-try (26 builds):
 *   * CONSTANT SPELLING: `0xfffe + win->w`, `win->w + 0xfffe`, `-2 + win->w` -- 2.
 *   * REASSOCIATION of the x expression: `+ 4` moved to every position
 *     ((x<<3) + 4 + (w2<<3); ((x<<3)+4) + (w2<<3); (x<<3) + ((w2<<3)+4);
 *     4 + (x<<3) + (w2<<3)) -- all 2.
 *   * STATEMENT POSITION: `s = &n->spr` before `q += 0x95b` -- 2;
 *     `n->f5 = 2` after `q += 0x95b` -- 2.
 *   * FLAGS, every one inert at 2: -fno-rerun-cse-after-loop, -fno-gcse,
 *     -fno-strength-reduce, -fno-cse-follow-jumps, -fno-peephole,
 *     -fno-schedule-insns (sched1), -fno-force-mem, -fno-thread-jumps,
 *     -fno-if-conversion, -fno-delayed-branch, -fno-function-cse.
 *     SO NO Makefile FLAG ROW SHOULD BE WRITTEN FOR THIS FILE.
 *
 * MEASURED WORSE -- do not re-try:
 *   * -fno-schedule-insns2 45, -fno-strict-aliasing 5,
 *     -fno-expensive-optimizations 88 (and 116 insns).
 *   * w-term first in the DISTRIBUTED form,
 *     `((unsigned short)(win->w - 2) << 3) + (win->x << 3) + 4` -- 24, and it
 *     does NOT even fix the target region: it still emits the constant load
 *     first and additionally swaps the r2/r3 roles through the whole block.
 *   * `((unsigned short)(win->w - 2) << 3) + 4 + (win->x << 3)` -- 23.
 *   * `* 8` instead of `<< 3` -- 84 at 121 insns.
 *   * THE UNDISTRIBUTED FORMS, which is the old park's rule re-confirmed from
 *     the other side: `((win->x + (unsigned short)(win->w - 2)) << 3) + 4` -- 82
 *     at 117 insns; the same with the w term first -- 82; the y analogues -- 59
 *     at 120; both together -- 78.  The ROM emits only ONE `lsl #3`, which makes
 *     the undistributed form look right, but combine folds the distributed
 *     source's two shifts back into that one and the undistributed source loses
 *     the range fold entirely.  DISTRIBUTE THE SHIFT BY HAND, still.
 *   * SIZE-BREAKERS (reloc offsets move, so not distances at all): swapping the
 *     `s->x` / `s->y` statements; `s = &n->spr` after the AllocSpriteSlot if;
 *     routing the x value through any extra named local (`int xx`, or reusing
 *     `idx` / `pos`) -- all four change the translation-unit size.
 *   * `slot` computed after the AllocSpriteSlot if -- 13.
 *   * an `int` local for `win->w` read in its own statement -- 252 bytes against
 *     260; for `win->x` -- 11; both -- 252 bytes; reusing `pos` for the w read --
 *     13; reusing `idx` -- 15.
 *
 * WHAT IS LEFT.  One sched2 tie with no source handle found in 40 builds.  The
 * group is three ready loads, not two, so the handle would have to change the
 * DAG shape rather than the order -- and every spelling that changes the DAG
 * shape here also changes the size.  Park it.
 */
struct Spr {
    unsigned int a;
    unsigned char y;
    unsigned char b;
    unsigned short x : 9;
    unsigned short rest : 7;
};

struct Node {
    unsigned int next;
    unsigned char f4;
    unsigned char f5;
    unsigned short x;
    unsigned short y;
    unsigned char pad[4];
    unsigned char slot;
    unsigned char fF;
    struct Spr spr;
    unsigned int pad2;
};

typedef unsigned char B7[7];

struct Win {
    unsigned char pad[8];
    unsigned short w;
    unsigned short h;
    unsigned short x;
    unsigned short y;
};

extern unsigned char *iwram_3001e8c;
extern struct Node *Func_8015e8c(void);
extern int AllocSpriteSlot(void);
extern int Func_8016584(struct Win *, struct Node *);

#define n ((struct Node *)p)
void Func_8018efc(struct Win *win, unsigned int ch, unsigned int x, unsigned int y, int mode)
{
    unsigned char *p;
    struct Spr *s;
    unsigned short *q;
    int slot;
    unsigned int pos;
    unsigned int idx;

    p = iwram_3001e8c;
    q = (unsigned short *)p;
    if (y > win->h - 2)
        return;
    if (x > win->w - 2)
        return;
    if (mode == 1) {
        p = (unsigned char *)Func_8015e8c();
        if (p == 0)
            return;
        slot = (B7 *)n - (B7 *)((unsigned char *)q + 0x698);
        n->f5 = 2;
        q += 0x95b;
        s = &n->spr;
        if (*q == 0x63)
            *q = AllocSpriteSlot();
        s->x = (win->x << 3) + ((unsigned short)(win->w - 2) << 3) + 4;
        s->y = (win->y << 3) + ((unsigned char)(win->h - 2) << 3) - 1;
        n->x = s->x;
        n->y = s->y;
        n->next = 0;
        n->slot = slot;
        if (n->f5 == 0)
            n->f5 = mode;
        Func_8016584(win, n);
    } else {
        if (ch > 0xff)
            return;
        y++;
        x++;
        pos = ((win->y + y) << 5) + (win->x + x);
        if (pos >= 0x280)
            return;
        idx = pos * 2;
        *(unsigned short *)(p + idx) = ch | 0xf000;
    }
}
