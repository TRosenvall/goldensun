/* Func_8078550  --  NOT MATCHING
 *
 * NON-MATCHING, 20 of 26 encodings  (MEASURED, batch 319 recipe backfill).
 *   COUNT DIFFERS (ref 26, ours 24) -- so this positional figure measures
 *   MISALIGNMENT, not distance.  Read the count before the figure.
 *   *** RELOCATIONS ALSO DIFFER -- this figure is NOT a distance. ***
 *   SIZE ref 56 bytes, ours 52.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/rom_77000/8078550.c \
 *     asm/rom_77000/rom_78414_c_c_a_a_c_a.s --func Func_8078550
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 *
 * Source asm: goldensun/asm/rom_77000/rom_78414_c_c_a_a.s
 * Best screen: 8 instructions in disagreeing regions, of 27 (rom 27, ours 25).
 *
 * BLOCKER CLASS: register allocation, on a stack buffer.
 *
 * Two differences, and the second follows from the first.
 *
 *  - The ROM COPIES the buffer pointer before the loop, `mov r6, r5`, keeping
 *    the original in r5 and walking r6, then reuses r5 for the count.  gcc
 *    walks r5 and counts in r6, so no copy is needed.
 *  - The ROM computes the running total with a three-operand subtract into a
 *    fresh register and then assigns, `sub r0, r7, r0 / mov r7, r0`, where gcc
 *    uses the destructive `sub r7, r0`.
 *
 * WHAT WAS TRIED
 *
 *  1. An explicit temporary for the subtract, `t = acc - r; acc = t;` -- the
 *     spelling that produces exactly that shape elsewhere.  BYTE-IDENTICAL,
 *     still 8 of 27.  gcc collapses the temp because nothing else reads it.
 *  2. A separate walking pointer initialised from the array while the array
 *     itself is passed to the call, so that two distinct pointers exist.
 *     WORSE, 10 of 27.
 *
 * Attempt (2) is the lever that DID work for Func_80a9cbc this batch -- a
 * derived initialiser forces the copy gcc otherwise coalesces away.  It fails
 * here because the second pointer is not derived: it is the same address, and
 * gcc has no reason to keep two names for one value.  The lever needs the
 * initialiser to be an EXPRESSION, not an alias.  See
 * src/rom_a1000/rom_a8604_c_c_a_c_a_b.c.
 */
extern int Func_80796c4(void *buf);
extern int FindEmptyInventorySlot(int item);

int Func_8078550(void)
{
    short buf[10];
    short *p;
    int n;
    int cnt;
    int acc;
    unsigned int o;
    int h;
    int r;

    p = buf;
    n = Func_80796c4(p);
    acc = 0;
    p = buf;
    if (acc >= n)
        goto done;
    cnt = n;
    do {
        o = 0;
        h = *(short *)((unsigned char *)p + o);
        r = FindEmptyInventorySlot(h);
        acc = acc - r;
        cnt--;
        p++;
        acc = acc + 0xf;
    } while (cnt != 0);
done:
    return acc;
}
