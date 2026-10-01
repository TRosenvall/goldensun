/* Func_80b9604 (FadeBattleMusicIn) -- NON-MATCHING, and a WHOLE-TU candidate:
 * Func_80b9724 with Func_80b9554 AND Func_80b9604 written as gcc NESTED
 * functions inside it. 0x080b9554 / 0x080b9604 / 0x080b9724, the last three of
 * the four functions in asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s
 * (Func_80b9470, parked separately, is the first; the .s ends with Func_80b9724's
 * pool, no data -- tools/datacheck.py is silent on it, so NO text/data split is
 * needed and the whole file converts in one commit if the TU lands).
 * Fresh in batch 287; improved in batch 314. Supersedes the r9-binding
 * transcription in src/non_matching/rom_b5000/80b9554.c.
 *
 * objcmp CANNOT SCORE THIS: the nested functions are emitted as the local
 * symbols `Func_80b9554.0` and `Func_80b9604.1`. Measured instead by assembling
 * the ROM's three functions (lines 122..end of the .s, with the two .include
 * lines) and this file, and diffing `objdump -d --no-show-raw-insn` with branch
 * targets, pool offsets and `bl` targets normalised. Under the PRODUCTION flags:
 *
 *     Func_80b9554    81 of 81 instructions, 0 differing lines  -- EXACT, nested
 *     Func_80b9604   129 against 131, 66 differing +/- lines
 *     Func_80b9724   182 against 181, 57 differing +/- lines
 *
 * tools/shimcount.py reports 0 pins, so a landing would need no fakematch.txt
 * row.
 *
 * Verify with (from the repo root; tools/objcmp.py has no mode for this):
 *   (head -2 asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s; \
 *    sed -n '122,$p' asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s) > /tmp/r3.s
 *   /opt/gcc296/xgcc -B/opt/gcc296/ -O2 -mthumb -mthumb-interwork -mcpu=arm7tdmi \
 *     -fno-builtin -nostdinc -ffreestanding -fcall-used-r4 -Iinclude -S \
 *     -o /tmp/c3.s src/non_matching/rom_b5000/80b9604.c
 *   printf '\n\t.text\n\t.align\t2, 0\n' >> /tmp/c3.s
 *   for f in r3 c3; do arm-none-eabi-as -mcpu=arm7tdmi -mthumb-interwork \
 *     -Iinclude -o /tmp/$f.o /tmp/$f.s; arm-none-eabi-objdump -d \
 *     --no-show-raw-insn /tmp/$f.o > /tmp/$f.txt; done; diff /tmp/r3.txt /tmp/c3.txt
 *
 * THE PARENT'S FRAME is the nested functions' chain: n at sp+0 (fp[-5]),
 * count sp+4 (fp[-4]), list sp+8 (fp[-3]), lim sp+0xc (fp[-2]), scratch
 * sp+0x10 (fp[-1]); r9 = sp+0x14 before each call. This source reproduces that
 * layout. The ROM's loop 1 reads `n` once and runs a dbra counter; that needs
 * struct E to hold NO `int` member (with one, the HImode stores alias `n` and n
 * is re-read every iteration). `u32 ime` (a u16 adds lsl/lsr before the
 * restore) and `rng = gRNGState` read once are both required.
 *
 * ===================================================================
 * Func_80b9604: THE PARK'S OLD BLOCKER DIAGNOSIS WAS WRONG, AND THE CORRECTION
 * IS WORTH 18 LINES. The old header said `t` lands in r6 because "r5 is skipped
 * as preferred by another pseudo in find_reg's first pass". It is not a
 * preference, it is a HARD-REGISTER CONFLICT, and `.18.greg` prints it:
 *
 *     ;; 37 conflicts: ... 0 1 2 3 5 13 14      <- 9604, hard reg 5 present
 *     ;; 37 conflicts: ... 0 1 2 3 13 14        <- 9554, no 5, and t IS in r5
 *
 * WHERE THE HARD r5 COMES FROM. 9604's loop 2 compares ewram_2002238 against a
 * limit whose computation calls __udivsi3, so the loaded halfword is live ACROSS
 * A CALL. Its pseudo is referenced in ONE basic block, so local-alloc owns it,
 * and find_free_reg for a call-crossing quantity excludes call_used_reg_set --
 * leaving r4 (call-used under -fcall-used-r4), r5, r6, r7, of which r7 is live
 * as the Thumb hard frame pointer in `.17.lreg`. So it takes r5, twice (once
 * per check site: `89 in 5`, `125 in 5` in the dispositions), and every global
 * live through those blocks -- t and c -- inherits a hard conflict with r5.
 * Everything else followed: t->r6, c->r7, fp->r5, &scratch->r8.
 *
 * WHAT PAID: naming the limit in a local (`u32 w`). The read of ewram_2002238
 * then happens AFTER __udivsi3 instead of before it, nothing is live across the
 * call, local-alloc never takes r5, and `t`/`c` land in r5/r6 exactly as the ROM
 * has them. 9604 goes 84 -> 68 differing lines, and loop 1 plus the
 * post-loop-1 check become instruction-for-instruction EXACT. Writing
 * `c = 0; t = 300;` rather than the reverse is a further 68 -> 66; it is inert
 * without the `w` lever, which is why the old park recorded it as inert.
 *
 * WHAT THE REMAINING 66 IS, AND IT IS ONE THING. The whole residue is that the
 * ROM puts the frame-pointer copy in a HI register:
 *
 *     ROM   t r5   c r6   &scratch r7   fp r8   &count r9   ewram-temp r8
 *     ours  t r5   c r6   fp r7         &scratch r8   &count r9   ewram-temp r3
 *
 * The opcode multiset says the same: ROM `mov r7, r8` / `add r3, r8` /
 * `mov r3, r8` / `mov r2, r8` against our `subs r2, r7, #4` /
 * `adds r1, r1, r7` / `adds r3, r7, #0` / `adds r2, r7, #0`, and the ROM's two
 * surplus instructions (131 against 129) are exactly its two extra
 * `mov r8, r3` copies of the ewram halfword into the register fp vacated.
 *
 * PRICED: fp's preferred class is LO_REGS and it cannot be anything else -- the
 * ONLY ref that penalises a hi register is the single `(plus fp -4)` that forms
 * &scratch, and `*thumb_addsi3`'s hi alternatives are `*`-marked so regclass
 * ignores them. So fp reaches r8 only through reg_alternate_class, i.e. only if
 * r5, r6 AND r7 all conflict at its turn -- which needs &scratch allocated
 * BEFORE fp. It is allocated after: the `.18.greg` order is
 * 39 43 79 85 37(t) 38(c) 77 67(&count) 36(fp) 41(&scratch), and on
 * floor_log2(n_refs)*n_refs/live_length fp (5 refs, ~60 insns) outranks
 * &scratch (3 refs, ~55) by a factor of three. Closing that needs &scratch to
 * carry FIVE references; 9604 reads `scratch` twice and the ROM shows no third
 * read. That is the wall.
 *
 * THE SAME WALL BLOCKS THE ROM'S EVALUATION ORDER. To read ewram first (as the
 * ROM does) the halfword must cross __udivsi3, and to avoid local-alloc's r5 it
 * must be a GLOBAL allocno -- which it becomes the moment one named variable
 * serves both loop-2 check sites (`;; 10 regs to allocate: 39 ...`, pseudo 39 is
 * it). But it is then allocated FIRST and takes r5 itself, putting t back in r6:
 * 4 refs over ~28 insns beats t's ~7 refs over ~105. Seven attempts to lower its
 * priority (u32/int/unsigned spellings, declaration first/last, t and c
 * initialised after the first call, `c`-before-`t`, a comma-expression decrement,
 * sharing the variable with the post-loop-1 check) were all BYTE-IDENTICAL to
 * the 84 baseline. Covering all four ewram sites with it lands the variable in
 * a hi register but costs instructions in loop 1, which the ROM does not pay:
 * 132 against 131, 103 lines.
 *
 * ALSO MEASURED AND REJECTED, all with the same normalisation:
 *   `if (ewram_2002238 > (w = ...))` 84 (the assignment-expression restores the
 *   ROM's order and with it the r5 theft); `int w` 70; a second variable for the
 *   halfword alongside `w` 84; `v`-only-at-one-site 84; `count = scratch[0]`,
 *   `&scratch[0]`, `&list[n]`, `((u32)count * 16 + ...)`, `if (scratch[0] != 0)`,
 *   and four declaration orders all inert at 66/68.
 *   `if (*scratch != 0) { count = *scratch; ... }` measures 63 but is a FALSE
 *   improvement: it reads *scratch once where the ROM reads it twice
 *   (`ldr r2, [r3]` then `ldr r3, [r3]`), and the count falls to 126.
 *   `(unsigned)(*scratch * 16 + ...)` 127/84.
 *
 * ===================================================================
 * Func_80b9724: 182 against 181, 57 differing lines, and the park's allocation
 * diagnosis survives re-derivation. The ROM keeps g (iwram_3001e74) in r9 and
 * gives `&lim` NO register at all (`str r3, [sp, #0xc]` -- the pseudo went
 * unallocated and reload substituted its REG_EQUIV frame address); we give
 * `&lim` r5 and g r6. The ROM therefore holds EIGHT callee-saved values plus the
 * constant 1 hoisted into r0, and we hold seven and rematerialise the 1 inside
 * loop 1 -- one from the pool (`ldr r2, [pc]` + `.word 1`) in the `|= 1` arm and
 * `movs r3, #1` in the `& 1` arm. THE TWO ARE THE SAME FACT: the ROM's cse
 * commons the two constant-1 uses into one pseudo with savings 2, `.08.loop`
 * hoists it, pressure rises to nine and `&lim` loses its register; ours keeps
 * two uncommoned constants with savings 1 each, nothing hoists, and r5 is free
 * for `&lim`. Naming the constant forces the hoist and is worse every way
 * (`u16 one` 189/122, `int one` and `u32 one` 185/94), because a named value is
 * preserved across the use and costs a copy the ROM does not pay.
 *   Loop 2's 0x80 is the same shape: the ROM hoists it as a POOL LOAD
 *   (`ldr r6, .Lb987c` + `eors r3, r6`), and naming it gives `movs r6, #128`
 *   plus a preserving copy -- 181 against 181, an EXACT COUNT that measures 66
 *   instead of 57. Recorded as the cleanest instance in this corpus of the
 *   count-is-blind-to-position rule.
 *   ALSO MEASURED: `if (g[0x50] == 0) {...} else e->f4 |= 1;` 189/112 (the park
 *   predicted a spill and it is worse than that); `&list[i]` for loop 1 189/78;
 *   a hoisted `gp = g + 0x50` 186/145; `&list[n]` walked in loop 2 182/137;
 *   `scratch = Func_8004970(0x28)` before the lim computation 191/146.
 *   INERT at 57: `e->f4 = e->f4 | 1`, `e->f4++`, `(e->f4 & 1) != 0`, `u32 lim`,
 *   and three declaration orders for `lim`/`count`/`scratch` (the frame layout
 *   does not move).
 *
 * ===================================================================
 * FLAGS, SWEPT ON THE WHOLE TU FOR THE FIRST TIME (the old park had no flag
 * figures at all). Totals are 9554 + 9604 + 9724 differing lines; the default is
 * 123. NOTHING BEATS THE DEFAULT, and the three that move 9604's count move it
 * the wrong way:
 *   -fno-gcse 281 (9604 127 insns, 9724 collapses to 164); -fno-rerun-cse-after-
 *   loop 246; -fno-strength-reduce 225; -fno-rerun-loop-opt 223;
 *   -fno-expensive-optimizations 217; -fno-schedule-insns2 177 (and it breaks
 *   9554's exactness, 10 lines); -fno-regmove = -fno-optimize-register-move 157
 *   (also breaks 9554, 8 lines); -fno-peephole 147; -fno-force-mem 327.
 *   INERT: -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-thread-jumps,
 *   -fomit-frame-pointer, -fno-delayed-branch, -fno-caller-saves, -fcaller-saves,
 *   -fno-function-cse, -fno-inline, -fno-defer-pop.
 *   -fno-if-conversion and -fno-cprop-registers do not exist in this cc1.
 * And sched1 DOES NOT RUN here: the -da sequence is 17.lreg 18.greg 19.flow2
 * 20.ce2 23.sched2 25.jump2 26.mach. Nothing above is a pre-reload scheduler
 * effect.
 */
