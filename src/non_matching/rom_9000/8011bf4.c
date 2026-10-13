/* Func_8011bf4 (PaletteCycleTask) -- NON-MATCHING, 78 encodings of 115 differ.
 *
 * ours 113 encodings / 109 16-bit instructions against ref 115 / 111, pool 3/3,
 * 232 bytes against 236, so the positional 78 is mostly SHIFT.  Honest figures:
 * aligncmp 27 of 115 (91 aligned-equal, 79.1%); tryc --align 23 of 120.
 * Whole of asm/rom_9000/rom_11568_c_c_c_c_c.s.  0 pins, 0 devices (shimcount).
 *
 * SPLIT SHAPE: NONE NEEDED.  datacheck.py exits silent (no data section, no
 * EXPORTS line) and split_s.py --dry-run says "holds only Func_8011bf4 and no
 * data; convert it directly, no split needed".
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_9000/8011bf4.c \
 *     asm/rom_9000/rom_11568_c_c_c_c_c.s --func Func_8011bf4
 * Rank variants on:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/aligncmp.py src/non_matching/rom_9000/8011bf4.c \
 *     asm/rom_9000/rom_11568_c_c_c_c_c.s Func_8011bf4
 *
 * ================= BATCH 348 (brief D): THE GAP IS TWO SITES OF 1 =============
 * Measured per region, not inferred.  Pre-loop 0, loop-1 and its preheader 0,
 * and then:
 *
 *   A. loop-2 preheader.  ROM `lsl r1, r7, #0x10 / mov r7, r1` (2); ours
 *      `lsl r7, #0x10` (1).                                            -1
 *   B. post-DMA phase join.  ROM recomputes `t >> 16` AFTER the join:
 *          add r1, r7, r2      ; t = (phase<<16) + 0x10000
 *          lsr r3, r1, #0x10   ; t >> 16, for the compare
 *          cmp r3, r4 / bcc .L11ca6
 *          mov r1, #0          ; t = 0, IN THE SHIFTED FORM
 *        .L11ca6:
 *          lsr r3, r1, #0x10   ; t >> 16 AGAIN, for the store
 *          strh r3, [r5, #4]
 *      ours commons the two `t >> 16` on the fall-through edge and
 *      constant-folds `0 >> 16` on the other, so `t` dies at the shift.   -1
 *
 * ---- SITE B IS CLOSED BY AN EXPLICIT if/else PHI (measured) ----
 *   {u32 t = (uph + 1) << 16; u32 v;
 *    if ((t >> 16) >= ulen) v = 0; else v = t;
 *    c->phase = v >> 16;}
 * takes insns 109 -> 111 (= ref), pool 3 (= ref), size 232 -> 236 (= ref) and
 * the relocation offsets become EXACT, so the figure becomes a TRUE DISTANCE:
 * objcmp 78 -> 54.  It is NOT shipped because the ALIGNED residue rises,
 * aligncmp 27 -> 34 and tryc --align 23 -> 33: the phi spends TWO instructions
 * at the join where the ROM spends one, and site A is still missing, so the two
 * wrong regions cancel in the total.  Both numbers are given on purpose.  A
 * next attempt that fixes site A should re-try the phi immediately.
 *
 * `u32 v = t; if (...) v = 0;` is NOT a phi -- copy propagation coalesces it and
 * BOTH spellings of that are bit-identical to this body.
 *
 * ---- SITE A: A REACHABILITY BOUND, WITH ITS EVIDENCE ----
 * The window `lsl rA, rB, #16` immediately followed by `mov rB, rA` occurs
 * 0 times in the 4,578 GENERATED (byte-matching) .s files in asm/ and once in
 * the 673 hand-written ones -- this function.  Widened to any ALU op followed by
 * a mov back to its source register: 15 generated hits.  Two read for their C:
 *   - asm/rom_8a000/rom_9b698_a_a_c.s:104 `asr r2, r3, #16 / mov r3, r2` is a
 *     CONDITIONAL-ASSIGNMENT PHI COPY -- src/rom_8a000/rom_9b698_a_a_c.c's
 *     `diff = (short)(a - cur); ad = diff < 0 ? -diff : diff;`
 *   - asm/rom_77000/rom_78414_c_c_a_c_a_a_c.s:72 `lsl r3, r6, #1 / mov r6, r3 /
 *     add r6, r6, #216` is a TWO-ADDRESS TIE -- src/rom_77000/...c's
 *     `u->items[i]`, where `i*2` becomes the tied operand of the next `add`.
 * Neither mechanism is available at our site: straight-line preheader code whose
 * only consumers are three-operand `lsr`/`add`.  A third mechanism may exist --
 * this is a bound with evidence, not a closed door.
 *
 * ---- MEASURED INERT, BIT-IDENTICAL TO THIS BODY ----
 *  - `(u16)phase` written inline at BOTH use sites in place of the named `uph`,
 *    across all 8 crossings with the tail spellings.  So the second `phase<<16`
 *    occurrence that would give gcse something to common is created by COMBINE,
 *    which runs after gcse, and never exists at gcse time.
 *  - `&c->colors[0]`, `(u16 *)((unsigned char *)c + 0xc)` for src.
 *  - `(s16)(t >> 16)` on the store; `!((t >> 16) < ulen)` for the test;
 *    `(uph << 16) + (1 << 16)` for t.
 * ---- MEASURED WORSE (insns / pool / size / aligncmp) ----
 *    u16 np = uph + 1; if (np >= ulen) np = 0;     110 / 3 / 232 / 39
 *    u16 q = t >> 16 named for the test            110 / 3 / 232 / 39
 *    ternary ((t>>16) >= ulen ? 0 : t) >> 16       110 / 4 / 236 / 46
 *    phi with the test inverted                    110 / 3 / 232 / 37
 *    u16 *b = buf  (on phi / on this body)         110 / 108, aligncmp 51 / 49
 *    src declared before dst                       109 / 3 / 232 / 36
 *
 * ---- FLAG PROBE, INSTRUMENT ONLY (no per-file flag ships) ----
 *    -fno-cse-follow-jumps            BIT-IDENTICAL
 *    -fno-cse-skip-blocks             BIT-IDENTICAL
 *    both together                    BIT-IDENTICAL
 *    -fno-gcse                        114 insns, 240 bytes, aligncmp 43
 *    -fno-rerun-cse-after-loop        115 insns, 244 bytes, aligncmp 54
 *    both of those                    130 insns, 272 bytes, aligncmp 88
 * So the join fold at site B is NEITHER cse-follow-jumps NOR cse-skip-blocks,
 * including the pair docs/elevation.md records as "decisive together" elsewhere;
 * gcse and the post-loop cse rerun are load-bearing in the OTHER direction.
 *
 * ================= WHAT WAS ALREADY SOLVED (keep all of it) ==================
 *  - loop bound `i < (n & 3)` with n the raw halfword: loop.c hoists the AND into
 *    a new pseudo in the pre-header, which is the ROM's `mov r9, r2` AFTER the
 *    entry test;
 *  - `void *dst = c->dst;` declared/assigned before `src` (dst -> r8, i -> r14);
 *  - phase/len read as s16 locals, then u16 copies (ulen, uph) -- the ROM's
 *    r6 = len<<16 with a separate lsr at every use comes out of that;
 *  - the phase update through a u32 <<16 temporary (t) instead of a u16 np.
 * Remaining beyond the two sites above:
 *  - src takes r1 and the per-loop `mov rX, sp` buffer bases r4; ROM is the other
 *    way round.  src (20 refs/32 insns) outranks the hoisted sp copies.
 *  - `n & 3`: ROM puts the 3 in the AND's destination (mov r2,#3; and r2,r3);
 *    `3 & n`, `(u8)(n & 3)`, `3u` inert, `(u16)` worse.
 */
