/* OvlFunc_932_20086a0  --  NOT MATCHING
 *
 * NON-MATCHING, 10 of 27 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 27, ours 25) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   RELOCATIONS differ, but THE SAME SYMBOLS AT A SHIFTED OFFSET -- which
 *   this project treats as a CONSEQUENCE of the length difference, not a
 *   separate blocker.  Re-classified in batch 322; the figure IS a distance.
 *   SIZE ref 60 bytes, ours 56.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7b9cb4/20086a0.c \
 *     asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c_a_a.s --func OvlFunc_932_20086a0
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_c_c_c_c_c.s
 * Best screen: 3 instructions in disagreeing regions, of 26 (rom 26, ours 24).
 *
 * BLOCKER CLASS: mask narrowing on a halfword store.
 *
 * The ROM narrows the value explicitly before storing it, even though `strh`
 * truncates on its own:
 *
 *      rom   lsl r3, r5, #0x10 / lsr r3, #0x10 / strh r3, [r6, #0x0]
 *      ours  strh r5, [r6, #0x0]
 *
 * gcc knows the store discards the top bits and drops the pair.  Every attempt
 * to keep them was folded away:
 *
 *  1. `m = (unsigned short)s; *p = m;` through an int temp.  Byte-identical.
 *  2. Declaring `s` as `unsigned short` so the OR itself narrows.  Also
 *     byte-identical -- gcc sinks the narrowing to the store and then removes
 *     it there.
 *
 * Same class as the other mask-narrowing parks: the redundancy is provable, so
 * the optimiser removes it no matter where the source puts it.
 *
 * TWO SPELLINGS THAT DID WORK and are kept below:
 *   - `x = 0x64; x *= r;` makes the CONSTANT the destination of the multiply,
 *     matching `mov r3, #0x64 / mul r3, r0`.  See docs/elevation.md; this is
 *     the same lever as the AND/ORR case and it applies to MUL too.
 *   - `t = 0xfdff; t &= v;` likewise.
 * The local data label is reached the established way,
 * `extern unsigned char L5238[] __asm__(".L5238");`.
 */
extern unsigned char L5238[] __asm__(".L5238");
extern int __Random(void);

void OvlFunc_932_20086a0(void)
{
    unsigned short *p;
    int v;
    int t;
    int s;
    int r;
    unsigned int x;
    int lim;
    int m;

    p = (unsigned short *)(0x80 << 19);
    v = *p;
    t = 0xfdff;
    t &= v;
    s = (short)(t << 16 >> 16);
    r = __Random();
    x = 0x64;
    x *= r;
    lim = *(unsigned short *)L5238;
    x >>= 16;
    if (x >= (unsigned int)lim) {
        m = 0x80 << 2;
        s |= m;
    }
    *p = (unsigned short)s;
}
