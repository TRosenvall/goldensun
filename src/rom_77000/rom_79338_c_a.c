// fakematch
/* GetFlagByte @ 0x080793b8, SetFlagByte @ 0x080793c8, IncFlagByte @ 0x080793d8,
 * DecFlagByte @ 0x080793f8, GetFlagNybble @ 0x08079418
 * (the whole of asm/rom_77000/rom_79338_c_a.s -- all five functions)
 *
 * The save-byte and save-nybble accessors.  Same family as GetFlag/SetFlag/
 * ClearFlag in rom_79338_a.c, and parked on the same blocker: the ROM's index
 * extract is THREE-operand (`lsl r3, r0, #0x14` / `lsr r0, r3, #0x17`) where
 * every pure-C spelling gives the destructive two-operand form.
 *
 * THE MECHANISM IS RECORDED IN FULL IN src/rom_77000/rom_79338_a.c -- read it
 * there rather than here.  In short: local-alloc's combine_regs ties the shift
 * result into the parameter's r0-suggested quantity, twice, and the two ties
 * must be broken separately.  The idiom that does it is transferred verbatim:
 *
 *     t = (unsigned int)idx << 20;
 *     __asm__ volatile ("" : : "r" (idx));   <- kills the first tie's REG_DEAD
 *     i = t >> 23;                           <- `i` pinned, kills the second
 *
 * FAKEMATCH: six register pins and five empty barriers, all from that idiom.
 *
 * WHAT WAS NEW HERE, and both results are about NOT reaching for a variable.
 *
 * 1. THE ROM'S `mov r3, r2` IS A CSE ARTIFACT, NOT A SOURCE VARIABLE.
 *    IncFlagByte's ROM loads into r2, copies to r3, compares the copy, then
 *    adds from r2.  That reads like two named locals.  It is not: writing
 *    `v = p[i]; w = v; ...` has the copy coalesced away (7 differing), and so
 *    does every other two-name spelling.  Reading `p[i]` THREE TIMES and naming
 *    nothing is exact -- cse1 makes the one load and leaves the copy:
 *
 *        if (p[i] <= 0xfe)
 *            p[i] = p[i] + 1;
 *        return p[i];
 *
 *    Measured: no locals EXACT; `unsigned char v` 7; `unsigned int v, w` 7;
 *    `v` plus a named result `w` 13 and two instructions long; early-return
 *    form 10; assigning the result back through `i` 7.  DecFlagByte is the same
 *    shape and closed on the same spelling with no further work.
 *
 *    The comparison must also be UNSIGNED -- the ROM's `bhi` against a signed
 *    `bgt` is one of the seven.
 *
 * 2. A PIN CAN BE THE WRONG TOOL EVEN WHERE A PIN IS ALREADY WORKING.
 *    GetFlagNybble needed the gFlags pointer pinned to r3 (8 differing -> 2;
 *    unpinned, gcc hoists the pool load ahead of the index and puts it in r2).
 *    The last two were the first two instructions swapped -- the cheap
 *    `mov r1, #4` hoisted above `lsl r3, r0, #20`.  Pinning the shift variable
 *    to r1 is 10, WORSE than doing nothing; moving the existing barrier to sit
 *    BETWEEN the shift and the mask constant is exact.
 *
 *        register ... *p __asm__("r3") + barrier after the shift   EXACT
 *        ... + shift variable pinned to r1                         10
 *
 *    So within one function one pin was load-bearing and a second pin was
 *    harmful, and the lever for the second defect was barrier PLACEMENT.  The
 *    recorded "try the bare pin first" rule is about reaching for a pin before
 *    a barrier; it does not mean a second pin is the next thing to try once one
 *    pin is already in place.
 */
extern unsigned char gFlags[];

int GetFlagByte(int idx)
{
    register unsigned int i __asm__("r0");
    unsigned int t;

    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    return gFlags[i];
}

void SetFlagByte(int idx, int val)
{
    register unsigned int i __asm__("r0");
    unsigned int t;

    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    gFlags[i] = val;
}

int IncFlagByte(int idx)
{
    register unsigned int i __asm__("r0");
    register unsigned char *p __asm__("r1");
    unsigned int t;

    t = (unsigned int)idx << 20;
    p = gFlags;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    if (p[i] <= 0xfe)
        p[i] = p[i] + 1;
    return p[i];
}

int DecFlagByte(int idx)
{
    register unsigned int i __asm__("r0");
    register unsigned char *p __asm__("r1");
    unsigned int t;

    t = (unsigned int)idx << 20;
    p = gFlags;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    if (p[i] != 0)
        p[i] = p[i] - 1;
    return p[i];
}

int GetFlagNybble(int idx)
{
    register unsigned int i __asm__("r0");
    register unsigned char *p __asm__("r3");
    unsigned int t;
    int sh, m, b;

    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx));
    sh = idx & 4;
    i = t >> 23;
    p = gFlags;
    m = 0xf;
    b = p[i];
    m <<= sh;
    b &= m;
    return b >> sh;
}
