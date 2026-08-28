/* OvlFunc_932_200a5c0 -- 0x0200a5c0, asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_c_c_a_a_a_c.s
 *
 * 107 of 107 lines, TWO differing.  Candidate: scratch/La5c0_D.c.
 *
 * Everything else reproduces, including two things that needed levers: the
 * guarded __MapActor_SetPos interleave (both split builds named in the block
 * dominating the call -- 5 differing to 2) and the four six-argument calls whose
 * spilled stack arguments are named in pairs.
 *
 * BLOCKER: register roles on a COMMUTATIVE operation.
 *      rom   ldrb r2, [r5] / mov r3, #0x2 / orr r3, r2 / strb r3, [r5]
 *      ours  ldrb r3, [r5] / mov r2, #0x2 / orr r3, r2 / strb r3, [r5]
 * The `orr` and the `strb` are IDENTICAL; only which register receives the
 * loaded byte and which the constant differs.  The ROM puts the constant in the
 * orr destination, we put the loaded value there.  Both compute the same thing.
 *
 * THIS BOUNDS THE SOURCE-ORDER LEVER.  Assignment order picks registers for two
 * INDEPENDENT values (docs/elevation.md, "source order of two loads"), and it
 * did so twice in this same round.  It does not reach the operands of a
 * commutative operator: every ordering below gives the identical 2.
 *
 * TRIED, all 2 differing:
 *      *p = 2 | *p;          *p |= 2;          *p = *p | 2;
 *      v = *p; c = 2; *p = c | v;              c = 2; v = *p; *p = c | v;
 *      p[0x23] = 2 | p[0x23];   (no pointer advance)
 *      CSE, GCSE, ALIAS, STRENGTH.  SCHED2 is worse (24 differing).
 *
 * Related signature: the base/offset register-role swap parked in 2008054.c and
 * 20091c4.c.  Those are address registers rather than an ALU pair, but all three
 * are "the right instructions with the two registers exchanged", and none has
 * yielded to a source spelling.  If a lever is found for one, retry the others.
 */
