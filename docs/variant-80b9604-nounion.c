/* UNION-FREE FALLBACK of 80b9604.c, two differing lines worse (42, not 40).
 * Ship this one if the alias-set union in the main candidate reads as contrived.
 *
 * Func_80b9604 (FadeBattleMusicIn) -- NON-MATCHING, and a WHOLE-TU candidate:
 * Func_80b9724 with Func_80b9554 AND Func_80b9604 written as gcc NESTED
 * functions inside it. 0x080b9554 / 0x080b9604 / 0x080b9724, the last three of
 * the four functions in asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s
 * (Func_80b9470, parked separately, is the first; the .s ends with Func_80b9724's
 * pool, no data -- tools/datacheck.py is silent on it, so NO text/data split is
 * needed and the whole file converts in one commit if the TU lands).
 * Fresh in batch 287; improved in batches 314 and 315.  Supersedes the
 * r9-binding transcription in src/non_matching/rom_b5000/80b9554.c.
 *
 * objcmp CANNOT SCORE THIS: the nested functions are emitted as the local
 * symbols `Func_80b9554.0` and `Func_80b9604.1`. Measured instead by assembling
 * the ROM's three functions (lines 122..end of the .s, with the two .include
 * lines) and this file, and diffing `objdump -d --no-show-raw-insn` with branch
 * targets, pool offsets and `bl` targets normalised. Under the PRODUCTION flags:
 *
 *     Func_80b9554    81 of 81 instructions, 0 differing lines  -- EXACT, nested
 *     Func_80b9604   129 against 131,  66 differing +/- lines
 *     Func_80b9724   183 against 181,  42 differing +/- lines   (was 57)
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
 * THE ALLOCATION MODEL FOR THIS WHOLE TU IS `allocno_compare`, AND IT IS
 * EXACTLY VERIFIABLE OFF `.17.lreg`.  Priority is
 *
 *     floor_log2(n_refs) * n_refs / live_length
 *
 * and `.17.lreg` prints both inputs for every pseudo as
 * `Register N used R times across L insns`.  Sorting that expression reproduces
 * `.18.greg`'s `;; N regs to allocate:` line EXACTLY on BOTH 9604 and 9724, so
 * any claim about "why this register" on this TU is checkable in one dump pass
 * rather than argued.  Use it before writing any allocation story here.
 *
 * ===================================================================
 * Func_80b9604: 129 against 131, 66 differing, and THE PARK'S PRICING OF THE
 * WALL WAS WRONG BY A FACTOR OF TWO.  The whole residue is still one fact --
 * the ROM puts the static-chain copy in a HI register:
 *
 *     ROM   t r5   c r6   &scratch r7   fp r8   &count r9   ewram-temp r8
 *     ours  t r5   c r6   fp r7         &scratch r8   &count r9   ewram-temp r3
 *
 * and the ROM's two surplus instructions (131 against 129) are exactly its two
 * extra `mov r8, r3` copies of the ewram halfword into the register fp vacated.
 * The 66 breaks down as prologue 12, the -1 constant in r2 against r3 12 (two
 * sites), the `count = *scratch` block 12, `mov r3,r8` against `adds r3,r7,#0`
 * 4, and the ewram temp 26 (two sites).
 *
 * `.17.lreg` prices the flip:
 *
 *     36 (fp)        5 refs / 38 insns   pri = 2*5/38 = 0.2632
 *     41 (&scratch)  3 refs / 29 insns   pri = 1*3/29 = 0.1034
 *     67 (&count)    6 refs / 45 insns   0.2667
 *     37 (t) 19/148 0.5135   38 (c) 17/150 0.4533   77 2/5 0.4
 *     39 8/8 3.0   43 3/3, 79 2/2, 85 3/3 all 1.0 (tie -> allocno order)
 *
 * which is `;; 10 regs to allocate: 39 43 79 85 37 38 77 67 36 41` to the letter,
 * with dispositions `36 in 7  37 in 5  38 in 6  41 in 8  67 in 9`.  fp is
 * allocated one place BEFORE &scratch and takes r7; &scratch then finds r5, r6,
 * r7, r9 gone and takes r8.  Swap those two turns and the ROM's assignment
 * falls out, because &scratch's own conflict set already forbids everything but
 * r7 at that point.
 *
 * THE PARK SAID &scratch NEEDS FIVE REFERENCES.  It does not -- that figure came
 * from guessed live lengths of ~60 and ~55.  The real lengths are 38 and 29, and
 * &scratch has the SHORTER range, so FOUR references suffice: 2*4/29 = 0.2759
 * beats fp's 0.2632.  The model admits five routes and prices each:
 *
 *     R1  &scratch 3 -> 4 refs with live_length <= 30   (8/LL > 0.2632)
 *     R2  fp 5 -> 4 refs (pri 0.2105), then &scratch needs 4 refs, LL < 38
 *     R3  fp 5 -> 3 refs  -> pri 0.0789 < 0.1034, no change to &scratch at all
 *     R4  live_length(fp)      > 96.7   (it is 38, in a 129-insn function)
 *     R5  live_length(&scratch) < 11.4  (it is 29, spanning loop 2)
 *
 * R4 and R5 are arithmetically out of reach.  R3 would need two of fp's four
 * uses to disappear, and they are &scratch, &count, &n and &list -- all four are
 * distinct `(plus fp k)` insns that C cannot merge.  R1/R2 need a FOURTH
 * reference to `scratch` inside its existing 29-insn window; the ROM reads the
 * frame slot exactly twice (`ldr r0,[r7,#0]` then `ldr r3,[r7,#0]`, with both
 * derefs of the count word hanging off the second), so every spelling that adds
 * one also adds an instruction the ROM does not have.  THAT, and not a
 * reference count of five, is the wall.
 *
 * Also note: the ROM's own fp and &scratch have the SAME 5 and 3 references as
 * ours, so the ROM's compilation must have differed in LIVE LENGTH, not in
 * reference count.  Whatever source shape lengthened fp's range past 96 insns
 * (or shortened &scratch's below 12) is the thing still to find; no spelling
 * tried so far moves either number.
 *
 * WHAT PAID EARLIER AND IS KEPT: naming the limit in a local (`u32 w`), which
 * stops the ewram halfword crossing __udivsi3 (84 -> 68), and writing
 * `c = 0; t = 300;` rather than the reverse (68 -> 66; inert without `w`).
 *
 * MEASURED AGAINST THIS BASELINE AND INERT: `scratch[0]` for `*scratch`,
 * `(int)&list[n]` and `(int)list + n*16` for `list + n`, `-1 ==` on both
 * comparisons, and every extern return-type respelling (below).  NEGATIVE:
 * `-fno-strict-aliasing` takes 9604 to 132/61 -- better on 9604 alone -- but it
 * is not a production flag, it does NOT flip fp out of r7, and it costs 9724 108
 * lines.  The batch-314 list (`if (ewram_2002238 > (w = ...))` 84, `int w` 70, a
 * second halfword variable 84, `count = scratch[0]`, `&scratch[0]`, `&list[n]`,
 * `((u32)count * 16 + ...)`, `if (scratch[0] != 0)` and four declaration orders)
 * was re-crossed and still reads the same.
 *
 * ===================================================================
 * Func_80b9724: 183 against 181, 40 differing -- down from 57, and the park's
 * own diagnosis is what came true.  The ROM keeps g (iwram_3001e74) in r9 and
 * gives `&lim` NO register (`str r3,[sp,#0xc]`), because its loop 1 holds NINE
 * callee-saved values: the ninth is the constant 1, hoisted out of the loop.
 * THREE SOURCE FACTS GET THAT HOIST, AND NONE OF THE THREE WORKS ALONE.
 *
 * (1) THE TWO CONSTANT-1 USES MUST SHARE A MODE.  `.08.loop` shows
 *
 *         (insn 161 (set (reg:HI 103) (const_int 1)))    <- the `|= 1` arm
 *         (insn 189 (set (reg:SI 116) (const_int 1)))    <- the `& 1` arm
 *
 *     `combine_movables` merges two movables only when their `set_dest` modes
 *     match, so these never combine, each keeps savings 1, and `move_movables`'
 *     `savings > 1` test fails.  `e->f4 |= 1` is HImode because
 *     `convert_to_integer` distributes the truncation down through BIT_IOR_EXPR;
 *     `if (e->f4 & 1)` is a truth value, fold strips the conversion, and the and
 *     stays SImode -- which also costs us a POOL WORD for the constant
 *     (`ldrh r2,.Lpool` + `.word 1`), two differing encodings that are not code.
 *     A CAST WILL NOT NARROW IT.  `(unsigned short)(e->f4 & 1)`, the same `!= 0`,
 *     `== 1`, `& 1u`, `e->f4 % 2` and an `unsigned short m = e->f4 & 1;`
 *     INITIALISATION are all byte-identical to the old baseline; `.00.rtl` still
 *     shows `(and:SI (subreg:SI (reg:HI N)) (reg:SI M))`.  THE CONSTRUCT THAT
 *     NARROWS IS THE COMPOUND ASSIGNMENT ITSELF -- `m = e->f4; m &= 1;`.
 *
 * (2) THE ARMS MUST BE IN THE ROM'S ORDER, `if (g->b[0x50] == 0) {...} else
 *     e->f4 |= 1;`, so the mask arm falls through and the ior arm sits out of
 *     line above the shared `strh`.
 *
 * (3) `g` MUST LEAVE ALIAS SET 0.  `unsigned char *` is alias set 0 and
 *     conflicts with everything, which is what pins g in a lo register; a real
 *     `struct G { unsigned char b[0x54]; } *` lets g reach r9 as the ROM has it.
 *
 * EACH OF THE THREE MEASURES NEGATIVE ON ITS OWN: the arm swap alone 189/181 and
 * 112 (the park's own figure), the compound mask alone 184/181 and 69, `g` as a
 * struct alone 185/181 and 142.  Swap + mask together are 183/181 and 48; adding
 * the struct gives 42.  A one-at-a-time sweep scores all three as regressions,
 * so A REJECTED-AS-NEGATIVE LIST ON THIS TU HAS TO BE RE-CROSSED, not re-run.
 *
 * (4) THE LAST TWO CAME FROM A UNION.  The ROM holds &sRPGRNGState in r4 across
 *     the gRNGState read; sched2 instead pulls our volatile `ime = REG_IME` load
 *     in front of the `scratch[1] = _RPGRandom()` store, because an `int` store
 *     and a `short` volatile load do not conflict and the alias check runs
 *     before volatility.  The aliasing store is EARLIER in the chain than the
 *     load, which is the precondition for adding a dependent, and a UNION's
 *     alias set conflicts with every member's: declaring the device buffer
 *     `union SU { int i; unsigned short h; } *` is worth 42 -> 40.  The member
 *     `h` is never read; it is there for the alias set.  IF THAT READS AS TOO
 *     CONTRIVED TO SHIP, drop the union and the `.i` suffixes and the TU is at
 *     42 instead of 40 with no other change.
 *
 * WHAT IS LEFT IN THE 40: `&lim` still gets r5 and an `add r5,sp,#12` the ROM
 * does not pay (the ROM stores straight to `[sp,#12]`); the ROM re-loads
 * `g->b[0x50]` every iteration where our typed struct lets gcc hoist the LOAD as
 * well as the address (same instruction count, 5 differing lines); and the ROM's
 * mask is `1 & f4` (`adds r3,r0,#0` + `ands r3,r2`) where ours is `f4 & 1`
 * (`ands r3,r0`), one instruction fewer.  `unsigned short m = 1; m &= e->f4;`
 * flips the operand order but collapses the function to 177/181 and 140.
 *
 * ALSO MEASURED AND REJECTED on the 42/40 baseline: `((unsigned char *)g)[0x50]`
 * and `*(unsigned char *)&g->b[0x50]` to put that one reference back in alias
 * set 0, both 183/48; a `struct G` with a real `short f52` 183/68; a hoisted
 * `gp = g + 0x50` 187/118; `lim` computed after the allocation 189/132; `lim`
 * assigned through `int *lp`, in two steps, as `u32`, or declared last, all
 * inert; `count = 0` before `g = ...` 50; `g = ...` last 54;
 * `e->f2 = g->b[gi]` with the index in a local, `g->b[0x48 + e->f0]`, the
 * `sRPGRNGState`/`scratch[2]` store order, `int` for ime/rng, and
 * `*(volatile short *)REG_ADDR_IME` for the IME read all inert or worse.
 * The batch-314 list for 9724 was re-crossed and still reads the same.
 *
 * ===================================================================
 * LEVER 3 DOES NOT REACH THIS TU.  Every extern in this file was respelled and
 * measured, twice -- once on the batch-314 baseline and once on this one:
 * WaitFrames as `void` and as `unsigned int`, free as `int` and as
 * `void free(int *)`, Func_800651c and Func_8006358 as `int`, _RPGRandom as
 * `unsigned int`, Func_80064f4 as `int` and `signed int`, Func_8006408 as `long`
 * and `unsigned int`, Func_8004970 as `char *`, as `int` with a cast, and with
 * an `unsigned int` parameter, Func_80063bc as `signed long`, and three
 * combinations of those.  ALL BYTE-IDENTICAL.  None of these symbols appears in
 * include/, and src/ carries both `void` and `int` spellings of WaitFrames,
 * Func_80063bc and Func_8006408, so there is no header to settle them against.
 *
 * FLAGS, RE-SWEPT ON THIS BASELINE (the batch-314 sweep was on the old one).
 * Totals are 9554 + 9604 + 9724; the default is 106 and NOTHING BEATS IT:
 *   -fno-schedule-insns2 154 (and it breaks 9554's exactness, 10 lines);
 *   -fno-regmove 146 (also breaks 9554, 8 lines); -fno-peephole 114;
 *   -fno-gcse 233; -fno-rerun-cse-after-loop 215; -fno-strength-reduce 158;
 *   -fno-rerun-loop-opt 176; -fno-expensive-optimizations 122;
 *   -fno-force-mem 173; -fmove-all-movables 175 (it hoists, but it also wrecks
 *   9554 and 9604); -freduce-all-givs 110; -fno-strict-aliasing 211.
 *   INERT: -fno-cse-follow-jumps, -fno-cse-skip-blocks, -fno-thread-jumps,
 *   -fno-caller-saves, -fcaller-saves, -fno-function-cse, -fno-delayed-branch,
 *   -fno-inline-functions.
 *   -fno-if-conversion, -fno-cprop-registers and -fno-alias-check do not exist
 *   in this cc1.
 * And sched1 DOES NOT RUN here: the -da sequence is 17.lreg 18.greg 19.flow2
 * 20.ce2 23.sched2 25.jump2 26.mach.
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

struct G { unsigned char b[0x54]; };
extern struct G *iwram_3001e74;
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
    struct G *g;
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
        count = scratch[0];
        if (scratch[0] != 0) {
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
        e->f2 = g->b[e->f0 + 0x48];
        if (g->b[0x50] == 0) {
            unsigned short m = e->f4;
            m &= 1;
            if (m != 0)
                e->f4 = e->f4 + 1;
        } else {
            e->f4 |= 1;
        }
    }
    if (g->b[0x52] != 0)
        goto fail;
    if (g->b[0x50] == 0) {
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
