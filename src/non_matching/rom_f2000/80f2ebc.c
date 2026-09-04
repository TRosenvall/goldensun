/* Func_80f2ebc -- 0x080f2ebc, asm/rom_f2000/rom_f2028_c_c_a_a.s
 * Twin of Func_809088c -- 0x0809088c, asm/rom_8a000/rom_8d9a4_c_c_c_a_a_a_c.s
 *
 * The twins differ in ONE constant (0x5ff against 0x53f), so one solution
 * elevates both.  A clean family: neither member was parked before now.
 *
 * 39 of 39 lines, TWO differing -- and ONE OF THOSE TWO IS COSMETIC.
 * Candidate: scratch/L2ebc_B.c, screened with --cflags "-mlong-calls".
 *
 * The C is finished.  Fixed-count loop, guard on the divisor, counter spilled
 * to [sp] across the call because every callee-saved register is already in
 * use.  The two source loads are named in the ROM's order (u = *a before
 * v = *b), which is what puts them in the ROM's registers.
 *
 * WHAT IT NEEDS, AND WHY IT CANNOT HAVE IT:
 *
 *  1. -mlong-calls.  Without it gcc emits `bl __divsi3` directly and the
 *     function is four lines short; with it the r10 setup is hoisted out of the
 *     loop exactly as the ROM has it.  No flag group in the Makefile carries
 *     this flag today, so one would have to be added.  That part is easy.
 *
 *  2. `bl _call_via_sl` against the ROM's `bl _call_via_r10` is NOT a real
 *     difference.  src/lib/call_via.o defines both names at offset 0x28 -- they
 *     are the same veneer, so the linked bytes agree.  Only the screen sees it.
 *
 *  3. `ldr r1, =__divsi3` against the ROM's `ldr r1, =divsi3_RAM` IS real, and
 *     is the blocker.  gcc-2.96 has no flag to rename __divsi3.
 *
 * THE ESTABLISHED REMEDY DOES NOT TRANSFER.  Overlays fix this with a linker
 * alias -- `__divsi3 = _divsi3_RAM;` in overlay.ld, see
 * src/overlays/rom_7a5214/ovl_17ec_c_b.c -- and it is safe there because each
 * overlay links separately.  The main ROM is ONE link, and 110 asm files under
 * asm/ outside overlays/ reference __divsi3 directly.  A global alias in
 * stage1.ld would redirect every one of them.  Measured, not assumed.
 *
 * What would actually work is a per-object symbol rename
 * (objcopy --redefine-sym __divsi3=divsi3_RAM) in a rule for this file alone.
 * The Makefile has no precedent for objcopy, so that is a new build technique
 * for one twin pair; left for a round that decides to adopt it deliberately
 * rather than smuggling it in here.
 *
 * ALSO TRIED, all worse: calling divsi3_RAM explicitly instead of using `/`
 * (36 lines with -mlong-calls, 35 without) and via a const function pointer
 * (35).  The division has to stay a `/` for the surrounding code to come out
 * right; the symbol is the only thing wrong with it.
 *
 * RE-EXAMINED IN BATCH 219 AND THE ANALYSIS ABOVE STANDS.  Batch 218 elevated
 * Func_80903bc, whose ROM also reaches divsi3_RAM through `bl _call_via_rN`,
 * by holding the helper in a FUNCTION-POINTER LOCAL -- and that looked like it
 * ought to unblock this pair, since this ROM's call is likewise indirect
 * (`ldr r1, =divsi3_RAM / bl _call_via_r10`).  IT DOES NOT TRANSFER, for the
 * reason already recorded two paragraphs up: there the pointer call IS the
 * whole computation, whereas here the surrounding loop only comes out right
 * when the division is written as `/`, and the pointer spelling was measured at
 * 35 lines against the ROM's 36.  The indirect CALL FORM is not what is
 * missing here -- `-mlong-calls` already produces it -- the POOLED SYMBOL NAME
 * is.  Do not re-try the function-pointer route; try the per-object
 * `objcopy --redefine-sym` rule described above, as a deliberate change.
 */
