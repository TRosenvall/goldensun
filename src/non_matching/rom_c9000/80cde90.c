/* DrawLine -- asm/rom_c9000/rom_cd508_c_a_c.s, 0x080cde90, 202 ROM lines.
 *
 * NON-MATCHING: 109 encodings of 205 differ (objcmp).
 *
 * 109 IS NOT A DISTANCE.  objcmp reports `SIZE ref 420 bytes, ours 416` and
 * `ref 205, ours 203`, so the streams are misaligned from the first shortfall
 * and every later encoding counts as differing.  THE REAL MEASURE IS THE
 * ALIGNED INSTRUCTION DIFF: 202 normalised ROM instructions against 201 of
 * ours, 20-25 in FIVE disagreeing regions, and the x-major loop body -- the
 * larger of the two -- matches ENTIRELY.  The RE is complete: this is a
 * Bresenham line into a 128-wide 8x8-tile framebuffer, and the arithmetic,
 * both major-axis branches, the coordinate swap, both __divsi3 calls and the
 * one stack spill around the second call all reproduce.  RELOCATIONS: same set,
 * same order; the FIRST __divsi3 sits at the ROM's exact offset 0x7e, so the
 * whole function up to the y-major divide is laid out byte-for-byte, and the
 * later three sites are 2 then 4 bytes early -- the shortfalls below.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_c9000/cde90_DrawLine.c \
 *     asm/rom_c9000/rom_cd508_c_a_c.s --func DrawLine
 *
 * THE FIVE REGIONS, exactly.
 *
 *  1. LENGTH, -1 instruction, in the x-major coordinate swap:
 *        rom   sub r2,r6,r5 / mov r1,r8 / mov r10,r2 / sub r7,r4,r1 / mov r1,r10
 *        ours  sub r1,r6,r5 / mov r2,r8 / mov r10,r1 / sub r7,r4,r2
 *     `n` and `dy` are separate pseudos; the ROM computes the difference into a
 *     scratch, copies it to dy's hi register r10, and then copies r10 BACK into
 *     n.  gcc coalesces instead: it computes straight into n's register and
 *     copies that to r10.  PASS: local_alloc / regmove coalescing.  Three
 *     statement orders are inert (`n = dy` before `dx`, the two branches'
 *     expressions exchanged, `n = y1 - y0` in both).  The remaining 2 bytes of
 *     the size gap are a literal-pool artefact, not code: the ROM reaches
 *     0xfffffeff through `ldr =0xfffffeff` TWICE with no `.pool` directive
 *     inside DrawLine, so the reference's pool words are not laid out the way
 *     gcc lays out its own `.L37`.
 *
 *  2. THE Y-MAJOR LOOP'S TWO COORDINATES ARE IN SWAPPED REGISTERS (7 places).
 *     ROM puts the SCANNED coordinate in r0 and the DITHERED one in r1 in BOTH
 *     branches -- `mov r0,r5 / mov r1,r8` in y-major and `mov r0,r8 / mov r1,r5`
 *     in x-major.  Because the two pairs exchange roles between the branches,
 *     the ROM's four coordinate values CANNOT be two pseudos with fixed
 *     registers.  Ours gets x-major right and y-major backwards.
 *     PASS: global_alloc priority.  Counting refs, the DITHERED coordinate has
 *     MORE (`add rN,#1` and `sub rN,#1` in two arms, plus the shift and the
 *     mask = 7) than the SCANNED one (shift, mask, increment, compare = 6), so
 *     it should rank higher and take r1 with the scanned one falling to r0 --
 *     which is the ROM.  Ours ranks them the other way and nothing source-level
 *     moved it: SEPARATE variable pairs per branch (A2), assignment order
 *     swapped, `u` assigned inside the guard, declaration order, `int` with
 *     explicit `(unsigned)` casts on the shifts, a `while` guard instead of
 *     `if` + do-while, and the loop written as a `goto` loop (that one is
 *     actively worse: 400 bytes, 196 instructions) are ALL INERT AT 109.
 *
 *  3.,4.,5. Three single-slot sched2 permutations, no length change:
 *     `mov r10,r1` one slot early in the prologue; `mov r1,r8` two slots early
 *     in the y-major swap block; `mov r0,r5` / `mov r1,r8` exchanged at the
 *     y-major loop guard (that one is region 2's consequence).
 *
 * ============================================================
 * WHAT WAS READ OUT OF THE ROM AND IS LOAD-BEARING.
 *
 *  - THE SCAN COORDINATES ARE `unsigned`.  The tile address uses `lsr rN,#3`
 *    with NO `cmp/add #7` correction, which is an UNSIGNED shift; the SAME
 *    function's neighbour AnimTransitionOut uses `asr` WITH the correction at
 *    the same position, i.e. a SIGNED `/ 8`.  Two functions in one bank, one
 *    tile-address formula, two different carrier types -- read the correction
 *    sequence, never assume.  `int` coordinates here cost the whole address
 *    block.
 *  - `if (fb[o] < col)` is `ldrb / ldr r5,[sp,#0x24] / cmp / bge`: the byte is
 *    zero-extended and the comparison is SIGNED against the fifth (stacked)
 *    argument, so `col` is `int` and the array `unsigned char`.
 *  - THE TWO ABSOLUTE VALUES ARE WRITTEN OUT, NOT CALLED: `adx = dx;
 *    if (dx < 0) adx = -dx;` gives the ROM's `mov r2,r7 / cmp r7,#0 / bge /
 *    neg r2,r7` exactly.
 *  - THE NUMERATOR IS BUILT UNCONDITIONALLY AND THEN OVERWRITTEN:
 *    `num = dx << 8; if (dx < 0) num = (x0 - x1) << 8;` -- the ROM emits the
 *    first `lsl` before the test, so this is two statements, not an if/else.
 *  - THE DIVISOR IS AN if/else IN THE X-MAJOR BRANCH AND A CONDITIONAL
 *    OVERWRITE IN THE Y-MAJOR ONE.  x-major has the `str r4,[sp] / bl __divsi3
 *    / ldr r4,[sp]` spill duplicated in BOTH arms, which is an if/else over the
 *    whole call; y-major has one call reached from both paths.
 *  - THE ACCUMULATOR MASK PAIR IS `acc & 0x100` and `acc &= ~0x100`, with
 *    0xfffffeff pooled -- exactly what `~0x100` compiles to.
 *  - `iwram_3001ef0` is the framebuffer and is read WHOLE (`ldr r3,=sym /
 *    ldr r3,[r3]`), so `extern unsigned char *iwram_3001ef0;` with a plain
 *    read; no offset and no `.sym` entry.
 *
 * rom_cd508_c.s has NO data section (tools/datacheck.py prints nothing), so the
 * split for this function is text-only.
 */
