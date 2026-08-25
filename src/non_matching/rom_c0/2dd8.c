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
