/* OvlFunc_881_2009c08 -- asm/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_a_a_a.s
 *
 * BLOCKER: CONSTANT CSE, STRAIGHT LINE -- same as OvlFunc_882_200bc48, twice
 *
 * 34 of 49 differing, ours 52 lines (three too many).  21 calls, zero labels.
 * `__ClearFlag(0x16f)` and `__ClearFlag(0x171)`/`__SetFlag(0x171)` each appear
 * at two sites; the ROM reloads `ldr r0, =0x16f` and `ldr r0, =0x171` from the
 * pool at every site, gcc hoists both into r5 and r6.
 *
 * TWO THINGS MEASURED HERE THAT ARE NOT IN THE DOC:
 *
 *  1. gcc hoists a POOL LOAD, not just a multi-instruction build.  I expected
 *     it not to -- a pool load costs one instruction and so does the `mov` that
 *     replaces it, so there is nothing to gain.  It hoists anyway.  That means
 *     the "expensive constant" heuristic in tools/script_candidates.py must
 *     count `ldr rN, =V` as well as `mov`+`lsl`, and it does.
 *  2. THE SYMBOL-ADDRESS TECHNIQUE DOES NOT DEFEAT IT.  Adding
 *     `_CONST_16f = 0x16f;` / `_CONST_171 = 0x171;` to a bind-mounted copy of
 *     const.sym and spelling the arguments `(int)&_CONST_16f` leaves the line
 *     count at 52 -- gcc CSEs the symbol address exactly as it CSEs the
 *     integer.  The doc's rule that "two DISTINCT symbols of equal value
 *     reload" is about two different symbols; ONE symbol used twice is hoisted
 *     like anything else, and this is the measurement that separates the two.
 *
 * Best C is scratch/p9c08.c; the symbol variant is scratch/p9c08b.c.
 */
