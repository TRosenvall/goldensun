/* OvlFunc_949_20082f0 -- 0x020082f0, asm/overlays/rom_7d4af4/ovl_30_c_c_a_c_c_a.s
 * Twin of OvlFunc_949_20083d0 -- 0x020083d0, SAME .s file.
 *
 * A clean twin family (neither member previously parked).  They differ only in
 * constants -- 0x10 against 0x11 and six message ids each one higher -- so one
 * solution elevates both.  Candidate: scratch/L82f0.c.
 *
 * THIS TU IS ALMOST CERTAINLY -O1, AND THAT IS THE FINDING.
 *
 *      default (-O2)   81 lines against the ROM's 86, 62 differing
 *      --O1            86 against 86, FIFTEEN differing
 *
 * The length agreeing only at O1 is the signal: the ROM's five extra
 * instructions are work that -O2 optimises away here, not something a source
 * spelling can add back.  Note the Makefile already carries an explicit O1 rule
 * for a sibling in this same overlay directory (ovl_30_c_c_a_c_c_c_c_c_c_b),
 * so an O1 TU here is unsurprising; a rule for this file would have to be added.
 *
 * WHY -O2 IS FIVE SHORT, since it explains the shape rather than just the count:
 * the six message-id arms each end by assigning one id, and the ROM loads each
 * id straight into r0 and branches to a single `bl __MessageID`.  At -O2 gcc
 * hoists the pool load ABOVE the `cmp r0, #0` that tests the __GetFlag result,
 * so r0 is still live and the merged value has to live in r3 instead, changing
 * the branch structure.  At -O1 the hoist does not happen.
 *
 * CONFIRMED ON THE WAY (worth keeping even though the function is parked):
 * at -O2 the compound `*f |= 2;` reproduces the ROM's POOLED constant
 * (`ldr r3, =0x2`) exactly as docs/elevation.md's HImode-literal rule predicts.
 * At -O1 the same source gives `mov r2, #0x2`, so that rule is -O2-specific --
 * which the doc does not currently say.
 *
 * REMAINING AT -O1, 15 differing: adjacency swaps in the prologue (the iwram
 * load against the GetActor argument, and `mov r8, r2` against the r6 setup)
 * and the register roles on the halfword OR.  TRIED, all 16 -- i.e. slightly
 * worse: swapping the source order of the `saved` and `f` assignments,
 * `*f = 2 | *f`, and an int intermediate for the OR.
 *
 * The OR role swap is the same signature as 200a5c0.c in this directory, which
 * was parked this round after seven spellings.  Do not re-spend a round on it.
 */
