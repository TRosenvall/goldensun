/* OvlFunc_955_2008970  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_c_c_c_c_c_c_c_c.s
 * Best screen: 4 instructions in disagreeing regions, of 28 (rom 28, ours 27).
 *
 * BLOCKER CLASS: cross-jumping -- ONE shared instruction.
 *
 * A poll loop that jumps into the middle of itself.  The ROM dereferences the
 * global on BOTH paths into the test:
 *
 *      ldr r3, =L4834 / ldr r3, [r3]    <- entry
 *      mov r5, #0 / b L1
 *   L2: ... / ldr r3, =L4834 / ldr r3, [r3]
 *   L1: cmp r3, #0
 *
 * gcc emits the address load on both paths but SHARES the dereference, placing
 * the label one instruction earlier:
 *
 *      ldr r3, =L4834 / mov r5, #0 / b L1
 *   L2: ... / ldr r3, =L4834
 *   L1: ldr r3, [r3] / cmp r3, #0
 *
 * Every instruction is otherwise identical; the whole diff is that one-slot
 * label shift and the pool-symbol renumbering it drags behind it.
 *
 * WHAT WAS TRIED
 *
 *  1. The un-rotated `goto` form below, which reproduces the ROM's
 *     jump-into-the-middle exactly.  4 of 28, the best.
 *  2. A plain `while (*a != 0 || *b != 0x4b) { ... }`.  MUCH worse, 23 of 28 --
 *     gcc rotates the loop and the whole shape changes.
 *
 * The two tails gcc merges are genuinely identical instructions on genuinely
 * identical values, so no source spelling distinguishes them.  Same class as
 * the pre-header load merge recorded in earlier batches, at its smallest
 * possible size: one instruction.
 */
extern unsigned char L4834[] __asm__(".L4834");
extern unsigned char L4838[] __asm__(".L4838");
extern void __WaitFrames(int n);

void OvlFunc_955_2008970(void)
{
    int v;
    int i;
    int lim;

    __WaitFrames(0xa);
    v = *(int *)L4834;
    i = 0;
    goto test;
loop:
    __WaitFrames(1);
    i++;
    lim = 0x96 << 2;
    if (i >= lim)
        goto done;
    v = *(int *)L4834;
test:
    if (v != 0)
        goto loop;
    if (*(int *)L4838 != 0x4b)
        goto loop;
done:
    ;
}
