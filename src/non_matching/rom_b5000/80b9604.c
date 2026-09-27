/* Func_80b9604 (FadeBattleMusicIn) -- NON-MATCHING, and a WHOLE-TU candidate:
 * Func_80b9724 with Func_80b9554 AND Func_80b9604 written as gcc NESTED
 * functions inside it. 0x080b9554 / 0x080b9604 / 0x080b9724, the last three of
 * the four functions in asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a_c_c_c.s
 * (Func_80b9470, parked separately, is the first; the .s ends with Func_80b9724's
 * pool, no data). Fresh in batch 287; supersedes the r9-binding transcription in
 * src/non_matching/rom_b5000/80b9554.c, whose header predicted exactly this.
 *
 * objcmp CANNOT SCORE THIS: the nested functions are emitted as the local
 * symbols `Func_80b9554.0` and `Func_80b9604.1`. Measured instead by assembling
 * the ROM's three functions (lines 122..end of the .s, with the two .include
 * lines) and this file, and diffing `objdump -d --no-show-raw-insn` with branch
 * targets, pool offsets and `bl` targets normalised:
 *
 *     Func_80b9554   81 of 81 instructions, 0 differing lines  -- EXACT, nested
 *     Func_80b9604  129 against 131, every residue a register-assignment one
 *     Func_80b9724  182 against 181, 57 differing +/- lines
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
 * BLOCKER, Func_80b9604: global allocation. The ROM gives t r5, c r6, the
 * &scratch pseudo r7 and the frame-pointer copy r8; we give fp r5, t r6, c r7,
 * &scratch r8, and everything after follows (the ROM's two extra instructions
 * are `mov r8, r3` copies of the ewram_2002238 value into the reg fp vacated).
 * .18.greg allocates in the order 42 78 84 37(t) 38(c) 76 36(fp) 66 40, yet t
 * lands in r6: r5 is skipped as "preferred by another pseudo" in find_reg's
 * first pass. Inert: `&list[n]`, t/c initialised after the first call, parent
 * declaration order. `if ((count = *scratch) != 0)` is worse (118 lines).
 * Needed and found: `if (*scratch != 0) { ... } return 0;` like Func_80b9554's
 * `if (lim != 0)`, not an early `return 0` (the shared `mov r0, #0`).
 *
 * BLOCKER, Func_80b9724: also allocation. The ROM keeps g (iwram_3001e74) in
 * r9 and `&lim` in NO register (`str r3, [sp, #0xc]` -- the pseudo went
 * unallocated and reload substituted its REG_EQUIV frame address); we put g in
 * r6, give &lim r5 (`add r5, sp, #12 / str r3, [r5]`), and loop 1 then fails to
 * hoist the constant 1 (ours loads it from the pool). The ROM also tests
 * g[0x50] with `bne` to the `|= 1` arm, i.e. `if (g[0x50] == 0) {...} else
 * e->f4 |= 1;` -- but that spelling here costs a spill slot (frame 0x18,
 * 112 lines) until the allocation is right, so the file keeps the other order.
 * Loop 1 walking `e` is better than `&list[i]` (57 vs 78); loop 2 must index
 * `&list[n + i]` so the base is formed after the count test.
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

        t = 300;
        c = 0;
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
                if (ewram_2002238 > (unsigned)(count * 16 + 0x13) / 0x14 * 0x14)
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
            if (ewram_2002238 != (unsigned)(count * 16 + 0x13) / 0x14 * 0x14)
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