#include "gba/types.h"
#include "dma.h"

extern unsigned char *iwram_3001ec0;

typedef struct {
    void *dst;
    s16 phase;
    u16 counter;
    u16 delay;
    s16 len;
    u16 colors[16];
} PalCycle;

void Func_8011bf4(void)
{
    unsigned char *st = iwram_3001ec0;
    u8 i;
    u16 buf[16];
    u32 n = *(u16 *)(st + 0xb0);

    for (i = 0; i < (n & 3); i++) {
        PalCycle *c = (PalCycle *)(st + i * 0x2c);
        if (c->counter == 0) {
            s16 phase = c->phase;
            s16 len = c->len;
            u8 j = len - phase;
            u16 ulen = len;
            u16 uph;
            void *dst = c->dst;
            u16 *src = c->colors;
            u16 np;
            for (; j < ulen; j++)
                buf[j] = *src++;
            uph = phase;
            for (j = 0; j < ulen - uph; j++)
                buf[j] = *src++;
            DMA3_SET(buf, dst, 0x80000000 | ulen);
            {u32 t = (uph + 1) << 16;
            if ((t >> 16) >= ulen)
                t = 0;
            c->phase = t >> 16;}
            c->counter = c->delay;
        } else {
            c->counter--;
        }
    }
}
