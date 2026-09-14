// fakematch
/* GetFlag @ 0x08079338, SetFlag @ 0x08079358, ClearFlag @ 0x08079374
 * (the whole of asm/rom_77000/rom_79338_a.s -- all three functions)
 *
 * The save-flag bit accessors.  Byte is (id & 0xfff) >> 3, bit is id & 7.
 * GetFlag has 180 call sites, ClearFlag 58, SetFlag 50.
 *
 * FAKEMATCH: five register pins and four empty `__asm__ volatile` barriers.
 * Both are load-bearing and both are explained below; every one was checked by
 * removal (dropping any single pin costs 6 or 7 differing).
 *
 * THE BLOCKER THE WHOLE FAMILY WAS PARKED ON, and its real mechanism.
 * The ROM's index extract is THREE-operand -- `lsl r3, r0, #0x14` then
 * `lsr r0, r3, #0x17` -- and every pure-C spelling gives the destructive
 * `lsl r0, #0x14 / lsr r0, #0x17`.  That is not a spelling question at all:
 *
 *   gcc-2.96 local-alloc.c, block_alloc() -> combine_regs().  `*thumb_ashlsi3`
 *   has two alternatives ("=l,l" / "l,0" / "N,l"), so `must_match_0` stays -1
 *   and `n_matching_alts` (1) != `n_alternatives` (2) -- operand 1 is NOT
 *   skipped, so the shift's input is a tie candidate.  It carries a REG_DEAD
 *   note (the parameter's last use), so combine_regs TIES the shift result into
 *   the parameter's quantity, which holds `qty_phys_copy_sugg = r0` from the
 *   entry copy `(set (reg 32) (reg:SI 0 r0))`.  Result: r0, destructively.
 *   The SAME tie then fires on `*thumb_lshrsi3`, pulling the byte index into
 *   the same quantity.  Two ties, one quantity, one register.
 *
 *   The ROM has the two values in DIFFERENT registers (r3 and r0), so BOTH
 *   ties failed in the original build.  Nothing at the C level reaches either:
 *   the tie is refused only when the source has no REG_DEAD note on that insn,
 *   or when `reg_qty` is -1 (REG_N_DEATHS != 1 / not block-local), and in a
 *   straight-line accessor neither is reachable.  Confirmed inert: every index
 *   spelling ((idx & 0xfff) >> 3, (idx >> 3) & 0x1ff, (idx & 4088) >> 3,
 *   unsigned parameter, unsigned copy, %/ arithmetic, named temp, one C
 *   variable reused for bitpos and temp), plus -fno-regmove, -fno-gcse,
 *   -fno-cse-follow-jumps, -fno-strength-reduce, -fno-expensive-optimizations,
 *   -fno-rerun-cse-after-loop.  All byte-identical to each other.
 *
 * SO THE TWO TIES ARE BROKEN INDIVIDUALLY:
 *   - the FIRST tie by an empty `__asm__ volatile` that reads `idx` AFTER the
 *     `lsl`, so `idx` no longer dies there and combine_regs finds no REG_DEAD;
 *     the shift temp then takes r3 by REG_ALLOC_ORDER (arm.h:989, {3,2,1,0}).
 *   - the SECOND tie by making the byte index a HARD register (`__asm__("r0")`).
 *     combine_regs returns 0 whenever SREG is a hard reg, so the temp cannot
 *     pull the index into r3 with it.
 *
 * THE OTHER HALF, and it had to be fixed first: the AND's operand order.
 * Writing `gFlags[i] & bit` puts the LOAD first in the RTL and gcc hoists the
 * whole address computation ahead of the mask (11 of 13 differing).  Naming the
 * loaded byte and writing `b & bit` -- value first, mask second -- restores the
 * ROM's order and takes GetFlag to 3 of 13 before any pin.  See the AND
 * destinations: ROM `and r3, r2` accumulates into the loaded byte.
 *
 * SetFlag and ClearFlag differ only in where `ldr r1, =gFlags` lands, and that
 * is a sched2 placement the barriers pin exactly: a volatile asm is a
 * scheduling barrier, so naming `p = gFlags` and mentioning it in a barrier
 * fixes the pool load to one side of it.  SetFlag's ROM has the load BEFORE
 * `lsl r2, r3`, so the mask is split into `bit = 1;` and `bit <<= bp;` with the
 * barrier between them; ClearFlag's has it after, so no split is needed.
 * `p` is pinned to r1 because without it the pointer and the mask exchange r1
 * and r2 (REG_ALLOC_ORDER again).
 *
 * The branchless tail `neg r0, r3 / orr r0, r3 / lsr r0, #0x1f` is gcc's `!= 0`
 * idiom and it must be written out longhand: `!= 0`, `!!v` and a named local
 * all give a branch and 16 lines.  (Recorded already in the old park note.)
 */
extern unsigned char gFlags[];

int GetFlag(int idx)
{
    register unsigned int i __asm__("r0");
    unsigned int t;
    int bit;
    int b;
    int v;

    bit = 1 << (idx & 7);
    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    b = gFlags[i];
    v = b & bit;
    return (unsigned int)(-v | v) >> 31;
}

void SetFlag(int idx)
{
    register unsigned int i __asm__("r0");
    register unsigned char *p __asm__("r1");
    unsigned int t;
    int bp;
    int bit;

    bp = idx & 7;
    bit = 1;
    p = gFlags;
    __asm__ volatile ("" : : "r" (p));
    bit <<= bp;
    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx));
    i = t >> 23;
    p[i] = bit | p[i];
}

void ClearFlag(int idx)
{
    register unsigned int i __asm__("r0");
    register unsigned char *p __asm__("r1");
    unsigned int t;
    int bit;

    bit = 1 << (idx & 7);
    p = gFlags;
    t = (unsigned int)idx << 20;
    __asm__ volatile ("" : : "r" (idx), "r" (p));
    i = t >> 23;
    p[i] = p[i] & ~bit;
}
