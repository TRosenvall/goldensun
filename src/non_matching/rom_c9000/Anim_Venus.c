/* Anim_Venus  --  0x080e0564, first of the four functions in
 * asm/rom_c9000/rom_e0564_a_a.s (Anim_Venus, Anim_Mars, Anim_Hail, Anim_Ground).
 *
 * NON-MATCHING, 20 of 381 encodings.  WAS 22; batch 326 (brief H) took it to 20.
 * SIZE and INSTRUCTION COUNT are both the ROM's (860 bytes, 381 = 381), so the
 * 20 IS a true distance.  aligncmp reads 366/381 aligned-equal (96.1%),
 * 24 differing/ins/del in 12 hunks.  objcmp verbatim:
 *     XX ENCODINGS differ in 20 place(s) (ref 381, ours 381)
 *        first at index 38: ref 6a2d  ours 9306
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/Anim_Venus.c \
 *     asm/rom_c9000/rom_e0564_a_a.s --func Anim_Venus
 *   (the previous header's recipe named src/non_matching/rom_c9000/e0564_Anim_Venus.c,
 *    a path that does not exist -- repointed here.)
 *
 * SPLIT SHAPE IF IT LANDS: unchanged from the earlier header.  Anim_Venus is the
 * FIRST function of rom_e0564_a_a.s, so a two-way split; with Anim_Hail also
 * landed the file set is
 *   src/rom_c9000/rom_e0564_a_a.c  Anim_Venus
 *   asm/rom_c9000/rom_e0564_a_b.s  Anim_Mars
 *   src/rom_c9000/rom_e0564_a_c.c  Anim_Hail (renumbered)
 *   asm/rom_c9000/rom_e0564_a_d.s  Anim_Ground
 * `python3 tools/datacheck.py asm/rom_c9000/rom_e0564_a_a.s` is SILENT (rc 0,
 * re-run batch 326); no data moves and no new exports are needed.
 *
 * PINS: 1 (the Anim_Vine StartTask pin, `tools/shimcount.py` = 1), unchanged and
 * still load-bearing -- 22 without it, 20 with.
 *
 * ================= WHAT BATCH 326 CHANGED: 22 -> 20 =========================
 * SPLIT THE BIAS OFF THE SHIFT in the blitB frame computation.  The ROM pairs
 * the two `asr #16`s and applies both biases afterwards; writing the bias into
 * the same expression makes gcc interleave x's `add` between them:
 *     ref   asr r5,#16 | asr r3,#16 | add r5,#0x16 | str r2,[sp] | ... | add r3,#0x1d
 *     ours  asr r5,#16 | add r5,#22 | asr r3,#16   | str r2,[sp] | ... | add r3,#29
 * So
 *     x = (sin(ang) * 24) >> 16;
 *     y = ((0x40 - frame * 2) * cos(ang)) >> 16;
 *     x += 0x16;
 *     y += 0x1d;
 * instead of biasing inside each expression.  This is the SAME family rule batch
 * 325 settled for HeightTile ("the writeback must be on the SUBTRACTION, not the
 * bias") pointing the other way: here the bias must be its OWN statement.
 * Biasing only x and leaving y fused is byte-identical (also 20), so the lever is
 * x's bias; writing `x = 0x16 + (...)` (operand order) is INERT at 22.
 *
 * ================= THE REMAINING 20, FOUR CLUSTERS =========================
 * Every role register is still the ROM's -- r9 base, r10 the shared counter,
 * r11 th, r8 p, the whole spill map, every branch target, every relocation.
 *
 * (A) 2 enc, index 38.  `str r3,[sp,#0x18]` one slot EARLY:
 *       ref  ldr r3,[r5,#0x1c] | ldr r5,[r5,#0x20] | str r3,[sp,#0x18]
 *       ours ldr r3,[r5,#0x1c] | str r3,[sp,#0x18] | ldr r5,[r5,#0x20]
 *     blitA's reload OUTPUT store against blitB's load.  sched2.
 *
 * (B) 4 enc, index 177-181.  The `gBuffer` pool load is r3 in ours and r2 in the
 *     ROM, and it is one slot late:
 *       ref  ldr r1,[sp,#0xc] | ldr r2,=gBuffer | mov r4,#0 | add r7,r1,r2
 *       ours ldr r3,=gBuffer  | ldr r1,[sp,#0xc]| mov r4,#0 | add r7,r1,r3
 *     THIS IS local-alloc, AND IT IS THE SAME BOUND AS Anim_Attack'S.  `.17.lreg`
 *     block 12 contains exactly ONE local qty, pseudo 117 (insn 403, the pool
 *     load; `;; Register 117 in 3.`), pref LO_REGS, no suggestion, life insns
 *     403-405.  `find_free_reg` (local-alloc.c:1963-2050) walks REG_ALLOC_ORDER
 *     {3,2,1,0,...} and takes the first free, so r3.  For r2, r3 must be in
 *     `used` across 117's life -- and no *local* qty can be, because block 12 is
 *     three insns long and anything live at its end is global.  Pinning it
 *     (`register char *gb __asm__("r2")`) DOES put the load in r2, measured, but
 *     then gcc canonicalises the add as `adds r7,r2,r1` against the ROM's
 *     `adds r7,r1,r2` and the figure stays 20 -- writing `boff + gb` does not
 *     flip it.  So the pin buys nothing here.
 *
 * (C) 3 enc, index 184-188, and ~11 enc at index 193-203.  TWO RELOAD SCRATCH
 *     REGISTERS inside the particle loop:
 *       0x80<<7 (the `+ 0x4000` on a2):  ROM r3, ours r2
 *       0x80<<1 (the `+ 0x100` on mag):  ROM r0, ours r2
 *     `.18.greg` prints `Spilling for insn 425. / Using reg 3` and
 *     `Spilling for insn 445. / Using reg 3` -- **find_reg printed r3 for BOTH
 *     and the emitted registers are r2 and r2**, which is the batch-325
 *     observable again: a `Using reg` line is not the register you get.  Confirm
 *     in `.19.flow2`: insns 1186/1187 and the pair before 445 are both r2.
 *     So this is LAYER 3, `allocate_reload_reg`'s round-robin (reload1.c:4996-5065)
 *     over `spill_regs[]`.  Two facts worth keeping:
 *       * `spill_regs[]` is built in ASCENDING HARD-REGISTER ORDER
 *         (reload1.c:3527-3532), NOT REG_ALLOC_ORDER, so the cursor walks
 *         r0,r1,r2,r3,... -- do not reason about it with {3,2,1,0}.
 *       * r0 IS free at insn 445 in our build: `.19.flow2` insn 441 carries
 *         `REG_DEAD (reg/v:SI 0 r0)` immediately before the reload.  The ROM
 *         takes r0 and we do not, so this is purely the CURSOR POSITION
 *         (`last_spill_reg`, reload1.c:4937), not availability.
 *     AND A PIN CANNOT REACH IT: `register int c __asm__("r0"); c = 0x80 << 1;
 *     mag = (rv & 0x1ff) + c;` compiles BYTE-IDENTICALLY to the unpinned body --
 *     the named pseudo is copy-propagated away and reload invents its own
 *     constant register regardless.  Same for a pin on the `i & 1` copy.
 *
 * (D) 2 enc, index 225-227.  `mov r2,sl / ands r3,r2` (ROM) against
 *     `mov r4,sl / ands r3,r4` (ours) -- the hi->lo copy for `i & 1`, another
 *     reload scratch register from the same cursor.
 *
 * So the real residue is: TWO sched2 ties (A and B's slot, 4 enc), ONE local-alloc
 * first-free walk (B's register, bounded above), and THREE reload-cursor
 * registers (C and D, ~16 enc) that move together.  The cursor is a single global
 * sequence over the function, so the lever is the NUMBER AND ORDER OF RELOADS
 * EARLIER IN THE FUNCTION, not the spelling of any differing site.
 *
 * MEASURED INERT, byte-identical to this file -- do not repeat:
 *   `g = (Part *)(boff + (char *)gBuffer)`; `&((char *)gBuffer)[boff]`;
 *   `(Part *)((unsigned)gBuffer + boff)`; a named `char *gb` for the address;
 *   `k = 0` before `g = ...`; `blitA, blitB` as one comma statement; via
 *   `pp = tbl + 7; *pp++`; `blitB = (DrawFn)*(tbl + 8)`; two temporaries for the
 *   blit pair; `x = 0x16 + (...)` operand order; a `register __asm__("r0")` pin
 *   on the 0x100 constant; a `register __asm__("r2")` pin on the `i & 1` copy.
 *   (Plus everything the earlier header already listed: operand order on
 *   `(Random() & 0x7fff) + 0x4000` and on `+ 0x100`; `a2`/`rv`/`mag` at function
 *   scope; `k` declared before `g`; `(i & 1) != 0`.)
 * MEASURED WORSE: `blitB` assigned before `blitA` 24; `g = &gBuffer[boff / 28]`
 *   239 and 12 bytes long; and the earlier header's list (DrawFn returning void
 *   27, BuildDraw2DFuncEx void 31, LoadVFXFile returning int 43, `gBuffer[i]`
 *   indexing in the draw loop 182, the particle-init loop indexed 265, `mag`
 *   split into mask-then-add 187).
 *
 * THE FIVE LEVERS THAT TOOK IT 312 -> 22 ARE ALL STILL LOAD-BEARING and are
 * documented at length in the batch-32x history: one counter across FOUR loops;
 * the spill map is REVERSE DECLARATION ORDER; the ids loop is a `while`, not an
 * `if`-guarded do-while; naming the second Random() result so the `g->x` store
 * interleaves into the `mag` computation; and the 0x20-slot init loop's base as a
 * strength-reduced giv (`((Part *)(base + (0xe1 << 7)))[i].x`) with the
 * per-target loop's four increments in the order `th, p, boff, i`.
  *
 * ================================================================
 * BATCH 327B -- RE-DERIVED, AND ONE INSTRUMENT RULED OUT FOR THIS FUNCTION
 * ================================================================
 * Baseline re-derived as installed: 20 of 381, counts exact (381 = 381), no
 * SIZE line and no RELOCATIONS line, first differing index 38.  The 20 stands.
 *
 * THE giv-ORDER LEVER THAT BATCH 327B OPENED ON Anim_Whirlwind DOES NOT APPLY
 * HERE, and this is worth recording so nobody spends the compile.  On
 * Anim_Whirlwind two strength-reduced givs land in two frame slots and their
 * order is decided by record_giv's prepend (see that file's batch-327B block).
 * `Anim_Venus`'s `.08.loop` has only ONE REDUCED GIV IN THE WHOLE FUNCTION --
 *     Loop from 535 to 618: giv at 567 reduced to (reg:SI 222)
 * and every other candidate is either "combined with"/"recombined with" another
 * giv or rejected ("giv of insn 761 not worth while, -102 vs 44", likewise 711,
 * 438, 434).  With one reduced giv per loop there is no PAIR to order, so the
 * slot-ordering question cannot arise.
 *
 * The residue characterised in the batch-326 block above (two sched2 ties, one
 * local-alloc first-free walk, three reload-cursor registers) is unchanged and
 * nothing in it was refuted this batch.
*/
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x;
    int y;
    int z;
    int vx;
    int vy;
    int vz;
    int t;
} Part;

