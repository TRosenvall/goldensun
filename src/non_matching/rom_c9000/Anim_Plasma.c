/* Anim_Plasma -- PARKED.  0x080d41a4, 466 instructions in the ROM listing.
 * NON-MATCHING, 378 of 493 encodings differ.
 * SIZE  ref 1096 bytes, ours 1092 (-4).  COUNT ref 493, ours 491 (-2).
 * tools/aligncmp.py: aligned-equal 406 of 493 (82.4%), 97 differing in 50 hunks.
 * SHIMS: none.  `python3 tools/shimcount.py` is silent.  No per-file flag override.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c9000/Anim_Plasma.c \
 *     asm/rom_c9000/rom_d2d98.s --func Anim_Plasma
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_c9000/Anim_Plasma.c \
 *     asm/rom_c9000/rom_d2d98.s Anim_Plasma -v
 *
 * SPLIT SHAPE.  asm/rom_c9000/rom_d2d98.s holds SIX functions in this order --
 * Anim_Nereid, Anim_Froth, Anim_Whirlwind, Anim_Prism, ColorCycleVFXPalette,
 * Anim_Plasma.  Anim_Plasma is the SIXTH and last.  Anim_Prism (parked beside
 * this file as park_Anim_Prism.c) is the FOURTH, so the two together are a
 * FOUR-WAY split of one stem and must be landed in one edit:
 *
 *   asm/rom_c9000/rom_d2d98_a.s   Nereid + Froth + Whirlwind + ALL .rodata
 *   src/rom_c9000/rom_d2d98_b.c   Anim_Prism
 *   asm/rom_c9000/rom_d2d98_c.s   ColorCycleVFXPalette
 *   src/rom_c9000/rom_d2d98_d.c   Anim_Plasma  (THIS FILE)
 *
 * and stage1.ld's `.text` and `.rodata` lines for the stem each become four, in
 * that order.  `tools/datacheck.py asm/rom_c9000/rom_d2d98.s` reports a .rodata
 * section with NO already-global labels, so both split-out C files need new
 * exports in the asm data piece:
 *   Anim_Plasma: `.global .Lee244 .Lee250 .Lee25e`
 *   Anim_Prism:  `.global .Lee1d3 .Lee1f5 .Lee1fb .Lee207 .Lee214`
 * ColorCycleVFXPalette reads no data label, so its piece needs none.
 * All the data stays in the asm object -- no .rodata is emitted from C and
 * objcmp's SIZE line therefore carries no false positive.
 *
 * ================================================================
 * WHAT CLOSED IT, BY PASS -- 380 -> 377 -> 378, aligned 337 -> 395 -> 406
 * ================================================================
 *
 * Pass 1 (pla1.c) read 380 of 493 at 489 encodings, with the first difference
 * only at index 42 -- the whole prologue, both BuildDraw2DFuncEx calls, both
 * LoadVFXFile calls, the GetFile/Func_8001af8 copy and the ewram_2010018 clear
 * loop exact on the first try, off the same oracles as Anim_Ray plus
 * Anim_Break's `int q0` / `copy = Func_8001af8;` shape for the palette DMA.
 *
 * Pass 2 (pla2.c), aligned 337 -> 395.  THE `t0 + 4` BOUND IS NOT A VARIABLE, IT
 * IS gcse's PRE PSEUDO.  The ROM assigns `sl` three times with the same value
 * (`adds r0,r5,#4 / mov sl,r0`, twice more) and compares it twice, which reads as
 * a source variable and is not one: `t0 + 4` appears in `frame < t0 + 4` and
 * `frame == t0 + 4` on several paths and gcc-2.96's partial-redundancy pass
 * INSERTS the expression on the edges where it is not available.
 *
 * > THREE STORES OF ONE VALUE INTO ONE REGISTER, ON THREE DIFFERENT PATHS, IS
 * > PRE INSERTING AN EXPRESSION -- NOT A VARIABLE BEING RE-ASSIGNED.  A source
 * > variable was worth 58 encodings of damage here, because a declared local also
 * > claims a frame slot and displaced the whole temp map.
 *
 * Removing it put `frame` in fp, `lim` in sl, `i` in r8, the burst loop's
 * `base + 0x7828` in r9 and `cnt` in memory at sp+0x10 -- every high register
 * and the spilled counter, in one edit.  The same pass folded
 * `mag = (Random() & 0x3ff) + 0x20` back into ONE expression; as two statements
 * the `+ 0x20` is born before the sin call instead of being scheduled past it.
 *
 * Pass 3 (pla5.c, this file), aligned 395 -> 406.  `px = d->x >> 16` must be
 * computed BEFORE the guard, not inside it:
 *
 *     } else {
 *         int px = d->x >> 16;
 *         if (d->x >= 0 && px <= 0x77 && d->y >= 0) { ... }
 *     }
 *
 * The ROM has `ldr r3,[r6] / asr r7,r3,#16 / cmp r3,#0 / blt`, i.e. the shift
 * ahead of the sign test it feeds.  Note also that this loop's guard is THREE
 * SIGNED TESTS (`x >= 0`, `x >> 16 <= 0x77`, `y >= 0`) where Anim_Ray's
 * corresponding loop is a single UNSIGNED `(unsigned)x <= 0x7effff` -- the two
 * functions share the loop body and not its guard, and copying one onto the
 * other is wrong.
 *
 * ================================================================
 * THE BLOCKER: -2 INSTRUCTIONS, BOTH IN THE SCREEN-SHIFT BLOCK, BOTH RELOAD
 * ================================================================
 *
 * 1. THE ROM RE-READS `*slot` AND WE CACHE IT (-2).
 *
 *        ROM   ldr r0,[sp,#0x14] / ldr r3,[r0] / ldr r3,[r3,#4]
 *        ours  ldr r3,[r0,#4]
 *
 *    The address `base + 0x7828` is hoisted into sp+0x14 in the frame loop's
 *    preheader in both; the ROM then reloads the State pointer for the `f4` test
 *    although the `f18` test six insns earlier already had it and nothing between
 *    them stores.  cse DOES common the two loads -- what differs is that in the
 *    ROM the commoned pseudo lost its register and reload rematerialised it from
 *    its REG_EQUIV memory.  That is pressure, not aliasing, and no spelling of
 *    the two reads reaches it: measured as an inline double derivation, as a
 *    declared `State **slot;` assigned at the top of the frame-loop body
 *    (Anim_Break's shape -- 406 unchanged, and 388 when combined with pass 3's
 *    earlier revision), and with the `iwram_3001e80` load moved either side.
 *
 * 2. THE ROM COPIES `frame` OUT OF fp TWICE AND WE INHERIT ONE COPY (-1).
 *
 *        ROM   mov r5,fp / cmp r5,#0x37 / ble / mov r7,fp / mov r3,#0xb0 /
 *              lsl r2,r7,#3
 *        ours  mov r3,fp / cmp r3,#0x37 / ble / lsl r2,r3,#3 / movs ..,#0xb0
 *
 *    `frame` lives in a HIGH register, so Thumb needs a low-register copy for
 *    both the `cmp #imm` and the `lsl`; the two uses are in DIFFERENT basic
 *    blocks and the ROM's reload did not carry the first copy across the label
 *    while ours did.  Reload inheritance across a CODE_LABEL is not reachable
 *    from the source.
 *
 * The two together are -3 and something else is +1, for the net -2.
 *
 * ALSO OPEN, count-neutral: THE FOUR COMPILER-TEMP SLOTS ARE IN EXACTLY THE
 * OPPOSITE ORDER.
 *
 *        ROM   0x08 n*8   0x0c &fns[0]   0x10 cnt    0x14 base+0x7828
 *        ours  0x08 &fns[0]  0x0c base+0x7828  0x10 n*8   0x14 cnt
 *
 * The six DECLARED locals are right (fns 0x2c/0x30, base 0x28, ctx 0x24,
 * lane 0x20, gfx 0x1c, n 0x18, in descending declaration order exactly as
 * Anim_Flare recorded), and `base` living in a SPILL SLOT rather than a register
 * is the ROM's own choice, reproduced.  But `cnt` sits BELOW the gcse pseudo in
 * the ROM, which by the declaration-order rule says `cnt` is not a declared local
 * at all -- and it is a counter, so it must be.  Block-scoping it inside the burst
 * (pla4.c) is byte-identical, so the rule's direction is not settled by this
 * function; recorded as a caution rather than a finding.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "file_table.h"

typedef int (*DrawFn)(void *ctx, void *src, int x, int y, int w, int h);
typedef void (*CopyFn)(void *dst, void *src, int len);
typedef void (*ClearFn)(void *dst, int len, int val);

typedef struct {
    int f0, f4, f8, fc, f10, f14, f18, f1c, f20;
    short ids[4];
} State;

typedef struct {
    int x, y, z, vx, vy, vz, t;
} Part;

extern int *iwram_3001eec[];
extern void *iwram_3001e80;
extern int ewram_2010018;
extern Part gBuffer[];
extern unsigned short Data_ede48[];
extern unsigned short Lee244[] __asm__(".Lee244");
extern unsigned char Lee250[] __asm__(".Lee250");
extern unsigned char Lee25e[] __asm__(".Lee25e");

extern void AnimStart(int n);
extern void AnimEnd(void);
extern void LoadVFXFile(int id, void *dst, int a, int b);
extern void *GetFile(int id);
extern void Func_8001af8(void *dst, void *src, int len);
extern int BuildDraw2DFuncEx(int idx, int a, int b, int c, int d);
extern void Func_80008d8(void *dst, int len, int val);
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
extern void UpdateScreenShake(int x, int y);
extern void Func_80cd52c(void);
extern void gfree(int tag);
extern void WaitFrames(unsigned int n);

void Anim_Plasma(void *context)
{
    DrawFn fns[2];
    u8 *base;
    void *ctx;
    int lane;
    u8 *gfx;
    int n;
    int cnt;
    char **tbl;
    char **pp;
    State **slot;
    Part *g;
    Part *d;
    int i;
    int frame;
    int arg;

    tbl = (char **)iwram_3001eec;
    pp = tbl;
    base = (u8 *)*pp++;
    ctx = (void *)*pp;
    gfx = (u8 *)tbl[2];
    slot = (State **)(base + 0x7828);
    *slot = (State *)context;
    AnimStart(1);
    REG_BLDALPHA = 0x1010;
    BuildDraw2DFuncEx(0x2e, 7, 7, 3, 3);
    fns[0] = (DrawFn)tbl[7];
    BuildDraw2DFuncEx(0x2f, 7, 7, 7, 2);
    fns[1] = (DrawFn)tbl[8];
    LoadVFXFile(FILE_d1, base, 1, 1);
    LoadVFXFile(FILE_73, gfx, 0, 0);
    if ((*slot)->f18 != 2) {
        void *data = GetFile(FILE_60);
        CopyFn copy;
        int q0 = 0xa0;
        copy = Func_8001af8;
        q0 <<= 19;
        copy((void *)q0, data, 0x80);
    }
    {
        int *p = &ewram_2010018;
        i = 0;
        do {
            i++;
            *p = 0;
            p = (int *)((char *)p + 0x1c);
        } while (i != (0x80 << 3));
    }
    *(int *)(base + (0xef << 7)) = 2;
    *(int *)(base + 0x7784) = 0x32;
    arg = 0x90;
    arg <<= 3;
    StartTask(Task_BlitAnim, arg);
    n = Lee25e[(*(State **)(base + 0x7828))->f18];
    frame = 0;
    if (n * 7 + 0x30 != 0) {
      do {
        if ((*(State **)(base + 0x7828))->f18 == 2 && frame <= 0x3f) {
            char *view = (char *)iwram_3001e80;
            int dv = 0x80 << 1;
            if (frame > 0x37) {
                dv = (0xb0 << 2) - frame * 8;
            }
            if ((*(State **)(base + 0x7828))->f4 == 1) {
                *(u16 *)(view + 0x36) = *(u16 *)(view + 0x36) - dv;
            } else {
                *(u16 *)(view + 0x36) = *(u16 *)(view + 0x36) + dv;
            }
        }
        if (frame == 0x20) {
            _Func_80bd7dc(0x86);
        }
        lane = 0;
        if (n != 0) {
            do {
                int t0 = lane * 8;
                if (frame == t0) {
                    ClearFn fill;
                    _PlaySound(0x86);
                    fill = Func_80008d8;
                    fill(ctx, 0x80 << 7, 0x10101010);
                }
                if (frame >= t0 && frame < t0 + 9) {
                    if (frame >= t0 + 1 && frame < t0 + 2) {
                        fns[0](ctx, base,
                               Lee250[lane + (*(State **)(base + 0x7828))->f4 * 7] - 0x18,
                               0, 0x30, 0x70);
                    }
                    if (frame >= t0 + 2) {
                        if (frame < t0 + 4) {
                            fns[0](ctx, base + (0xa8 << 5),
                                   Lee250[lane + (*(State **)(base + 0x7828))->f4 * 7] - 0x18,
                                   0, 0x30, 0x70);
                        }
                        if (frame == t0 + 2) {
                            cnt = 0;
                            i = 0;
                            g = gBuffer;
                            do {
                                if (g->t == 0) {
                                    int mag = (Random() & 0x3ff) + 0x20;
                                    int ang = (Random() & 0x7fff) - 0x4000;
                                    g->x = Lee250[lane + (*(State **)(base + 0x7828))->f4 * 7] << 16;
                                    g->y = 0xd0 << 15;
                                    g->vx = (sin(ang) * mag) >> 7;
                                    g->vy = -(cos(ang) * mag * 2) >> 7;
                                    g->t = (Random() & 7) + 0x20;
                                    cnt++;
                                    if (cnt == Lee244[(*(State **)(base + 0x7828))->f18 * 2]) {
                                        break;
                                    }
                                }
                                i++;
                                g++;
                            } while (i != (0x80 << 3));
                            *(int *)(base + 0x77a8) =
                                Lee244[(*(State **)(base + 0x7828))->f18 * 2 + 1];
                        }
                    }
                }
                if (frame == t0 + 4) {
                    i = 0;
                    if ((*(State **)(base + 0x7828))->f14 != 0) {
                        do {
                            _SetBattleActorKnockback((*(State **)(base + 0x7828))->ids[i], 1);
                            Func_80d6888((*(State **)(base + 0x7828))->ids[i], 7, 5, i, 8);
                            i++;
                        } while (i != (*(State **)(base + 0x7828))->f14);
                    }
                }
                lane++;
            } while (lane != n);
        }
        i = 0;
        d = gBuffer;
        do {
            if (d->t > 0) {
                d->t -= 1;
                Func_80e3908(d, 0x3c, 0x80 << 5);
                if (d->y > (0xd0 << 15)) {
                    d->vy = -d->vy / 2;
                } else {
                  int px = d->x >> 16;
                  if (d->x >= 0 && px <= 0x77 && d->y >= 0) {
                    int py = d->y >> 16;
                    int k = d->t / 8 + 1;
                    int w2 = k * 2;
                    fns[i & 1](ctx, gfx + Data_ede48[k - 1], px - k / 2, py - k, k, w2);
                  }
                }
            }
            i++;
            d++;
        } while (i != (0x80 << 3));
        UpdateScreenShake(8, 0x10);
        Func_80cd52c();
        *(int *)(base + 0x7824) = 1;
        WaitFrames(1);
        frame++;
      } while (frame != n * 7 + 0x30);
    }
    StopTask(Task_BlitAnim);
    gfree(0x2f);
    gfree(0x2e);
    AnimEnd();
}
