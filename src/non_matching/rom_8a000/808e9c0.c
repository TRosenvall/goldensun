/* Func_808e9c0  --  asm/rom_8a000/rom_8d9a4_a_c_a_c_c.s  (0x0808e9c0)
 *
 * NON-MATCHING: 251 encodings of 276 differ (objcmp).
 * Working distance: 194 instructions in disagreeing regions of 289 (tryc --align).
 * NOT a true distance: length is 273 against the ROM's 289, sixteen instructions short, and
 * the shortfall has ONE named cause -- see CROSS-JUMP below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_8a000/808e9c0.c asm/rom_8a000/rom_8d9a4_a_c_a_c_c.s --whole
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/tryc.py src/non_matching/rom_8a000/808e9c0.c --ref asm/rom_8a000/rom_8d9a4_a_c_a_c_c.s --align
 *
 * Whole-file conversion: one function, no data. No pins, no flags, no volatile.
 * Shims: ZERO of my own. DMA3_CLEAR from include/dma.h carries the tree's standard
 * register-asm block; the ROM's `add r0,sp,#8 / str r1,[r0] / stmia r3!,{r0,r1,r2} / sub r3,#0xc`
 * is that inline verbatim, with size 80 (cnt 0x85000014).
 *
 * WHAT THE FUNCTION IS
 *   No arguments. It clears 80 bytes at iwram_3001e70[0x13] + 0x11c, then walks a byte stream
 *   at (*(void **)iwram_3001e70)[4] in (x, y, id) triples, terminated by an x/y pair of
 *   0xff/0xff. For each triple whose id is in [0x64, 0x64+0x8b], it calls the overlay hook at
 *   __start_overlay[9] to get a 0xc-byte entry table and scans it (terminated by w == -1) for
 *   an entry whose (short)e->f4 matches the id. On a match, `w & 0x1ff` selects one of two
 *   placement bodies -- 0x13 creates actor kind 0x14, 3 creates kind 0x1c under two extra
 *   gates -- and each writes one 8-byte descriptor at dst. After ten descriptors (n > 9) the
 *   whole walk stops.
 *
 * LEVERS THAT PAID
 *   BOTH LOOPS ARE `goto` LOOPS. Same reason as this batch's LoadMapActors: a real loop gets
 *     its exit test copied to the entry by jump.c:1137 duplicate_loop_exit_test, and the inner
 *     loop would get strength reduction on `e++`. The ROM rematerialises `-1` inside the inner
 *     loop every iteration (`mov r4,#1 / neg r4,r4`), which is direct evidence that nothing is
 *     hoisted there and so that it is not a real C loop.
 *   `x = *q++; y = *q++;` RATHER THAN `x = q[0]; y = q[1]; q += 2;`  -- 270 -> 273 at the right
 *     end (positional differences 262 -> 254). The align count rose 192 -> 194 while the length
 *     got closer, which is the batch-292 "take the length" case.
 *   `u16 f4` WITH `(short)e->f4` AT THE COMPARISON. The ROM reads that field BOTH ways:
 *     `ldrsh r3,[r6,r4]` for the id comparison and `ldrh r3,[r6,#4]` for the byte store into
 *     dst[4]. An s16 field gives ldrsh at both.
 *
 * MEASURED INERT (each a single drop)
 *   `dst` declared before `q`                                        194
 *   `k` named before px/pz                                           194
 *   `k + px` instead of `px + k` at all four sites                   194
 *   the two sums hoisted into named locals `ax`/`az`                 194
 * MEASURED WORSE
 *   `dst` ASSIGNED before `q`                                        199
 *   `volatile int id` (tried only to force the ROM's frame slot)      272 at length 302
 *
 * THE 194, AND THE 16 MISSING INSTRUCTIONS
 *
 * 1. CROSS-JUMP: THIS IS THE WHOLE LENGTH SHORTFALL, AND IT IS DOWNSTREAM OF ALLOCATION.
 *    The two placement bodies end with the same source -- `dst[6] = a->x / 0x100000;
 *    dst[7] = a->z / 0x100000;` -- and jump2's find_cross_jump merges about twenty
 *    instructions of ours into one copy. The ROM merges only from its `.L8eba2` label, i.e.
 *    the last two instructions of the dst[7] bias plus the n++/dst += 8 tail. The reason the
 *    ROM stops there is visible in the ROM itself: case 0x13 writes dst[6] through
 *    `mov r1,r8 / strb r3,[r1,#6]` and case 3 through `mov r2,r8 / strb r3,[r2,#6]` -- DIFFERENT
 *    REGISTERS, so the blocks are not identical and cannot be merged. find_cross_jump runs in
 *    jump2, after reload, so this is a CONSEQUENCE of getting the allocation right and not an
 *    independent lever. Do NOT reach for a barrier here: the ROM genuinely merges at .L8eba2,
 *    so a barrier that suppresses the merge entirely would overshoot. I deliberately kept the
 *    two bodies in the ROM's opposite statement orders (0x13 writes a[0x23] then a[0x59] and
 *    dst[4] then *dst; case 3 the reverse of both) and that is not enough on its own.
 *
 * 2. THE FOUR HIGH REGISTERS ARE SPENT ON DIFFERENT THINGS, AND `id` GOES TO THE FRAME IN THE
 *    ROM. The ROM's frame is 12 bytes: sp+0 = n, sp+4 = id, sp+8 = DMA3_CLEAR's zero. Ours is
 *    8 -- we keep `id` in r11 and never spill it. The ROM's high registers are
 *    r8 = dst, r9 = 0x80000, r10 = q, r11 = x << 20; ours are r8 = q, r10 = dst, r11 = id, and
 *    r9 = x << 20, with 0x80000 rematerialised. So there are two couplings here, not one:
 *    `id` has to leave, and the constant 0x80000 has to arrive.
 *    This is the SECOND function in this brief where the ROM spills the short-lived inner index
 *    and we keep it in r11 (LoadMapActors spills `idx` the same way). Two independent instances
 *    of one pattern is worth reading `.17.lreg`/`.18.greg` for, rather than nudging source --
 *    six source nudges across the two functions were all inert.
 *
 * 3. 0x80000 IS REMATERIALISED AT BOTH CALL SITES (`mov r1,#0x80 / lsl r1,#0xc` twice, 4
 *    instructions) WHERE THE ROM HOLDS IT IN r9 AND COPIES IT OUT (`mov r4,r9 / add r1,r9`).
 *    The mechanism is the goto loop biting back: with no NOTE_INSN_LOOP_BEG there is no LICM to
 *    hoist the constant, so it has to survive as a source variable across the loop head -- and
 *    the inner loop head has two predecessors, so cse cannot propagate INTO it, which is
 *    exactly why the ROM keeps it. Ours is propagated anyway, which points at gcse's cprop
 *    (which is dominator-based, not EBB-based) rather than cse. Four spellings of the sum were
 *    inert; the next thing to try is whether -fno-gcse alone recovers r9, purely as a
 *    diagnostic to confirm the pass.
 *
 * 4. THE `/ 0x10000` AND `/ 0x100000` STORES SCHEDULE THEIR ADDRESS TOO EARLY (4 sites, ~10
 *    rows). The ROM computes the bias-corrected value, THEN `mov r3,r5 / add r3,#0x64`, then
 *    `asr / strh`. We compute the address before the bias branch. Note the ROM then derives the
 *    NEXT offset from it by move2add: case 0x13 does 0x66 -> `sub r2,#0x43` for a+0x23 ->
 *    `add r2,#0x36` for a+0x59, and case 3 does 0x66 -> `sub r2,#0xd` -> `sub r2,#0x36`. So
 *    these two clusters are one: get the address into the ROM's register and the offset chain
 *    follows. A value-into-a-temp spelling was measured on LoadMapActors' identical shape and
 *    is much worse there (117 -> 144), so do not start with that.
 */
