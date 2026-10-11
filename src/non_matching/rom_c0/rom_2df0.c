/* BACKFILLED FIGURE (batch 337) -- NOT MATCHING, 5 differing encodings of 7.
 *
 * This park carried NO PARSEABLE FIGURE AND NO RECIPE, so it was invisible to
 * every targeting instrument in this tree: absent from the figure bands,
 * NO CLAIM in parkcheck, unrankable in reports/frontier.md.  170 of 675 park
 * files were in that state; 26 of them had a compilable body, which means a
 * figure was one compile away.  tools/backfill.py found them and this is the
 * measurement.
 *
 * ref 7 / ours 7 stream encodings; 6/6 16-bit instructions -- IN PHASE, so this is a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/rom_c0/rom_2df0.c \
 *     asm/rom_c0/rom_2dd8.s --func free
 *
 * THE FIGURE IS EVIDENCE; ANY DIAGNOSIS BELOW IS A HYPOTHESIS.  Across pass two
 * a park's stated blocker has been wrong roughly 40 times in 42.
 */
/* free @ 0x08002df0 -- asm/rom_c0/rom_2dd8.s
 *
 * Source asm: goldensun/asm/rom_c0/rom_2dd8.s
 *
 * Blocker class 2, REGISTER BIRTH ORDER. Six instructions, all six right,
 * every register different:
 *
 *     rom    ldr r4, =gPtrs / mov r1, #4 / lsr r2, r0, #22 / and r2, r1 / str r0, [r2, r4]
 *     ours   lsr r3, r0, #22 / mov r2, #4 / and r3, r2 / ldr r2, =gPtrs / str r0, [r3, r2]
 *
 * The ROM loads the array base FIRST, so it takes r4; gcc computes the shift
 * first and the base ends up last. Tried: the base in a u8* local, in a
 * void** local, and inline; the arena index in its own local and inline. The
 * base load stays last every time.
 *
 * Its sibling gfree (0x08002dd8) is parked for a related reason -- gcc emits
 * a push/pop frame there that the ROM does not have.
 */
#include "gba/types.h"

extern void *gPtrs[];

/* Rewinds whichever arena the pointer belongs to back to it, taking the
 * address directly rather than a tag. Bit 22 of the address selects the
 * arena: an EWRAM pointer gives 0, an IWRAM pointer gives 4.
 *
 * CALLERS MUST FREE IN REVERSE ALLOCATION ORDER. This is a bump-pointer
 * rewind, not a free list -- everything allocated after the block is silently
 * reclaimed with it.
 */
void free(void *ptr)
{
    *(void **)((u8 *)gPtrs + (((u32)ptr >> 22) & 4)) = ptr;
}
