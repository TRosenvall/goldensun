/* Func_8079008 -- NOT MATCHING. 11 of 38.
 *
 * NON-MATCHING, 29 of 39 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 39, ours 37) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 84 bytes, ours 80.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/rom_79008.c \
 *     asm/rom_77000/rom_79008_a.s --func Func_8079008
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * ITS PREVIOUS PROSE CLAIM OF 11 DID NOT REPRODUCE -- measured 29.
 *
 * Source asm: goldensun/asm/rom_77000/rom_79008_a.s
 *
 * Blocker: register allocation around a multiply chain, plus one folded
 * constant.
 *
 *     rom    mov r2, r3 / lsl r3, r2, #1 / add r3, r2
 *     ours   lsl r3, r0, #1 / add r3, r0
 *
 * The ROM COPIES the index before shifting because it lives in the register the
 * shift overwrites; gcc keeps it where no copy is needed. One instruction, and
 * it is allocation rather than expression shape.
 *
 * WHAT IS ALREADY RIGHT:
 *
 *   `t = k * 99;` is the correct spelling even though the ROM emits the chain
 *   `k*2 + k`, then `t + (t << 5)`. Per batch 56's test, writing the literal and
 *   seeing whether gcc produces the chain is what decides it -- gcc produces
 *   exactly that chain, so it is gcc's.
 *
 *   the range checks are a mix: `idx <= 0` and `idx > 0x63` are SIGNED (`ble`,
 *   `bgt`) while `k > 7` is UNSIGNED (`bhi`), so the two are different types.
 *
 * TRIED: the byte offset as its own local so the `- 4` stays a `sub`
 * instruction. Without it gcc folds the four into the symbol as `=L2-4`; with
 * it the `sub` appears but the count is unchanged at 11, because the remaining
 * differences are elsewhere.
 *
 * NEXT: nothing at the expression level.
 */
extern void *GetUnit(void);
extern unsigned char L7a830[] __asm__(".L7a830");

int Func_8079008(int unused, int idx)
{
    unsigned char *u;
    unsigned int k;
    int t;
    unsigned int off;

    u = (unsigned char *)GetUnit();
    if (u[0x129] == 0)
        goto neg;
    if (idx <= 0)
        return 0;
    if (idx > 0x63)
        goto neg;
    k = u[0x94 << 1];
    if (k > 7)
        goto neg;
    t = k * 99;
    t += idx;
    off = t << 2;
    off -= 4;
    return *(int *)(L7a830 + off);
neg:
    return -1;
}