#include "gba/types.h"
#include "gba/io.h"

struct E {
    short f0;
    short f2;
    unsigned short f4;
    short f6;
    short f8;
    unsigned short fa;
    short fc;
    short fe;
};

extern unsigned char *iwram_3001e74;
extern unsigned short iwram_3001f64;
extern unsigned short ewram_2002238;
extern unsigned int gRNGState;
extern unsigned int sRPGRNGState;
extern int Func_80063bc(int a, int b);
extern int Func_8006408(int p);
extern unsigned int Func_80064f4(void);
extern int WaitFrames(int frames);
extern void *Func_8004970(int size);
extern int _RPGRandom(void);
extern void Func_800651c(void);
extern void Func_8006358(void);
extern void free(void *p);

int Func_80b9724(struct E *list, int n)
{
    int *scratch;
    int lim;
    int count;
    unsigned char *g;
    struct E *e;
    int i;
    u32 ime;
    u32 rng;

    int Func_80b9554(void)
    {
        int t;
        int c;

        if (Func_80063bc((int)scratch, 0x14) == -1)
            return -1;
        c = 0;
        t = 300;
        while (Func_80064f4() != 0) {
            WaitFrames(1);
            if (--t < 0)
                return -1;
            if ((iwram_3001f64 & 3) != 3) {
                if (++c > 0x18)
                    return -1;
            } else {
                c = 0;
            }
        }
        if (lim != 0) {
            if (Func_80063bc((int)list, lim) == -1)
                return -1;
            while (Func_80064f4() != 0) {
                WaitFrames(1);
                if (--t < 0)
                    return -1;
                if ((iwram_3001f64 & 3) != 3) {
                    if (++c > 0x18)
                        return -1;
                } else {
                    c = 0;
                }
            }
        }
        return 0;
    }

    int Func_80b9604(void)
    {
        int t;
        int c;
        u32 w;

        c = 0;
        t = 300;
        if (Func_8006408((int)scratch) == -1)
            return -1;
        while (Func_80064f4() != 0) {
            if (ewram_2002238 > 0x14)
                return -1;
            WaitFrames(1);
            if (--t < 0)
                return -1;
            if ((iwram_3001f64 & 3) != 3) {
                if (++c > 0x18)
                    return -1;
            } else {
                c = 0;
            }
        }
        if (ewram_2002238 != 0x14)
            return -1;
        count = *scratch;
        if (*scratch != 0) {
            if (Func_8006408((int)(list + n)) == -1)
                return -1;
            while (Func_80064f4() != 0) {
                w = (unsigned)(count * 16 + 0x13) / 0x14 * 0x14;
                if (ewram_2002238 > w)
                    return -1;
                WaitFrames(1);
                if (--t < 0)
                    return -1;
                if ((iwram_3001f64 & 3) != 3) {
                    if (++c > 0x18)
                        return -1;
                } else {
                    c = 0;
                }
            }
            w = (unsigned)(count * 16 + 0x13) / 0x14 * 0x14;
            if (ewram_2002238 != w)
                return -1;
        }
        return 0;
    }

    g = iwram_3001e74;
    count = 0;
    lim = (unsigned)(n * 16 + 0x13) / 0x14 * 0x14;
    scratch = Func_8004970(0x28);
    for (i = 0, e = list; i < n; i++, e++) {
        e->f2 = g[e->f0 + 0x48];
        if (g[0x50] != 0)
            e->f4 |= 1;
        else if (e->f4 & 1)
            e->f4 = e->f4 + 1;
    }
    if (g[0x52] != 0)
        goto fail;
    if (g[0x50] == 0) {
        scratch[0] = n;
        scratch[1] = _RPGRandom();
        ime = REG_IME;
        SET_IO(REG_IME, REG_ADDR_IME);
        rng = gRNGState;
        scratch[2] = rng;
        sRPGRNGState = rng;
        SET_IO(REG_IME, ime);
        if (Func_80b9554() < 0)
            goto fail;
        if (Func_80b9604() < 0)
            goto fail;
        count = scratch[0];
    } else {
        if (Func_80b9604() < 0)
            goto fail;
        count = scratch[0];
        scratch[0] = n;
        if (Func_80b9554() < 0)
            goto fail;
        if (_RPGRandom() != scratch[1])
            goto fail;
        sRPGRNGState = scratch[2];
    }
    for (i = 0; i < count; i++) {
        e = &list[n + i];
        e->f0 = e->f2;
        e->fa ^= 0x80;
    }
    free(scratch);
    return count;
fail:
    Func_800651c();
    Func_8006358();
    free(scratch);
    return -1;
}