#include "gba/types.h"
#include "gba/io.h"
#include "dma.h"

struct Ent {
    int w;        /* 0x00 */
    u16 f4;       /* 0x04 */
    s16 f6;       /* 0x06 */
    int f8;       /* 0x08 */
};

extern void *iwram_3001e70[];
extern void *__start_overlay[];

extern unsigned char *_CreateActor(int kind, int x, int y, int z);
extern void Func_808e9a8(void);
extern void _Actor_SetSpriteFlags(unsigned char *a, int f);
extern int _GetFlag(int flag);
extern void _DeleteActor(unsigned char *a);
extern void _Actor_SetAnim(unsigned char *a, int n);
extern void _Actor_Stop(unsigned char *a);

void Func_808e9c0(void);

void Func_808e9c0(void)
{
    unsigned char *q;
    unsigned char *dst;
    struct Ent *e;
    unsigned char *a;
    struct Ent *(*fn)(void);
    int n;
    int x;
    int y;
    int id;
    int w;
    int t;
    int px;
    int pz;
    int k;

    n = 0;
    q = (unsigned char *)((void **)iwram_3001e70[0])[4];
    dst = (unsigned char *)iwram_3001e70[0x13] + (0x8e << 1);
    DMA3_CLEAR(dst, 80);
    if (q == 0)
        goto done;
    x = *q++;
    y = *q++;
    if (x == 0xff && y == 0xff)
        goto done;
entry:
    id = *q++;
    if ((unsigned)(id - 0x64) > 0x8b)
        goto step;
    fn = (struct Ent *(*)(void))__start_overlay[9];
    e = fn();
    w = e->w;
    if (w == -1)
        goto step;
    px = x << 20;
    pz = y << 20;
    k = 0x80 << 12;
inner:
    if ((short)e->f4 != id)
        goto innext;
    t = w & 0x1ff;
    if (t == 0x13) {
        a = _CreateActor(0x14, px + k, 0, pz + k);
        if (a == 0)
            goto innext;
        Func_808e9a8();
        _Actor_SetSpriteFlags(a, 0);
        if (_GetFlag((short)e->f6) != 0) {
            if ((e->f8 & 0xfff00000) == 0xa0 << 15) {
                _DeleteActor(a);
                goto innext;
            }
            _Actor_SetAnim(a, 2);
        }
        _Actor_Stop(a);
        *(s16 *)(a + 0x64) = *(int *)(a + 8) / 0x10000;
        *(s16 *)(a + 0x66) = *(int *)(a + 0x10) / 0x10000;
        a[0x23] = 1;
        a[0x59] = 1;
        dst[4] = e->f4;
        *(unsigned char **)dst = a;
        dst[6] = *(int *)(a + 8) / 0x100000;
        dst[7] = *(int *)(a + 0x10) / 0x100000;
    } else {
        if (t != 3)
            goto innext;
        if ((e->f8 & 0xfff00000) != 0xc0 << 14)
            goto innext;
        if (_GetFlag((short)e->f6) != 0)
            goto innext;
        a = _CreateActor(0x1c, px + k, 0, pz + k);
        if (a == 0)
            goto innext;
        Func_808e9a8();
        _Actor_SetSpriteFlags(a, 0);
        _Actor_Stop(a);
        _Actor_SetAnim(a, 1);
        *(s16 *)(a + 0x64) = *(int *)(a + 8) / 0x10000;
        *(s16 *)(a + 0x66) = *(int *)(a + 0x10) / 0x10000;
        a[0x59] = 1;
        a[0x23] = 1;
        *(unsigned char **)dst = a;
        dst[4] = e->f4;
        dst[6] = *(int *)(a + 8) / 0x100000;
        dst[7] = *(int *)(a + 0x10) / 0x100000;
    }
    n++;
    dst += 8;
    if (n > 9)
        goto done;
    goto step;
innext:
    e++;
    w = e->w;
    if (w == -1)
        goto step;
    goto inner;
step:
    x = *q++;
    y = *q++;
    if (x == 0xff && y == 0xff)
        goto done;
    goto entry;
done:
    ;
}
