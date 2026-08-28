/* OvlFunc_897_200a84c -- asm/overlays/rom_791794/ovl_30_c_c_a_c_a_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION -- a clean r5/r6 swap, 16 of 65, same length
 *
 * Two arms of __CopyMapTiles calls.  Every instruction is right; the two
 * carried constants land in the opposite registers:
 *
 *     rom   one (1) -> r5,  two (2) -> r6
 *     ours  one (1) -> r6,  two (2) -> r5
 *
 * `one` is used seven times across BOTH arms, `two` twice in the then-arm
 * only, and the ROM gives the more-used value the lower register.  gcc does
 * the reverse.  Note the else-arm's single `2` is a plain literal in the ROM
 * (`mov r3, #2 / str r3, [sp]`), not the carried r6 -- so the source really
 * does name it in one arm and not the other, and that is reproduced.
 *
 * MEASURED (all 65 lines, all 16 differing at position 4):
 *   int one; int two;  (both function-scope)              16
 *   int two; int one;  (declarations swapped)             16
 *   `two` block-scoped inside the then-arm                16
 *
 * The declaration-order lever is inert here, as it is on most functions --
 * see the CONFLICT note in docs/elevation.md.  Nothing in the source reaches
 * the allocator's priority order for two values with these live ranges.
 *
 * Best C: scratch/za84c.c.
 */
