/* OvlFunc_943_20090a0 -- 0x020090a0, asm/overlays/rom_7c7b9c/ovl_30_c_a_a_c_a_c_a_c_a_a.s
 * Twin of OvlFunc_943_20091c8 -- 0x020091c8, SAME .s file.
 *
 * A clean twin family: the two differ in FOUR immediates (0xe8/0xca, 0xf4/0xc0,
 * and 0xf8/0xb4 twice), so one solution elevates both.
 *
 * 102 of 102 lines, TWO differing.  Candidate: scratch/W90a0_D.c.
 *
 * THE INTERLEAVE LEVER DID ALMOST ALL OF IT -- 18 differing to 2 -- and this is
 * the largest number of sites it has fixed at once: SIX.  Every call whose
 * argument setup the ROM interleaves has its non-r0 arguments named in the block
 * dominating the guarded region (before the outer `if`), and r0 left a literal:
 * two __MapActor_SetSpeed pooled pairs, three __Func_80921c4 third arguments,
 * the __MapActor_SetPos split builds, and the __MapActor_Emote pooled argument.
 *
 * Naming them next to their calls instead does NOTHING (18, unchanged); naming
 * them before the outer `if` is what works, even though the values then cross
 * two __GetFlag calls.  Both were screened, and it is the same dominance
 * precondition confirmed on OvlFunc_948_200a188 and OvlFunc_932_200a9dc.
 *
 * BLOCKER: the commutative register-role swap, at ONE of the two `*p |= 1` sites.
 *      rom   orr r6, r3 / strb r6      (mutates the constant register)
 *      ours  orr r3, r6 / strb r3      (preserves it)
 * The FIRST of the two sites matches exactly; only the second differs.
 *
 * THE INTERESTING PART, and why this is not just another instance: the two MASK
 * sites in the same function have the identical shape -- `and` with a value held
 * across calls -- and gcc gets BOTH right on its own, preserving r5 at the first
 * and destroying it at the second exactly as the ROM does.  So gcc is capable of
 * the preserve-then-mutate pattern; it simply does not choose it for the `orr`.
 * That kills the obvious theory that the class is about liveness, because the
 * liveness structure of the mask and the bit is the same.
 *
 * TRIED, all 2 except where noted: constant on the left at both OR sites and at
 * the second only; spelling the second as a mutation (`b |= *p; *p = b;`) which
 * is what the ROM literally does -- WORSE, 103 lines and 42 differing, because
 * the mutation costs an instruction; CSE, GCSE, ALIAS, STRENGTH.  SCHED2 is
 * worse (24).
 *
 * Same class as 200a5c0.c, where seven spellings also failed.  Do not spend a
 * round on it; the note above about the mask is the new information.
 */
