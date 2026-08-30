/* OvlFunc_common2_41c -- asm/overlays/common/common2_c_c_c_c_c_c_c_a.s
 *
 * BLOCKER CLASS: callee-saved register-allocation coin flip.
 *
 * REWRITTEN batch 151. The previous note here said "the C shape is probably
 * wrong, not the flags", at 22 of 28 differing against a 27-line ROM. Both
 * halves of that have now been settled, and in opposite directions.
 *
 * THE FLAGS WERE WRONG, and are now fixed tree-wide. common2 was built without
 * -fcall-used-r4; COMMON2_CFLAGS substitutes -fcall-saved-r4 as of this batch,
 * verified a no-op for all nine already-matched common2_c*.c. The old note
 * recorded that passing the flag by hand "changed nothing", which was true and
 * is no longer interesting: with the C shape a line long the flag had nothing
 * to bite on, exactly as that note predicted.
 *
 * THE C SHAPE IS NOW RIGHT. 27 lines against the ROM's 27, same block order,
 * same branch senses, same instruction shapes, one for one. What it took:
 *
 *   - a union of `unsigned long long` and two u32 halves, with the result
 *     written back through the union AFTER the join, not inside either arm.
 *     Building the result as `((u64)rhi << 32) | rlo` instead costs five
 *     instructions (two mov #0 and two orr) where the ROM moves the pair;
 *     assigning the union inside both arms lets gcc thread each arm straight
 *     to the epilogue and drops the join entirely (24 lines, 22 differing).
 *   - a goto putting the count>=32 arm first, since the ROM branches `bgt`
 *     to the count<32 arm and falls through to the other.
 *
 * WHAT IS LEFT is 18 differing lines and every one is a register rename:
 *
 *     rom    count in r6, result pair in r4/r5, push {r4, r5, r6, lr}
 *     ours   count in r4, result pair in r2/r3, push {r4, r5, lr}
 *
 * The function is a leaf, so gcc has no reason to prefer callee-saved
 * registers and picks the low scratch ones; the ROM spends three pushes to use
 * r4/r5/r6 anyway. That is the same preference-pass difference documented for
 * the four coin-flip parks in docs/elevation.md, and this one is a cleaner
 * specimen than any of them because nothing else differs.
 *
 * NOT re-derivable from source: tried the structured (non-goto) join, direct
 * union assignment in both arms, separate (lo, hi, count) parameters instead
 * of a u64 first argument (40 lines, 39 differing -- clearly wrong), and
 * `return v >> shift` (gcc emits a call to __lshrdi3).
 */
union U {
    unsigned long long q;
    struct { unsigned int lo; unsigned int hi; } h;
};

unsigned long long OvlFunc_common2_41c(unsigned long long v, int shift)
{
    union U u;
    unsigned int rlo;
    unsigned int rhi;
    int n;

    if (shift == 0)
        return v;
    u.q = v;
    n = 32 - shift;
    if (n > 0)
        goto wide;
    rlo = u.h.hi >> -n;
    rhi = 0;
    goto join;
wide:
    rlo = (u.h.lo >> shift) | (u.h.hi << n);
    rhi = u.h.hi >> shift;
join:
    u.h.lo = rlo;
    u.h.hi = rhi;
    return u.q;
}
