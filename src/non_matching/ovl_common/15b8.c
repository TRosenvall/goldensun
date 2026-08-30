/* OvlFunc_common1_15b8 -- asm/overlays/common/common1_a_c_c_c.s
 *
 * 34 of 34 lines, SIX differing.  Candidate at scratch/N15b8_best.c.
 *
 * The residue is the byte store `a->f5b = 0`.  Its offset is past the strb
 * immediate range so the address must be computed, and the ROM and gcc disagree
 * about both the ORDER of that computation and which scratch register holds
 * what:
 *      rom   asr r3,#1 / str r3,[r5,#0x34] / mov r3,r5 / mov r2,#0 /
 *            add r3,#0x5b / strb r2,[r3]
 *      ours  mov r2,r5 / asr r3,#1 / add r2,#0x5b / str r3,[r5,#0x34] /
 *            mov r3,#0 / strb r3,[r2]
 *
 * A USEFUL NEGATIVE ABOUT THE FLAG.  `-fno-schedule-insns2` takes it from 6
 * differing to 4 -- the scheduler really is interleaving the address
 * computation into the preceding store -- but the four that remain are the
 * r2/r3 SWAP alone, which the flag does not touch.  So this function does NOT
 * match under -fno-schedule-insns2 and adding a SCHED2_CFLAGS rule for it would
 * be wrong.  The tree has that group for two TUs that genuinely match with it;
 * this is not a third.
 *
 * What is underneath is the SCRATCH-register rotation of
 * src/non_matching/rom_c9000/80e3a3c.c: no push differs, nothing is spilled,
 * and gcc simply takes the two free registers the other way round.
 *
 * SCREENED AND INERT, all still 6: a named zero for the byte store; the shifted
 * value in its own local.  WORSE: the byte store written before the two int
 * stores (9 differing) -- it reorders more than it fixes.
 *
 * Settled: `v = 0xa0 << 9` with `a->f34 = v >> 1` gives the ROM's `asr r3, #1`
 * off the same register, and __Actor_Stop takes the actor in r0 left over from
 * __GetFieldActor, so it needs no explicit reload.
 */
