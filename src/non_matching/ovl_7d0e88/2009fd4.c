/* OvlFunc_947_2009fd4 -- asm/overlays/rom_7d0e88/ovl_1528_a_a_c.s
 *
 * BLOCKER: orr OPERAND ROLES at the LAST of four mask sites -- 2 of 39
 *
 * 37 of 39 exact.  The function masks two actor bytes with 0xfe, calls two
 * helpers, then ORs the same two bytes with 1.  THREE of the four sites match,
 * including both `and` sites and the first `orr`:
 *
 *   and site 1   rom mov r3, r5 / and r3, r2     (copy, mask stays live)   OK
 *   and site 2   rom and r5, r3                  (destructive on the mask) OK
 *   orr site 1   rom orr r3, r5                  (copy)                    OK
 *   orr site 2   rom orr r5, r3                  ours orr r3, r5           XX
 *
 * The difference between `and` site 2 and `orr` site 2 in the SOURCE is only
 * what happens to the mask afterwards: after the second `and` it is reassigned
 * (`m = 1`), after the second `orr` it simply dies at the end of the block.
 * gcc destroys the mask register in the first case and not the second; the ROM
 * destroys it in both.
 *
 * MEASURED (all 39 lines, all 2 differing at position 32 unless noted):
 *   *p = m | *p;                                          2
 *   *p = *p | m;                                          2   (canonicalised)
 *   *p |= m;                                              2
 *   m = m | *p; *p = m;                       10 of 39, ours 40 lines (worse)
 *   a separate `m2` for the two orr sites                 2
 *   unsigned char m instead of int           18, first diff at 8 (much worse)
 *
 * The "constant as ORR destination" lever in the doc is about which operand is
 * the destination; here BOTH spellings of that produce the same output and the
 * choice is made downstream, by whether the mask's live range ends in an
 * assignment or at a block boundary.  That is not currently reachable from C.
 *
 * Best C: scratch/y9fd4.c.
 */
