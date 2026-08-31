/* GetFlagByte -- 0x080793b8, SetFlagByte -- 0x080793c8, IncFlagByte -- 0x080793d8,
 * DecFlagByte -- 0x080793f8, GetFlagNybble -- 0x08079418
 * (asm/rom_77000/rom_79338_c_a.s -- all five functions in the TU)
 *
 * BLOCKER: same as the sibling TU, src/non_matching/rom_77000/rom_79338_a.c --
 * the gFlags address is materialised before the index shifts, occupying the
 * register the ROM uses as the shift temp.  Read that file first; it carries
 * the full root-cause analysis.  This file records what the five byte and
 * nibble accessors add to it.
 *
 *   GetFlagByte     3 of 5 differ    exact length
 *   SetFlagByte     3 of 5 differ    exact length
 *   GetFlagNybble   9 of 11 differ   exact length
 *   IncFlagByte    11 of 14 differ   13 lines, one short
 *   DecFlagByte    11 of 14 differ   13 lines, one short
 *
 * GetFlagByte is the cleanest statement of the whole family's problem, at five
 * instructions with three differing and nothing else going on:
 *
 *     rom   lsl r3, r0, #0x14 / lsr r0, r3, #0x17 / ldr r3, =gFlags
 *     ours  ldr r3, =gFlags   / lsl r0, #0x14     / lsr r0, #0x17
 *
 * Identical instructions, three of them in the wrong order, and the shift form
 * degrades from three-operand to destructive as a consequence of the address
 * load having taken r3 first.  There is no arithmetic left to get wrong.
 *
 * THE SHIFT FORM IS NOT SEPARATELY REACHABLE.  Three spellings of the index,
 * screened in isolation on GetFlagByte, are BYTE-IDENTICAL to each other:
 *
 *     unsigned int parameter, (idx & 0xfff) >> 3              3 differ
 *     named unsigned temp, t = idx << 20; i = t >> 23         3 differ
 *     fully inline, (unsigned)(idx << 20) >> 23               3 differ
 *
 * gcc lowers all three to the same pair of shifts and then allocates registers
 * without reference to how the source was written.  Combined with the
 * named-pointer result in the sibling file, both ends of this problem -- the
 * ordering and the register choice -- are closed to source-level control.
 *
 * IncFlagByte and DecFlagByte are one instruction SHORT, and the missing
 * instruction is real: the ROM copies the loaded byte with `mov r3, r2` before
 * testing it, keeping the original in r2 and the incremented value in r3.  Ours
 * tests and increments the same register.  That copy is the signature of two
 * distinct source variables where we have one, but every two-variable spelling
 * tried either reproduced the copy and lost the ordering, or kept the ordering
 * and dropped the copy.  Since the surrounding ordering is already wrong for
 * the reason above, this was not pursued further -- it should be revisited only
 * after the address-hoist question is settled, because the register pressure
 * that decides the copy depends on it.
 *
 * The saturating guards themselves reproduce exactly and are worth recording as
 * correct: `cmp r3, #0xfe / bhi` is `if (v <= 0xfe)` on a value loaded by ldrb,
 * and `add r3, #0xff` is a subtract of one, not an add.
 */
extern unsigned char gFlags[];

int GetFlagByte(int idx)
{
    int i;

    i = (idx & 0xfff) >> 3;
    return gFlags[i];
}

void SetFlagByte(int idx, int v)
{
    int i;

    i = (idx & 0xfff) >> 3;
    gFlags[i] = v;
}

int IncFlagByte(int idx)
{
    int i;
    int v;

    i = (idx & 0xfff) >> 3;
    v = gFlags[i];
    if (v <= 0xfe)
        gFlags[i] = v + 1;
    return gFlags[i];
}

int DecFlagByte(int idx)
{
    int i;
    int v;

    i = (idx & 0xfff) >> 3;
    v = gFlags[i];
    if (v != 0)
        gFlags[i] = v - 1;
    return gFlags[i];
}

int GetFlagNybble(int idx)
{
    int i;
    int sh;

    i = (idx & 0xfff) >> 3;
    sh = idx & 4;
    return (gFlags[i] & (0xf << sh)) >> sh;
}