extern int *iwram_3001eec[];
extern int ewram_2010018;
extern Part gBuffer[];
extern unsigned short Data_ede48[];

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern int Random(void);
extern int sin(int a);
extern int cos(int a);
extern void StartTask(void *fn, int arg);
extern void StopTask(void *fn);
extern void Task_BlitAnim(void);
extern void _PlaySound(int id);
extern void _Func_80bd7dc(int a);
extern void Func_80d6888(int id, int a, int b, int c, int d);
extern void _SetBattleActorKnockback(int id, int a);
extern void Func_80e3908(Part *g, int a, int b);
extern void UpdateScreenShake(int w, int h);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Venus(void *context)
{
    void *ctx;
    int frame;
    DrawFn blitB;
    DrawFn blitA;
    unsigned char *gfx;
    int ang;
    int boff;
    unsigned char *base;
    int **tbl;
    int **pp;
    int i;
    int th;
    Part *p;
    int arg;

    tbl = (int **)iwram_3001eec;
    pp = tbl;
    base = (unsigned char *)*pp++;
    ctx = (void *)*pp;
    gfx = (unsigned char *)tbl[2];
    *(State **)(base + 0x7828) = (State *)context;
    AnimStart(0);
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncEx(0x2e, 7, 7, 0xb, 2);
    BuildDraw2DFuncEx(0x2f, 7, 7, 3, 3);
    blitA = (DrawFn)tbl[7];
    blitB = (DrawFn)tbl[8];
    LoadVFXFile(FILE_73, gfx, 0, 0);
    LoadVFXFile(FILE_94, base, 1, 1);
    LoadVFXFile(FILE_6f, base + (0xbe << 2), 1, 0);
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x4b;
    arg = 0x90;
    {
        register void *tf __asm__("r0");
        tf = (void *)Task_BlitAnim;
        arg <<= 3;
        StartTask(tf, arg);
    }
    {
        i = 0;
        do {
            ((Part *)(base + (0xe1 << 7)))[i].x = Random() & 0x3f;
            ((Part *)(base + (0xe1 << 7)))[i].y = 0x68;
            i++;
        } while (i != 0x20);
    }
    {
        int *q = &ewram_2010018;
        i = 0;
        do {
            i++;
            *q = -1;
            q = (int *)((char *)q + 0x1c);
        } while (i != (0x80 << 2));
    }
    _PlaySound(0x8d);
    frame = 0;
    ang = 0x80 << 8;
    do {
        if (frame <= 0x4f) {
            int x;
            int y;

            x = (sin(ang) * 24) >> 16;
            y = ((0x40 - frame * 2) * cos(ang)) >> 16;
            x += 0x16;
            y += 0x1d;
            blitB(ctx, base, x, y, 0x14, 0x26);
        }
        if (frame == 0x38) {
            _Func_80bd7dc(0x85);
        }
        i = 0;
        boff = 0;
        th = 0x10;
        p = (Part *)(base + (0xe1 << 7));
        do {
            if (frame >= th) {
                blitA(ctx, base + (0x9e << 4), p->x - 0x11, p->y - 0x20, 0x22, 0x41);
                if (frame == th) {
                    Part *g;
                    int k;

                    g = (Part *)((char *)gBuffer + boff);
                    k = 0;
                    do {
                        int a2 = (Random() & 0x7fff) + (0x80 << 7);
                        int rv = Random();
                        int mag;
                        g->x = p->x << 16;
                        mag = (rv & 0x1ff) + (0x80 << 1);
                        g->y = (p->y + 0x10) << 16;
                        g->vx = (sin(a2) * mag) >> 7;
                        g->vy = (cos(a2) * mag) >> 6;
                        g->t = (Random() & 0xf) + 0x20;
                        k++;
                        g++;
                    } while (k != 0x10);
                    if (i & 1) {
                        _PlaySound(0x85);
                    }
                    *(int *)(base + 0x77a8) = 4;
                    k = 0;
                    while (k != (*(State **)(base + 0x7828))->f14) {
                        Func_80d6888((*(State **)(base + 0x7828))->ids[k], 7, 5, k, 6);
                        _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[k], 6);
                        k++;
                    }
                }
                p->y -= 0xc;
            }
            th += 4;
            p = (Part *)((char *)p + 0x1c);
            boff += 0xe0 << 2;
            i++;
        } while (i != 0xa);
        {
            Part *g = gBuffer;
            i = 0;
            do {
                if (g->t != -1) {
                    int n = g->t / 16 + 2;
                    unsigned char *src = gfx + Data_ede48[n - 1];
                    int w2 = n * 2;
                    int x = *(short *)((char *)g + 2) - n / 2;
                    int y = *(short *)((char *)g + 6) - n;
                    blitB(ctx, src, x, y, n, w2);
                    Func_80e3908(g, 0x3e, 0x80 << 6);
                    g->t -= 1;
                }
                i++;
                g++;
            } while (i != (0x80 << 2));
        }
        UpdateScreenShake(4, 4);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        ang -= 0x800;
        frame++;
    } while (frame != 0x60);
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
