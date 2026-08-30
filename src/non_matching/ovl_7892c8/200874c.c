/* OvlFunc_888_200874c -- 0x0200874c,
 * asm/overlays/rom_7892c8/ovl_30_c_c_a_a_a_c_a_c_a.s
 *
 * 87 lines against the ROM's 89, 47 differing, all of it in ONE run of seven
 * stores through a base pointer.  Everything before and after -- seven
 * __MapActor_SetAnim calls, the fade pair, the choice prompt, the two-armed
 * tail -- is exact.  Candidate at scratch/N874c_best.c.
 *
 * THE STORE BLOCK, and three separate residues in it:
 *
 *   ROM   ldr r0, =0xe5a / ldr r4, =0xe5c / mov r1, #0xf8 / lsl r1, #7
 *         add r3, r2, r0 / strh r1, [r3] / add r3, r2, r4 / add r4, #2 ...
 *   ours  ldr r0, =0xe5a / ldr r1, =0x7c00
 *         add r3, r2, r0 / add r0, #2 / strh r1, [r3] ...
 *
 *   1. THE HALFWORD VALUE IS POOLED.  We emit `ldr r1, =0x7c00`; the ROM builds
 *      it `mov r1, #0xf8 / lsl r1, #7`.  Same shape as
 *      src/non_matching/ovl_7b2078/2008afc.c, where naming the value in an int
 *      local fixes it -- and here that fix costs more than it buys: 47
 *      differing -> 52, because it changes which register every offset lands in.
 *   2. THE OFFSETS.  The ROM keeps 0xe5a and 0xe5c in TWO registers and derives
 *      only the third (`add r4, #2`); we keep one and mutate it twice.  The
 *      same at the byte stores: the ROM pool-loads 0x2a01, 0x2a02 and 0x2a03
 *      separately where we derive each from the last with `add r0, #1`.
 *      Writing them as separate literals is what the candidate already does --
 *      gcc CSEs them into a chain anyway.  A named mutated offset for the pair
 *      (the batch-141 lever) goes the wrong way: 85 lines, 55 differing.
 *   3. THE ZERO BYTE STORE.  The ROM has `ldr r0, =0x0` -- a pooled zero -- and
 *      gcc notices the address register for 0x2a00 already has a zero low byte
 *      and emits `strb r3, [r1]` off the ADDRESS.  That is correct code and one
 *      instruction shorter, and there is nothing to write in C that forbids it.
 *
 * Residues 1 and 3 are both the narrow-store constant question that
 * 2008afc and src/non_matching/ovl_77a7c8/200b57c.c also park on, and the three
 * of them disagree about which way gcc goes: 200b57c pools a zero the ROM movs,
 * this one movs a value the ROM pools, and 2008afc needs one of each.  The
 * typed-field lever settles it when the destination is a struct member; here
 * the destinations are seven different offsets off one iwram pointer and there
 * is no struct to hang them on without inventing one.
 *
 * NEXT THING TO TRY: build the struct.  If `iwram_3001ed0` gets a declared type
 * with `short` at 0xe5a/0xe5c/0xe5e and `unsigned char` at 0x2a00..0x2a03, all
 * three residues are in scope for the one lever, and docs/structs.md says no
 * such struct exists yet.  That is a bigger change than a spelling and it is
 * the reason this is parked rather than pushed further today.
 */
