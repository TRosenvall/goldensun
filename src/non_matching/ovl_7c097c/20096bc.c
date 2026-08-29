/* OvlFunc_936_20096bc -- 0x020096bc, asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_a_c_c.s
 *
 * 121 of 121 lines, SEVEN differing, down from 65.
 * Candidate: scratch/Q96bc_best.c.
 *
 * FOUR THINGS CARRIED IT AND ARE WORTH REUSING:
 *
 *   1. BOTH SIDES TYPED.  The actor and the sprite it points at (actor+0x50) are
 *      declared as structs with named fields rather than reached by pointer
 *      arithmetic.  Every one of the eleven field accesses reproduces.
 *   2. The guarded __MapActor_SetPos interleave, fixed by naming its two split
 *      builds in the dominating block.  First difference moved 34 -> 57.
 *   3. AN INT INTERMEDIATE FOR THE SECOND NEGATED MASK, worth 53 lines by
 *      itself (62 -> 9).  `s->f05 = s->f05 & -0x21` truncates to a byte and gcc
 *      emits one `mov r3, #0xdf`; the ROM builds -0x21 in SImode with
 *      `mov r3,#0x21 / neg r3,r3`.  I had applied this to the OTHER mask chain
 *      in the same function and missed this one -- the tell is a single mov
 *      where the ROM has mov+neg, and it also costs a line.
 *   4. Dropping __UploadSpriteGFX's prototype, which pushes its r0 argument to
 *      the end where the ROM has it.  9 -> 7.
 *
 * BLOCKER: the two mask chains.  Register roles are exchanged --
 *      rom   mov r2,#0xd / ldrb r3,[r6,#9] / neg r2,r2
 *      ours  mov r3,#0xd / ldrb r2,[r6,#9] / neg r3,r3
 * -- and the ROM INTERLEAVES the second chain's load (`ldrb r1,[r6,#5]`) into
 * the middle of the first chain, where ours keeps the two chains sequential.
 *
 * TRIED: naming both mask constants, with the constant assigned before the load
 * (9, no change); reordering the source so the second read happens early, which
 * is what the ROM's interleave looks like -- WORSE, 120 lines and 61 differing.
 *
 * The residue is the commutative register-role class, which batch 136 showed can
 * be a typing problem in disguise.  Here the typing is already done, so this is
 * the genuine article rather than a mislabelled one.
 */
