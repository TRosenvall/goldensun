/* Func_80b8824 -- asm/rom_b5000/rom_b8228_c_a_c_a_c_a.s
 *
 * BLOCKER: WHICH REGISTER THE BUFFER ADDRESS IS BORN IN -- ours 44 of the
 * ROM's 45, one instruction short
 *
 *     rom  mov r5, sp / ... / bl Func_80b6b40 / ... / mov r8, r5 / mov r5, #0
 *     ours mov r8, sp / ... / bl Func_80b6b40 / ...              / mov r5, #0
 *
 * The ROM materialises the stack buffer's address into a LOW register (r5,
 * because it is the second argument of the call), and only after the
 * `if (n != 0)` guard copies it into r8 to free r5 as the loop's byte offset.
 * gcc puts it straight into r8 and never needs the copy, so we are exactly one
 * `mov` short and every register downstream renames.
 *
 * MEASURED:
 *   short buf[14], indexed as (char *)buf + off      44 differing, ours 39
 *                                                    lines (SIX short; gcc
 *                                                    folds base and offset
 *                                                    into one pointer and
 *                                                    never uses r8 at all)
 *   + `char *base` named, assigned inside the guard  30, ours 44
 *   + base assigned BEFORE the Func_80b6b40 call and
 *     passed as its argument                         30, ours 44
 *   + a SECOND local `walk = base;` inside the guard,
 *     mirroring the ROM's `mov r8, r5`               30, ours 44 (coalesced)
 *
 * The first step is the reusable one: naming the base so it does not fold into
 * the offset is worth five instructions and is what makes r8 appear at all.
 * The last step is the interesting negative -- agent3's round-6 finding that
 * giving a second computation its own variable rotates the allocation does NOT
 * apply to a plain copy of the same value; gcc coalesces it away.
 *
 * Best C: scratch/Bb8824d.c.
 */
