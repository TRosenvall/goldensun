/* Func_801ff58 -- "SubmitCursorSprite", from asm/rom_15000/rom_1fe2c_c_a.s.
 *
 * rom_15000 is NOT an overlay; nothing here was transplanted from the overlay
 * cutscene-script work that the interleave class has mostly been mined from.
 *
 * REQUIRES -fno-rerun-loop-opt.  See "the flag" below -- it is the whole cost
 * of this function and it is read out of loop.c, not guessed.
 *
 * THE INTERLEAVED SITE CAME FREE.  The function carries the "zero interleaved
 * into a shifted build" shape at one straight-line site:
 *
 *     mov r3, #0x80 / mov r2, r4 / mov r1, r6 / lsl r3, #7 / str r4, [sp]
 *     bl _UpdateSprite
 *
 * -- two single-instruction arguments landing INSIDE the split build of the
 * fourth.  Per the re-derived reading of the wall, a BARE call was tried first
 * and reproduced the site exactly on screen 1, with no pins, no flags and no
 * barriers.  Every later candidate kept it.  Nothing in this file is a pin, and
 * none of the three recorded pin jobs applies: there is one site, the shifted
 * constant is not repeated, and no value is passed twice.
 *
 * TWO REAL DEFECTS, both structural, both found by reading the frame.
 *
 * 1. `str r4, [sp, #0]` before the call and `ldr r4, [sp, #0]` after it is
 *    CALLER-SAVE, not a fifth argument.  Read as an argument it costs 4 bytes
 *    of frame: a 5-argument call reserves an outgoing-arg word at [sp, #0] AND
 *    gcc then allocates a separate save slot at [sp, #4], giving `sub sp, #0x20`
 *    against the ROM's `sub sp, #0x1c` and a doubled `str r4`.  With four
 *    arguments the outgoing area vanishes, the save slot lands at [sp, #0], and
 *    the frame and both r4 accesses match.  GCC296_CFLAGS carries
 *    -fcall-used-r4, which is why a pointer in r4 needs saving across a call at
 *    all -- the tell is a str/ldr PAIR on one register bracketing a `bl`, with
 *    no other use of that stack word.
 *
 * 2. The two ROM pointers must be written as SUBSCRIPTS OF THE COUNTER, not as
 *    walking pointers.  `slot[i]`, `slot[i + 0x10]`, `pos[i]`, `pos[i + 8]`
 *    make gcc's own strength reduction produce the ROM's `add r7, #2` /
 *    `add r5, #4`.  Writing the walks by hand (`pos++; slot++;`) instead is 37
 *    differing: it hands gcc a counter with no givs and it reverses the loop in
 *    the FIRST loop pass, and it also permutes r5/r6.  This is the same
 *    "naming a value gcc already CARRIES destroys the carry" trap one level up
 *    -- here what gcc carries is an induction variable, not a constant.
 *
 * THE FLAG, and why no C spelling replaces it.  gcc-2.96's loop optimiser runs
 * TWICE at -O2 (-frerun-loop-opt).  Pass 1 reduces the four subscript givs into
 * independent bivs; pass 2 therefore sees the counter `i` with giv_count == 0
 * and computes `no_use_except_counting = 1`.  In check_dbra_loop (loop.c:7748)
 * that single flag is load-bearing twice over:
 *
 *     if ((num_nonfixed_reads <= 1 && ! loop_info->has_call && ...)
 *         || no_use_except_counting)
 *       ...
 *       if (comparison && (GET_CODE (comparison) == LT
 *                          || (GET_CODE (comparison) == LE
 *                              && no_use_except_counting)))
 *
 * This loop CONTAINS A CALL, so the first disjunct is dead and reversal rests
 * entirely on no_use_except_counting -- which is computed at all only under
 * `bl->giv_count == 0 && ! loop->exit_count`.  Pass 1 satisfies neither (the
 * counter still has givs, and num_mem_sets is 6, so the else-branch returns 0);
 * pass 2 satisfies both.  Deleting pass 2 is exact.  Confirmed from the other
 * side: an `unsigned` counter compares LTU/LEU, matches neither arm of that
 * gate, is never reversed, and lands 1 differing -- `bls` where the ROM has
 * `ble`.  That is the whole reachable range from C: signed reverses, unsigned
 * does not reverse but cannot spell `ble`.  To defeat it in source you would
 * need a non-counting use of `i` surviving pass 2, or a second loop exit, and
 * the ROM's 62 instructions contain neither.
 *
 * `-fno-strength-reduce` is NOT a substitute (it is the existing Makefile group
 * and was tried first): it kills check_dbra_loop, but it also kills the giv
 * reduction this function depends on -- 60 differing on the subscript form, and
 * 23 on the walking-pointer form.  The two passes are separable only by
 * -fno-rerun-loop-opt.
 *
 * MEASURED (tryc instruction stream, rom = 62 lines):
 *
 *   bare, 5-arg call, `for` <= 3, subscripts       64 lines, 43 differing
 *   4-arg, walking pointers, do/while <= 3         63, 37
 *   4-arg, subscripts, `for` i <= 3 / i < 4        63, 26
 *   4-arg, subscripts, do/while i <= 3             63, 26
 *   4-arg, subscripts, while (1) + break           63, 26
 *   4-arg, subscripts, backward `goto` loop        61, 57   (kills the givs)
 *   4-arg, subscripts, do/while i != 4             62,  2   (cmp #4/bne)
 *   4-arg, subscripts, `unsigned` counter          62,  1   (bls)
 *   walking pointers  + -fno-strength-reduce       62, 23
 *   subscripts        + -fno-strength-reduce       64, 60
 *   walking pointers  + -fno-rerun-loop-opt        63, 37
 *   THIS FILE         + -fno-rerun-loop-opt        62,  0   exact
 *
 * The loop was left as a plain `for`; do/while with `i <= 3` is byte-identical
 * under the flag, so the `for` is kept as the more ordinary spelling.
 */
extern unsigned char *iwram_3001f2c;
extern void _UpdateSprite(void *obj, int *xf, int *src, int mode);

void Func_801ff58(void)
{
    unsigned char *base;
    short *pos;
    int *slot;
    int i;
    void *obj;
    int src[2];
    int xf[4];

    base = iwram_3001f2c;
    pos = (short *)(base + 0x134);
    slot = (int *)(base + 0x114);
    for (i = 0; i < 4; i++) {
        obj = (void *)slot[i];
        if (obj != 0) {
            src[0] = slot[i + 0x10];
            src[1] = slot[i + 0x10];
            xf[0] = pos[i] << 16;
            xf[1] = 0xfa << 17;
            xf[2] = (pos[i + 8] << 16) + (0xfa << 17);
            xf[3] = 0;
            _UpdateSprite(obj, xf, src, 0x80 << 7);
        }
    }
}
