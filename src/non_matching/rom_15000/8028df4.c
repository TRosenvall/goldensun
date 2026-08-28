/* YesNoMenu -- asm/rom_15000/rom_23178_a_a_c_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION -- a five-way permutation, ours 43 of the ROM's 45
 *
 * Both versions spend the same five callee-saved registers (r5, r6, r7, r8,
 * r10) and the same instructions; only the assignment differs:
 *
 *     rom   a->r7   b->r10  c->r5  d->r6   k->r8
 *     ours  a->r6   b->r8   c->r5  d->r10  k->r7
 *
 * The two-instruction gap is entirely the flag variable `k`.  In the ROM it
 * lands in r8, a HIGH register, which cannot take an immediate -- so `k = 0`
 * costs `mov r3, #0 / mov r8, r3` and `k = 0x11` costs `mov r3, #0x11 /
 * mov r8, r3`.  We put it in r7 and pay one instruction each time.
 *
 * MEASURED (all 39 differing of 45, ours 43 lines):
 *   int k; int r;   with k = 0 as the first statement          39
 *   int r; int k;   (declarations swapped)                     39
 *   k = 0 moved down to just before the `if (a != 0)`          39
 *
 * Nothing in the source chooses which of five equally-long live ranges gets
 * the high register.  The parameter-copy lever (batch 118) applies to the
 * ORDER of two entry `mov`s, not to the register set, and there is no second
 * value here to trade against.
 *
 * Best C: scratch/Cyesno.c.
 */
