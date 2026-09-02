/* gfree and free  --  0x08002dd8 and 0x08002df0, asm/rom_c0/rom_2dd8.s
 *
 * BLOCKER CLASS: register birth order, plus a prologue gcc emits and the ROM
 * does not.
 * Status: gfree 15 lines against the ROM's 12; free 6 against 6 with the
 * registers permuted.
 *
 * WHAT THEY DO
 * Both rewind one of two arena bump-pointers held at the head of the block at
 * iwram_3001e50, choosing which by `(ptr >> 22) & 4` -- an EWRAM address gives
 * 0, an IWRAM address gives 4. `gfree` takes a tag and clears the tag slot
 * first; `free` takes the pointer directly. The .s comment explains why freeing
 * out of order corrupts the arenas.
 *
 * THE READING IS BELIEVED RIGHT: every instruction of `free` is present in our
 * output, only in different registers, and `gfree` differs by the same
 * permutation plus a prologue.
 *
 * THE PROLOGUE IS THE INTERESTING PART. The ROM's `gfree` is a leaf and returns
 * with a bare `bx lr`. gcc emits `push {lr}` and `pop {r0} / bx r0` -- three
 * instructions the ROM does not have -- even though nothing in the function
 * calls anything and `-fcall-used-r4` means r4 needs no saving. `free`, which
 * is the same shape without the early return, gets no prologue at all. So it is
 * the conditional return that provokes it.
 *
 * WHAT WAS TRIED: the early return written as an `if` around the body rather
 * than a `return`, which is the shape that has no second exit. Byte-identical.
 *
 * The `>> 22` test relies on the shift setting the flags -- the ROM's
 * `lsr r3, r1, #0x16 / beq` with no `cmp` -- and gcc reproduces that from
 * `t = p >> 22; if (t == 0)`, so that part needed no help.

 * UPDATE, batch 181 -- THE OPERAND ORDER IS FIXED; THREE INSTRUCTIONS REMAIN.
 *
 * The same pointer-typed-operand lever that fixed `free` next door applies to
 * both of gfree's memory accesses. The ROM has `ldr r1, [r0, r4]` and
 * `str r1, [r3, r4]` -- INDEX first, base second -- and every earlier spelling
 * produced [base, index]. Giving the scaled index the pointer type and holding
 * the table base as an `unsigned int` swaps them:
 *
 *     base = (unsigned int)gPtrs;
 *     k = (unsigned char *)(i << 2);
 *     p = *(unsigned int *)(k + base);
 *
 * TWO RESIDUES REMAIN, and they are separate problems.
 *
 *   AN EXTRA `cmp`. The ROM branches off the flags the shift itself sets --
 *   `lsr r3, r1, #0x16 / beq` with no compare -- and reuses r3 for the later
 *   `and`. We emit `lsr` then `cmp r2, #0`. The park's earlier note that gcc
 *   reproduces the flag-setting form from `t = p >> 22; if (t == 0)` DOES NOT
 *   HOLD when the shifted value is still live afterwards, which it is here:
 *   both the `== 0` early return and the `!= 0` guarded block emit the compare.
 *   `free` does not hit this because its shift result feeds only the mask.
 *
 *   THE PROLOGUE. gcc emits `push {lr}` and `pop {r0} / bx r0` where the ROM
 *   has a bare `bx lr`, on a leaf with nothing to save. `free`, the same shape
 *   without the conditional exit, gets no prologue, so the second exit is what
 *   provokes it -- and writing the body as one guarded block rather than an
 *   early return is byte-identical, so it is not the RETURN STATEMENT but the
 *   two basic blocks.
 *
 * Recorded as an improvement, not an elevation: 12 aligned of 12 still, because
 * the register rotation from `free` is present here too and now overlaps these
 * two. The operand-order half is settled and should not be re-tried.
 */

extern void *gPtrs[];

void gfree(int i)
{
    unsigned int p;
    unsigned int t;

    p = (unsigned int)gPtrs[i];
    t = p >> 22;
    if (t == 0)
        return;
    gPtrs[i] = 0;
    t &= 4;
    *(unsigned int *)((char *)gPtrs + t) = p;
}

void free(void *p)
{
    unsigned int t;

    t = ((unsigned int)p >> 22) & 4;
    *(void **)((char *)gPtrs + t) = p;
}
