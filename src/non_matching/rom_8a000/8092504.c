/* Func_8092504 -- asm/rom_8a000/rom_91584_c_c_a_c_c_c_c_c_a_c_c.s
 *
 * BLOCKER: an r6/r7 swap. 7 of 34, LENGTH EXACT.
 *
 * Polls a byte on a field actor for up to 0x5a frames, returning as soon as it
 * changes from the value read at entry.
 *
 * NEW LEVER, worth 25 differences and the whole five-line shortfall:
 * A `volatile` LOCAL FOR A VALUE THE ROM KEEPS IN A STACK SLOT.
 *
 *     rom    sub sp, #4 / mov r7, sp / str r3, [r7]   ... ldr r3, [r7] in the
 *                                                          loop, every pass
 *     ours   the value held in a register across the call
 *
 * The ROM allocates a four-byte frame for ONE value, writes the entry byte
 * into it, and RE-READS IT ON EVERY ITERATION rather than keeping it live in a
 * callee-saved register -- and r5, r6 and r7 are all pushed, so it is not
 * short of registers. A local that is memory-resident and re-read at each use
 * is what `volatile` produces, and declaring it so took the function from
 * 33 differing at 29 lines to 8 at exactly 34.
 *
 * STATED AS AN INFERENCE, not a certainty: a plain local that gcc happened to
 * SPILL would give the same shape, and the two cannot be told apart from the
 * output alone. What the ROM shows is a memory-resident, re-read local; that
 * `volatile` is the construct which produces it is the simplest explanation,
 * not a proven one. The remaining 7 mean it is not confirmed either way.
 *
 * The tell to reuse: `sub sp, #N` for a frame that holds ONE value, written
 * once and read inside a loop, on a function that is not short of registers.
 *
 * ALSO FIXED: the comparison operand order. The ROM has `cmp r3, r2` --
 * saved value first, current byte second -- so the source reads
 * `if (init != *q)`, not `if (*q != init)`.                     8 -> 7
 *
 * WHAT REMAINS: the ROM puts the polled pointer in r6 and the frame pointer in
 * r7; ours has them the other way round, and that accounts for all seven.
 * Three declaration orders -- the volatile before the pointer, after the
 * counter, and first of all -- are BYTE-IDENTICAL, so the source cannot
 * express it.
 */
extern char *GetFieldActor(int slot);
extern void WaitFrames(int n);

void Func_8092504(int slot)
{
    char *a;
    unsigned char *q;
    volatile int init;
    int i;

    a = GetFieldActor(slot);
    if (a == 0)
        return;
    if (*(unsigned char *)(a + 0x54) != 1)
        return;
    q = (unsigned char *)(*(int *)(a + 0x50) + 0x24);
    init = *q;
    for (i = 0; i <= 0x59; i++) {
        WaitFrames(1);
        if (init != *q)
            break;
    }
}
