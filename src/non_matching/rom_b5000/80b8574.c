/* Func_80b8574 -- 0x080b8574, asm/rom_b5000/rom_b8228_c_a_c_a_a_c_a.s
 *
 * 105 differing of 178, at IDENTICAL size (376 bytes) and identical encoding
 * count, with the sort loop byte-identical for its last fourteen instructions.
 * Candidate and sweep in scratch_elev/b258/mixed/.
 *
 * BLOCKER CLASS: GLOBAL REGISTER-ALLOCATION ORDER. `.18.greg` ranks the output
 * counter 19th of 21, below loop 2's zero, so it takes r11 where the ROM has r9.
 *
 * THE FLOOR IS 101, NOT 105, AND THE DIFFERENCE IS A PIN WE WILL NOT SHIP.
 * `register int cnt __asm__("r9")` confirms the diagnosis -- 105 -> 101, with no
 * second definition of r9, so this is NOT the recorded pin-miscompile class --
 * but it is a forbidden high-register pin worth only 4 encodings. Shipped
 * without it. Anyone re-screening should know both numbers: 101 is what the
 * correct diagnosis buys, and it is still not a match.
 *
 * ALIAS IS THE WRONG AXIS: `-fno-schedule-insns2` REGRESSES, 138 -> 156.
 *
 * THE BIGGEST LEVER BY FAR WAS ONE LOCAL PER VALUE, and it is worth reading as
 * a series rather than a single result: a separate id for loop 2 is 130 -> 123;
 * a separate counter for the sort's inner loop is 123 -> 101; and A THIRD SUCH
 * SPLIT IS WORSE (105). So the rule has an optimum rather than a direction --
 * splitting is not monotonically good.
 *
 * THE FILE-MATE LEAD PAID, AS PREDICTED. The solved sibling
 * src/rom_b5000/rom_b8228_c_a_c_a_a_c_c.c supplied the Ent struct and the
 * _GetUnit spelling verbatim, where the ranked neighbour did not. And the DMA3
 * swap is include/dma.h's DMA3_COPY, exact unaided.
 *
 * LANDING IF CLOSED: asmfacts.py says `3 functions  split first`, and BOTH
 * siblings are ALREADY PARKED (src/non_matching/rom_b5000/80b84c0.c and
 * rom_b8530.c), so this file cannot land whole until all three close. MAIN ROM,
 * so its linker line is in stage1.ld and MUST KEEP its asm/ path.
 * makefile_flags() is empty, no data, no cross-file `.L`.
 */
