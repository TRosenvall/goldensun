/* Func_808bec0 -- STRUCTURALLY BLOCKED.  NO CANDIDATE AND NO FIGURE IS OFFERED,
 * NO `Verify with:` RECIPE BY DESIGN -- there is no candidate body in this file, so
 * there is nothing for objcmp to measure and nothing for parkcheck to re-check.
 * parkcheck reporting this file UNCHECKABLE is CORRECT, not documentation debt.
 * DELIBERATELY.  This file is NOT a park and must not be fed to parkcheck: it
 * carries no "NON-MATCHING, N of M" line because measuring one would imply the
 * function is a decompilation target under the current toolchain, and it is not.
 *
 * Reference asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s, 470 instructions.
 *
 * ============ WHY, AND WHY THIS SHOULD NOT HAVE BEEN ASSIGNED ============
 *
 * `Func_808bec0` IS NAMED BY NAME IN docs/elevation.md, in the section
 * "`.call_via` IS AN INLINE VENEER gcc NEVER EMITS -- A STRUCTURAL BLOCKER CLASS"
 * (elevation.md:1473).  It is one of the seventeen still-available functions that
 * section lists, and that section ends with the instruction
 *
 *     **Do not assign these to a survey brief.**
 *
 * The block, restated so this file stands alone: include/macros.inc expands
 * `.call_via reg` to `.align 2,0 / mov r12, pc / bx \reg` -- four bytes of INLINE
 * interworking veneer.  gcc-2.96 with -mthumb-interwork compiles a call through a
 * function pointer as `bl _call_via_rN`, also four bytes and a completely different
 * encoding.  No C spelling closes that gap; the corpus check is decisive, 33
 * hand-written `.s` files contain `.call_via` and ZERO of the 4,300+ generated ones
 * do.
 *
 * VERIFIED HERE, not taken on the doc's word:
 *   - EXACTLY ONE site, at line 235 of the function's block (line 356 of the .s),
 *     `.call_via r4`, reached as
 *         ldr r4, =Func_8000888
 *         ldr r0, [r3]              @ r3 = scene + 0x1b0
 *         ldr r1, [r2, #0x30]       @ r2 = the player actor
 *         .call_via r4
 *     -- i.e. the ROM loads a KNOWN function's address into a register and calls
 *     through it.  A direct `Func_8000888(a, b)` in C gives `bl Func_8000888`, and a
 *     function-pointer call gives `bl _call_via_r4`; neither is the veneer.
 *   - `grep -c call_via` over the function's block: 1.  Over Func_8020244's block
 *     (the other large target in this brief): 0, so that one is unaffected.
 *
 * One site is the best case in that list of seventeen (UpdateActors has 31), so if
 * anyone ever finds a route to the inline form -- the doc names
 * -mcallee-super-interworking and friends as unexplored, and the original may have
 * used a macro or inline asm at this one call -- this is among the first functions
 * to retry.  Until then the four bytes cannot match and a park figure would only
 * measure the other 466 instructions against a wall.
 *
 * ============ WHAT WAS LEARNED ANYWAY, so a future attempt starts warm ============
 *
 * SPLIT SHAPE (computed, not guessed).  `python3 tools/datacheck.py
 * asm/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a.s` prints NOTHING -- no text/data split.
 * The .s holds TWO functions, CheckSpecialExits at 0x0808bde0 and Func_808bec0 at
 * 0x0808bec0, and the target is the LAST, so split_s.py is a clean TAIL split:
 * ..._c_a_c_a_a.s keeps CheckSpecialExits and the target becomes
 * src/rom_8a000/rom_8ba38_a_a_a_c_a_c_a_a_b.c.  Note that CheckSpecialExits is
 * CALLED BY the target, so the two must stay in link order.
 *
 * THE BODY, as far as it was traced.  FindInteractionTarget: four arguments
 * (origin a, x b, range c, z d).  It snapshots every party member's HP into a
 * `short[8]` at sp+0x1c from `_GetUnit(gState[0x1f8 + i])->f38` over
 * `_GetPartySize()`; picks a collision cell either from `ewram_2020000` on a 32x32
 * grid (`(b >> 21) & 0x1f` plus `((d >> 21) & 0x1f) << 5`, scene->f19e == 3) or from
 * a 128-wide grid off `*(iwram_3001ebc - 0x4c)` indexed `(b >> 20) + ((d >> 20) << 7)`;
 * rotates scene->f1b8 into scene->f1bc; and calls CheckSpecialExits(b, c, d) when the
 * cell's byte +2 is non-zero.  The rest applies damage/poison bookkeeping to gState
 * at 0x22c/0x22e/0x230/0x232/0x23e/0x244 and finishes with a second party sweep that
 * fills scene->f188[] with the ids of members whose f38 <= 0.
 *
 * TWO THINGS A FUTURE ATTEMPT WILL NEED.
 *  1. `-fcall-used-r4` IS SET FOR THIS FILE (objcmp's own flag set), so r4 is
 *     CALL-CLOBBERED and the ROM spills it around every call as
 *     `str r4, [sp] / bl ... / ldr r4, [sp]` -- using sp+0, the outgoing-argument
 *     slot, as r4's spill slot, because no call in that region passes a fifth
 *     argument.  Six such pairs.  That is not a source fact and must not be chased.
 *  2. The signed divisions are all `if (v < 0) v += mask; v >>= shift` rounding
 *     pairs -- `>> 21` with 0x1fffff, `>> 20` with 0xfffff, `>> 16` with 0xffff,
 *     `>> 1` via `lsr #31 / add / asr #1` -- i.e. plain `/ 0x200000`, `/ 0x100000`,
 *     `/ 0x10000` and `/ 2` on signed ints, NOT shifts in the source.
 */