extern unsigned char *iwram_3001ef0;

void DrawLine(int x0, int y0, int x1, int y1, int col)
{
    unsigned char *fb;
    int dx, dy, adx, ady, step, acc, num, n, t;
    unsigned int u, v;
    int o;

    dy = y1 - y0;
    dx = x1 - x0;
    acc = 0x80;
    fb = iwram_3001ef0;
    if (y0 < 0)
        y0 = 0;
    if (y0 > 0x7f)
        y0 = 0x7f;
    if (y1 < 0)
        y1 = 0;
    if (y1 > 0x7f)
        y1 = 0x7f;
    adx = dx;
    if (dx < 0)
        adx = -dx;
    ady = dy;
    if (dy < 0)
        ady = -dy;
    if (adx < ady) {
        if (dy < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dx = x1 - x0;
        }
        n = y1 - y0;
        num = dx << 8;
        if (dx < 0)
            num = (x0 - x1) << 8;
        if (n < 0)
            n = y0 - y1;
        step = num / n;
        v = y0;
        u = x0;
        if (v != y1) {
            do {
                o = (((v >> 3) * 16 + (u >> 3)) * 8 + (v & 7)) * 8 + (u & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dx > 0)
                        u++;
                    else
                        u--;
                    acc &= ~0x100;
                }
                v++;
            } while (v != y1);
        }
    } else {
        if (dx < 0) {
            t = x0;
            x0 = x1;
            x1 = t;
            t = y0;
            y0 = y1;
            y1 = t;
            dy = y1 - y0;
            dx = x1 - x0;
            n = dy;
        } else {
            n = y1 - y0;
        }
        num = n << 8;
        if (n < 0)
            num = (y0 - y1) << 8;
        if (dx >= 0)
            step = num / dx;
        else
            step = num / (x0 - x1);
        u = x0;
        v = y0;
        if (u != x1) {
            do {
                o = (((v >> 3) * 16 + (u >> 3)) * 8 + (v & 7)) * 8 + (u & 7);
                if (fb[o] < col)
                    fb[o] = col;
                acc += step;
                if (acc & 0x100) {
                    if (dy > 0)
                        v++;
                    else
                        v--;
                    acc &= ~0x100;
                }
                u++;
            } while (u != x1);
        }
    }
}
