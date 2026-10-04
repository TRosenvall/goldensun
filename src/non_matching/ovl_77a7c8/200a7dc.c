/* OvlFunc_881_200a7dc -- NON-MATCHING.
 *
 * NON-MATCHING, 30 of 31 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 31, ours 29) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 64 bytes, ours 60.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_77a7c8/200a7dc.c \
 *     asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_c_a.s --func OvlFunc_881_200a7dc
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: DUPLICATED BASE POINTER, the elided-copy shape.
 * 31 lines against the ROM's 33, TWO SHORT, 32 differing.
 *
 * A scan over 12-byte records for one whose first word is 2 and whose halfword
 * at +4 is 0x8a, stopping at a first word of -1. The ROM carries the table
 * base in TWO registers -- `ldr r0, =gOvl_0200e3f4 / mov r7, r0` -- reading
 * through `[r2, r0]` and writing through `[r2, r7]`. gcc sees one value, keeps
 * one register, and the function comes out two instructions shorter.
 *
 * This is the same shape as Func_80bf54c and the other elided-copy parks: the
 * ROM copies a value before use and gcc loads straight into the destination.
 * Writing two locals both assigned the base does not produce two registers,
 * because they are provably equal.
 *
 * Worth contrasting with Func_80a1bdc, elevated this batch, where adding a
 * SECOND POINTER did work. There the two pointers advance differently -- one
 * post-increments for the read, the other lags for the call argument -- so
 * they hold different values and gcc must keep both. Here they never differ.
 * The lever is "give the source the values the ROM carries" only when those
 * values are actually distinct.
 */
extern char gOvl_0200e3f4[];

void OvlFunc_881_200a7dc(void)
{
    char *base;
    char *w;
    char *p;
    int off;
    int k, one, neg1;

    base = gOvl_0200e3f4;
    k = 0x21;
    neg1 = 1;
    w = base;
    one = 1;
    p = base + 4;
    off = 0;
    neg1 = -neg1;
    for (;;) {
        if (*(int *)(base + off) == 2 && *(short *)p == 0x8a) {
            *(int *)(w + off) = one;
            *(int *)(p + 4) = k;
            return;
        }
        if (*(int *)(base + off) == neg1)
            return;
        p += 0xc;
        off += 0xc;
    }
}
