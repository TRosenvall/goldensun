/* Func_8010788 (CopyMapRectIndices) @ 0x08010788 -- NON-MATCHING.
 *
 * NON-MATCHING: 118 encodings of 150 differ (objcmp).
 * Reference 150 encodings / 316 bytes / 4 relocations; ours 150 encodings, the
 * SAME 316 bytes and the SAME 4 relocation SYMBOLS -- only the pool ORDER
 * differs, because two of the four constants are hoisted (see below).
 * First divergence at index 7: `sub sp, #0x24` (ROM) vs `sub sp, #0x2c` (ours).
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py <this> \
 *     asm/rom_9000/rom_10424_c_a.s --whole
 *
 * rom_10424_c_a.s HOLDS ONLY THIS FUNCTION AND NO DATA SECTION (datacheck.py),
 * so landing it is a pure whole-file text conversion with no split.
 *
 * THE READING IS RIGHT.  Every loop bound, both `& 0xf` wraps, the `<< 3` table
 * stride, the `(0x80 - w) * 4` row advance, the three-window visibility test with
 * its 0x10 x 0xc extents, the `dst = (dst & 0xfffff000) | (src & 0xfff)` merge,
 * the signed loop comparisons, the `iwram_3001e70 + (0x82 << 1)` window fetch
 * with its `asr #20` pair and its stride 0x30, and the `check_dbra_loop`
 * count-down of the three-iteration fill loop all appear on both sides, in order.
 * OURS IS THE ROM'S LENGTH AND THE ROM'S BYTE COUNT.
 *
 * BLOCKER: A TWO-STAGE LOOP-INVARIANT HOIST INFLATES THE FRAME BY TWO SLOTS.
 *
 * `loop_optimize` scans loops INNERMOST FIRST (loop.c, `for (i = max_loop_num - 1;
 * i >= 0; i--)`).  The four pool constants used by the refresh -- 0x6002800,
 * ewram_2020000, 0x6002840, ewram_2020004 -- are set and used inside the
 * three-iteration LAYER loop, so stage one moves them to the layer loop's
 * pre-header, i.e. into the x-loop body.  Their x-loop LIFETIME now spans the
 * whole layer loop, and stage two (`move_movables`, whose test is
 * `threshold * savings * m->lifetime >= insn_count` with `threshold -= 3` after
 * each move) then moves TWO of them out of the x loop as well:
 *
 *     ours   r9  = 0x6002800      r11 = ewram_2020004   r12 = (y & 0xf) << 5
 *     rom    r9  = &win           r11 = w               r12 = (y & 0xf)
 *
 * That is the whole residue.  Two extra live values take the two registers the
 * ROM gives to `w` and `&win`, which spill to [sp,#0x10] and [sp,#0x0] and turn
 * the ROM's `sub sp, #0x24` into `sub sp, #0x2c`.  The ROM hoists NOTHING out of
 * the layer loop -- `lsl r3, r5, #3` and all four `ldr rX, =...` are inside the
 * conditional body -- and only `0xf` and `y & 0xf` out of the x loop.  0xfff and
 * 0xfffff000, whose set and use are adjacent in the x body, are NOT hoisted on
 * either side, which is the lifetime asymmetry that identifies the mechanism.
 *
 * NOT a register permutation and not a declaration-order problem: our frame is
 * TWO SLOTS LARGER, so there is no allocno contest to price.
 *
 * MEASURED, all against the 118 (ours/ref encodings in brackets):
 *   this file                                                118 [150/150]
 *   `(((wy + (k << 4)) << 5) + (x & 0xf)) * 4`                118 [150/150]
 *   `i` as an explicit accumulator (`i += 0x800`)             125 [150/150]
 *   `... << 2) + (k << 11)` (giv added last)                  124 [151/150]
 *   `(... + (k << 9)) << 2`                                   126 [150/150]
 *   `wy = y & 0xf` named in the y body                        118  INERT
 *   `t << 3` named once and used twice                        118  INERT
 *   0x6002840 written as `0x6002800 + i + 0x40`               118  INERT (folds)
 *   four `continue`-style guards instead of `&&`              118  INERT
 *   one `||` guard with `continue`                            118  INERT
 *   named `xlim`/`ylim`                                       120 [148/150]
 *   `int` array externs indexed `[t * 2]`                     138 [142/150]
 *   named pointer for the two STORE addresses only            118  INERT
 *   named pointer for the two LOAD addresses only             140 [144/150]
 *   named pointers for all four                               140 [144/150]
 *   layer loop as a `goto` loop                              142 [144/150]  frame 0x28
 *   layer goto loop + named store pointers                   142 [144/150]  frame 0x28
 *   ALL THREE loops as `goto` loops                          143 [136/150]  frame 0x1c
 *
 * THE `goto` LAYER LOOP IS THE ONE THING THAT MOVES THE FRAME (0x2c -> 0x28),
 * which CONFIRMS the two-stage diagnosis: killing stage one recovers one slot.
 * It is not the answer, because it also kills the strength reduction that builds
 * the ROM's `lsl r4, r3, #2` giv and costs six instructions.  The all-goto shape
 * is 14 instructions SHORT, so the ROM's loops really are gcc loops.
 *
 * FLAG PROBES (diagnostic only, not shipping levers):
 *   -fno-strength-reduce   113 differing at the ROM's 150 -- frame STILL 0x2c,
 *                          so strength reduction is not the pressure source
 *   -fno-gcse              140 [152]
 *   -fno-rerun-loop-opt    136 [158]
 *   -fno-rerun-cse-after-loop 125 [153]
 *
 * NEXT: make gcc keep two FEWER values live across the x loop, not permute the
 * ones it keeps.  The question to answer is why stage one fires here and not in
 * the ROM: `reg_in_basic_block_p` is TRUE for all four constants (set and last
 * use in one block), so `maybe_never` does not block them, and the threshold
 * arithmetic says a 15-insn loop always loses its invariants.  The same wall
 * blocks the two file-mates of this family, CopyMapTiles and Func_80105d4, and
 * the unconditional-body cousin src/non_matching/overlays/2008098.c.
 */
