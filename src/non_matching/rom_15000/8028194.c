/* Func_8028194 / BuildMenuSprites (0x08028194) -- NON-MATCHING, 338 of 401 encodings differ.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *       goldensun-build python3 tools/objcmp.py \
 *       src/non_matching/rom_15000/8028194.c \
 *       asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s --func Func_8028194
 *
 * 338 IS NOT A DISTANCE: 836 bytes against 840, 399 encodings against 401 -- TWO
 * INSTRUCTIONS SHORT.  aligncmp (masks nothing) reads 176 of 401 aligned-equal
 * = 43.9%.  The RELOCATION SEQUENCE is exact and in order (Func_8003d28,
 * Func_8003dec, then iwram_3001f38, iwram_3001e40, Data_366f8, gSpriteSlots,
 * iwram_3001ecc), and the prologue matches -- three high registers pushed, not
 * four.
 *
 * THE NEW LEVER, AND IT IS THE REASON TO READ THIS FILE.
 * INTERLEAVE THE OTHER WORD'S INSERT TO STOP THE DEAD-MASK FOLD.  The affine
 * request block wants
 *
 *     blk.w[0] = ((blk.w[0] & 0xffff0000) | lo) & 0xffff | (lo << 16);
 *
 * and gcc PROVES the `& 0xffff` redundant (`force_to_mode` distributes the mask
 * through the IOR, `nonzero_bits` kills the left arm) and emits `lo | (lo << 16)`
 * with no read of blk at all.  src/non_matching/rom_9000/800b168.c records this
 * as its open blocker and lists two statements, four statements and named mask
 * locals as INERT -- all three confirmed here.  What works is putting the
 * `blk.w[1]` insert BETWEEN the two `blk.w[0]` statements:
 *
 *     blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)w;
 *     blk.w[1] = blk.w[1] & 0xffff0000;                        <- the barrier
 *     blk.w[0] = (blk.w[0] & 0xffff) | ((unsigned short)w << 16);
 *
 * 388 -> 397 encodings, 812 -> 832 bytes.  The intervening store to an aliasing
 * MEM stops the forward, so the second statement reloads and the mask survives.
 * `volatile int w[2]` in the union reaches the SAME 397/832 -- so this is the
 * pin-free spelling of a result that would otherwise cost a fakematch row, and
 * it is worth trying on 800b168.c, whose "NEXT" line is exactly this problem.
 *
 * It is still not the ROM's shape: the ROM has ONE load and ONE store for
 * blk.w[0] and keeps the dead `& 0xffff` inside a single expression, which
 * neither form reproduces.  That residue is ~4 instructions and is where the
 * remaining two-instruction shortfall most likely lives.
 *
 * SECOND LEVER: A WALKING int POINTER FOR THE THREE-WORD OAM WRITE.  The ROM
 * emits `mov r0, r6 / stmia r0!, {r3} / ... / stmia r0!, {r3} / ... / str r3, [r0]`
 * -- post-increment, which `p->f0 = / p->f4 = / p->f8 =` never produces.
 * `o = (int *)p; *o++ = 0; *o++ = ...; *o = ...;` does.  Applied to the i != sel
 * arm ALONE it is 397 -> 399 and the prologue still matches; applied to BOTH
 * arms it costs a fourth high register (r11) and the prologue grows by two
 * instructions each end, so the ROM uses the pointer in one arm only.
 *
 * WHAT IS ESTABLISHED:
 *   - VOID, NO ARGUMENTS.  r0-r3 are all overwritten before use.
 *   - THE HEADER IS iwram_3001f38, the `struct Ui` of the rom_23178 menu family
 *     -- stride 0x14 entries from +0, `sel` at 0x8c, `count` at 0x8e, `mode` at
 *     0x94.  The 0x14 stride and the 0x8e count are the layout the landed
 *     file-mates src/rom_15000/rom_23178_a_a_a_a_c_c_a_b.c and
 *     rom_23178_a_a_a_a_c_c_c_b.c already confirm, extended here by sel/mode.
 *     `h->ent[sel].f0c` reproduces the ROM's `sel*20 + 0xc` (gcc expands *20 as
 *     `lsl #2 / add / lsl #2`).
 *   - `count` IS RE-READ EVERY ITERATION through a HELD POINTER (r10) and the
 *     loop compare is UNSIGNED (`bcs`/`bcc`), so `i` is `unsigned int` and the
 *     short `count` converts to unsigned.  The sibling
 *     rom_23178_a_a_a_a_c_c_a_b.c records the OPPOSITE spelling for its own loop
 *     ("subscript, not a walking pointer") -- here the entry pointer IS a
 *     walking one, because `mov r6, r8` sits ABOVE the guard, before
 *     Func_8003d28 is even called.
 *   - `w = (Data_366f8[(iwram_3001e40 * 2) & 0x1f] - 0x100) / 4 + 0x130`.
 *     `(x*2) & 0x1f` is `lsl #1 / and #0x1f`; the algebraically equal
 *     `(x & 0xf) * 2` is `and #0xf / lsl #1` and does NOT match.  Data_366f8 is
 *     a `unsigned short []`.
 *   - THE SCALED OFFSETS ARE SIGNED DIVISIONS, spelled with the ROM's own
 *     numerators: `(w * 7) / 0x200`, `(w * 3) / 0x100`, `(w * 15) / 0x100`,
 *     `(w * 12 - 0xb01) / 0x100`, `(w * 32 - 0x1f01) / 0x200`.  Each one's
 *     rounding branch (`cmp #0 / bge / add 0x1ff or 0xff`) is what identifies
 *     the divisor, and the pooled `0xfffff5fe` / `0xffffe2fe` are the
 *     pre-rounded negative constants, not separate values.
 *   - gSpriteSlots IS `struct SpriteSlot { u16 size; u16 vramOffset; }` (the
 *     tree's existing declaration, 4 bytes), indexed by `p->f12`, and the
 *     array base is hoisted into the loop preheader BY LICM -- it must not be
 *     named before the loop, since the ROM's `ldr r4, =gSpriteSlots` sits
 *     AFTER the entry-count guard.  It gets r4, which `-fcall-used-r4` makes
 *     call-clobbered, hence the ROM's own `str r4, [sp] / ldr r4, [sp]` around
 *     Func_8003dec.
 *   - iwram_3001ecc is a byte pointer whose own [0x539] byte indexes a 644-byte
 *     stride (`b*644` comes out as the ROM's `lsl #2 / add / lsl #5 / add /
 *     lsl #2`), and the halfword array inside starts at +4 with the written
 *     field at +2 of each 4-byte record -- q+0x66 is element 0x18, q+6 is
 *     element 0, q+0x226 is element 0x88.
 *   - THE FOUR INNER LOOPS write only the HIGH byte: `*r = (*r & 0xff) | v`.
 *     Their bounds are inclusive in the ROM (`bls` against 0x17 / 0x87 / 0x9f),
 *     so `i <= N` and not `i < N + 1`; the `< N+1` spelling lets gcc fold the
 *     +1 into the `+ 0x18` and loses the ROM's separate `add r4, r3, #1`.
 *   - THE TWO TAIL BLOCKS ARE NOT A SHARED SUBROUTINE: mode == 0 runs
 *     `i = 0x18 .. Y + 0x18 + k2` then `i = 0 .. 0x17`; mode != 0 runs
 *     `i = Y - k2 - 1 .. 0x87` then `i = 0x88 .. 0x9f`.  Written out twice, as
 *     the ROM has them.
 *
 * MEASURED LADDER (ref 840 bytes / 401 encodings):
 *   first draft                                    808 / 387, 388 differ
 *   + `unsigned short rec[]` indexed [i*2+1] so the giv is the halfword address
 *     (gives `ldrh [r1]` and the pooled 0x226, not `ldrh [r1,#2]`)
 *   + inclusive loop bounds                        812 / 388, 387 differ
 *   + the blk.w[1] barrier                         832 / 397, 362 differ
 *   + the walking int pointer in one arm           836 / 399, 360 differ
 *   + `h->count` inline instead of a `short *pc` local
 *                                                  836 / 399, 338 differ  <- this file
 *
 * INERT OR WORSE, MEASURED: a `struct Rec { u16 a; u16 b; }` array (identical
 * output to the flat `u16 rec[]` once the index is [i*2+1]); walking
 * `unsigned short *r` pointers in all four inner loops (byte-identical to the
 * subscript form -- strength reduction gets there either way); the single-
 * expression affine insert (812/388, folds); reading blk.h[0] back for the
 * second insert (812/388); the walking int pointer in both arms (836/400 but
 * a fourth high register in the prologue); `pc` kept with the pointer in both
 * arms (840/402 -- SIZE EXACT with the count one OVER, and further away).
 *
 * NEXT: the ROM's single-load/single-store affine insert.  The barrier gets the
 * instruction count to within two but not the shape; what is wanted is a
 * spelling in which combine does not distribute the `& 0xffff` through the IOR
 * at all.  A `"+r"` barrier on the intermediate was not tried, deliberately --
 * it would be a pin, and the point of the interleave finding is that this class
 * is reachable without one.
 *
 * SPLIT SHAPE: asm/rom_15000/rom_23178_a_a_a_a_c_a_c_c.s holds Func_8027114
 * (0x8027114, 2,007 lines, unattempted) and Func_8028194, and datacheck.py
 * reports no data.  Landing this one alone is a two-way split with Func_8027114's
 * .s first.  Func_8028194 is already referenced by name from three landed
 * file-mates (src/rom_15000/rom_23178_a_a_a_c_c_c.c,
 * rom_23178_a_a_a_a_c_c_a_b.c, rom_23178_a_a_a_a_c_b.c), which pass it to
 * StartTask/StopTask, so the symbol is global today.  NO SHIMS:
 * tools/shimcount.py reports none, and there is no inline asm at all.
 */
