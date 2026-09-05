/* Func_80b9554 -- 0x080b9554  [asm/rom_b5000/rom_b8228_c_a_c_c_a_c_a.s]
 *
 * PARKED DELIBERATELY, NOT FOR LACK OF A MATCH. The body below is BYTE-EXACT --
 * tools/objcmp.py reports 176 bytes, 81 encodings and 7 relocations identical.
 * It is parked because it is a knowingly-wrong transcription of the source, and
 * the right one is now known.
 *
 * THIS IS A GCC NESTED FUNCTION, and that is the finding. It is the
 * static-chain class docs/elevation.md already describes: r9 is read with no
 * defining write, saved and restored, and one stack slot is never read back.
 * The caller Func_80b9724 sits in the SAME .s and does
 * `add r2, sp, #0x14 / mov r9, r2 / bl Func_80b9554` at three sites, so
 * `fp[-1]`, `fp[-2]` and `fp[-3]` are that caller's own locals.
 *
 * PROVED, NOT INFERRED. Writing this body as a genuine nested function inside a
 * stand-in parent makes gcc-2.96 emit it BYTE-IDENTICAL to the ROM -- prologue,
 * the `mov r7, r2` copy, the `mov r2, #0x1` and the pool word included.
 *
 * WHY THE BODY BELOW IS NOT THE ANSWER. To reproduce a static chain from a
 * NON-nested function it has to: read a hard register that nothing writes;
 * launder a pointer through `__asm__ __volatile__` so flow.c cannot see the
 * frame slot as dead; and rely on that asm's placement to order two moves. None
 * of that means anything to a reader, none of it is checkable, and any edit
 * near it breaks the result silently. It is the same trade refused in batch 219
 * for the label-shifting `do { } while (0)`: byte-correct and wrong.
 *
 * THE ACTUAL FIX, and it is a whole-file job rather than a function one:
 * elevate Func_80b9724 and write this function and its sibling Func_80b9604
 * NESTED INSIDE IT. The r9 binding, the slot and both asm statements all
 * disappear. Func_80b9604 is the same class -- its chain reaches fp[-1],
 * fp[-3], fp[-4] and fp[-5] -- so the two should land together with the parent.
 *
 * WHAT THE TRANSCRIPTION TAUGHT ANYWAY, worth keeping because it is about the
 * class rather than the hack:
 *
 *  - THE CHAIN SLOT'S VALUE CANNOT BE A NAMED C VARIABLE. Eight spellings that
 *    gave it a name all scored 7 differing with r2 and r3 swapped throughout,
 *    including the second `-1`. A named value gets a pseudo, and that pseudo
 *    takes r2 away from the ADDRESS. gcc's chain slot is
 *    `(mem (plus virtual_stack_vars -4))`, a NEGATIVE frame offset that fails
 *    `memory_address_p`, so the address is forced into a pseudo BEFORE the
 *    hi-register source is forced into a low one -- which is why the address is
 *    born first and takes r2. A plain `chain = c` giving `str rX, [sp]` can
 *    never reach it.
 *  - PINNING THE FRAME POINTER TO r7 MISCOMPILES. Because r9 is saved it is in
 *    the allocation pool, so gcc assigns another local the same register and
 *    clobbers it. 22 differing, and wrong code.
 *  - Block layout follows source order here too: testing the mask one way round
 *    costs five lines over the other.
 *
 * Func_80063bc needs an `int` return in this file (it is compared against -1),
 * where src/rom_c0/rom_5cf8_c.c declares it void; the `pop {r1} / bx r1`
 * epilogue confirms the live return value.
 */
extern unsigned short iwram_3001f64;
extern int Func_80063bc(int a, int b);
extern unsigned int Func_80064f4(void);
extern int WaitFrames(int frames);

int Func_80b9554(void)
{
    unsigned int chain;
    unsigned int *slot;
    register unsigned int _chain __asm__("r9");
    int *fp;
    int t;
    int n;

    slot = &chain;
    __asm__ __volatile__ ("" : "=l" (slot) : "0" (slot));
    *slot = _chain;
    fp = (int *)*slot;
    if (Func_80063bc(fp[-1], 0x14) == -1)
        return -1;
    n = 0;
    t = 300;
    while (Func_80064f4() != 0) {
        WaitFrames(1);
        if (--t < 0)
            return -1;
        if ((iwram_3001f64 & 3) != 3) {
            if (++n > 0x18)
                return -1;
        } else {
            n = 0;
        }
    }
    if (fp[-2] != 0) {
        if (Func_80063bc(fp[-3], fp[-2]) == -1)
            return -1;
        while (Func_80064f4() != 0) {
            WaitFrames(1);
            if (--t < 0)
                return -1;
            if ((iwram_3001f64 & 3) != 3) {
                if (++n > 0x18)
                    return -1;
            } else {
                n = 0;
            }
        }
    }
    return 0;
}