extern unsigned int gBuffer[];
extern unsigned char *iwram_3001e70;
extern unsigned char ewram_2020000[];
extern unsigned char ewram_2020004[];

void Func_8010788(int sx, int sy, int w, int h, int dx, int dy)
{
    unsigned int *src;
    unsigned int *dst;
    unsigned char *p;
    int *q;
    int win[6];
    int i, x, y, k;
    unsigned int t;

    src = gBuffer + ((sy << 7) + sx);
    dst = gBuffer + ((dy << 7) + dx);
    p = iwram_3001e70 + (0x82 << 1);
    q = win;
    for (i = 0; i < 3; i++) {
        q[0] = *(int *)p >> 20;
        q[1] = *(int *)(p + 4) >> 20;
        p += 0x30;
        q += 2;
    }
    for (y = dy; y < dy + h; y++) {
        for (x = dx; x < dx + w; x++) {
            t = *src++ & 0xfff;
            *dst = (*dst & 0xfffff000) | t;
            dst++;
            q = win;
            for (k = 0; k < 3; k++) {
                i = ((((y & 0xf) + (k << 4)) << 5) + (x & 0xf)) << 2;
                if (q[0] <= x && q[0] + 0x10 > x && q[1] <= y && q[1] + 0xc > y) {
                    *(int *)(0x6002800 + i) = *(int *)(ewram_2020000 + (t << 3));
                    *(int *)(0x6002840 + i) = *(int *)(ewram_2020004 + (t << 3));
                    break;
                }
                q += 2;
            }
        }
        src += 0x80 - w;
        dst += 0x80 - w;
    }
}