#include "gba/types.h"

struct Ent {
    int f0;
    int f4;
    int f8;
    short f0c;
    short f0e;
    unsigned short f10;
    unsigned short f12;
};

struct Ui {
    struct Ent ent[7];
    short sel;
    short count;
    unsigned char pad90[4];
    short mode;
};

struct SpriteSlot {
    unsigned short size;
    unsigned short vramOffset;
};

struct Q {
    int f0;
    unsigned short rec[1];
};

union AffineReq {
    int w[2];
    unsigned short h[4];
};

extern unsigned char *iwram_3001f38;
extern unsigned char *iwram_3001ecc;
extern unsigned int iwram_3001e40;
extern unsigned short Data_366f8[];
extern struct SpriteSlot gSpriteSlots[];
extern int Func_8003d28(union AffineReq *req);
extern void Func_8003dec(struct Ent *e, int kind);

void Func_8028194(void)
{
    struct Ui *h;
    struct Ent *p;
    struct Q *q;
    unsigned char *e;
    union AffineReq blk;
    unsigned int i;
    int w;
    int t;
    int mtx;
    int kind;
    int sel;
    int X;
    int Y;
    int k;
    int v;
    int u;
    int n;
    int *o;

    h = (struct Ui *)iwram_3001f38;
    t = Data_366f8[(iwram_3001e40 * 2) & 0x1f];
    w = (t - 0x100) / 4 + 0x130;
    p = h->ent;
    blk.w[0] = (blk.w[0] & 0xffff0000) | (unsigned short)w;
    blk.w[1] = blk.w[1] & 0xffff0000;
    blk.w[0] = (blk.w[0] & 0xffff) | ((unsigned short)w << 16);
    mtx = Func_8003d28(&blk);
    for (i = 0; i < h->count; i++, p++) {
        if (p->f0c != 0) {
            if (i == h->sel) {
                X = p->f0c + (w * 7) / 0x200 - 0x14;
                if (p->f0e != 0)
                    Y = p->f0e + (w * 3) / 0x100 - 0x14;
                else
                    Y = ((w * 15) / 0x100 - 0x1e) & 0xff;
                p->f0 = 0;
                p->f4 = (mtx << 25) | Y | (X << 16) | 0x80002300;
                p->f8 = gSpriteSlots[p->f12].vramOffset >> 5;
                kind = 0xf6;
            } else {
                o = (int *)p;
                *o++ = 0;
                *o++ = p->f0e | (p->f0c << 16) | 0x80002000;
                *o = gSpriteSlots[p->f12].vramOffset >> 5;
                kind = 0xf5;
            }
            Func_8003dec(p, kind);
        }
    }
    if (h->mode == 0) {
        e = iwram_3001ecc;
        if (e == 0)
            return;
        if (h->count == 0)
            return;
        q = (struct Q *)(e + e[0x539] * 644);
        sel = h->sel;
        X = h->ent[sel].f0c;
        k = (w * 12 - 0xb01) / 0x100;
        v = ((X - k) << 8) + (X + k) + 0x17;
        Y = h->ent[sel].f0e;
        n = Y + 0x18 + (w * 32 - 0x1f01) / 0x200;
        for (i = 0x18; i <= n; i++)
            q->rec[i * 2 + 1] = (q->rec[i * 2 + 1] & 0xff) | v;
        u = h->ent[0].f0c;
        if (h->sel == 0)
            u = u - (w * 12 - 0xb01) / 0x100;
        u <<= 8;
        for (i = 0; i <= 0x17; i++)
            q->rec[i * 2 + 1] = (q->rec[i * 2 + 1] & 0xff) | u;
    } else {
        e = iwram_3001ecc;
        if (e == 0)
            return;
        if (h->count == 0)
            return;
        q = (struct Q *)(e + e[0x539] * 644);
        sel = h->sel;
        X = h->ent[sel].f0c;
        k = (w * 12 - 0xb01) / 0x100;
        v = ((X - k) << 8) + (X + k) + 0x17;
        Y = h->ent[sel].f0e;
        n = Y - (w * 32 - 0x1f01) / 0x200;
        for (i = n - 1; i <= 0x87; i++)
            q->rec[i * 2 + 1] = (q->rec[i * 2 + 1] & 0xff) | v;
        u = h->ent[0].f0c;
        if (h->sel == 0)
            u = u - (w * 12 - 0xb01) / 0x100;
        u <<= 8;
        for (i = 0x88; i <= 0x9f; i++)
            q->rec[i * 2 + 1] = (q->rec[i * 2 + 1] & 0xff) | u;
    }
}
