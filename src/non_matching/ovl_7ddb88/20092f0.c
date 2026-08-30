/* OvlFunc_955_20092f0 -- 0x020092f0, asm/overlays/rom_7ddb88/ovl_30_c_c_c_c.s
 *
 * 123 of 123 lines, 15 differing, and the instruction MULTISET is exact:
 * every mnemonic, every operand, every constant is right.  The whole residue is
 * the ORDER of the argument setup at six call sites.  Candidate kept at
 * scratch/H92f0_best.c.
 *
 * BLOCKER: SCHEDULER INTERLEAVE.  At every three-register-argument call whose
 * first argument is the actor slot, the ROM loads r0 LAST and we load it FIRST:
 *
 *      rom   mov r1, #0x9f / lsl r1, #3 / mov r2, #0xa8 / mov r0, #0 / bl
 *      ours  mov r1, #0x9f / lsl r1, #3 / mov r0, #0 / mov r2, #0xa8 / bl
 *
 * The same one-slot displacement of `mov r0` accounts for all fifteen: the six
 * sites are OvlFunc_common1_1078, four OvlFunc_common1_15b8 calls, and the tail
 * OvlFunc_common1_5e4 -- and that last one takes THREE VARIABLES, not literals,
 * so the residue is not about constants.  It is the position of the r0 load.
 *
 * WHAT DID WORK, and it is worth keeping: the basic-block lever from
 * OvlFunc_926_200a484 (src/overlays/rom_7b2078/ovl_314_c_c_a_c_c_c_c_a_a.c)
 * closed three of the original eighteen.  Assigning
 *
 *      sx = 0xc0 << 9;  sy = 0xc0 << 8;  m = 0x95 << 3;
 *
 * BEFORE the `if` and using them inside it makes gcc rematerialise each at its
 * call site, which is what splits `mov r1,#0xc0 / mov r2,#0xc0 / lsl r5,#3 /
 * mov r0,#0 / lsl r1,#9 / lsl r2,#8` the ROM's way.  Written inline at the call
 * those two sites are contiguous and wrong.  That lever is real and it is in
 * the best candidate; it just does not reach the other six.
 *
 * WHAT DID NOT MOVE IT.  Six further spellings, all screened, all still 15:
 *      hoist the 1078 constants as well                        16 (worse)
 *      hoist EVERY call-argument constant in the block         16 (worse)
 *      hoist k = 0xa1 << 3 to the top with the others         111 (much worse)
 *      one shared `z = 0` used as the slot argument everywhere  15
 *      `k -= 0x40` folded into the call argument               15
 *      a per-call-site `y` local for each y coordinate         15
 *      unprototyped callee declarations                        15
 *
 * Four different source-level levers leaving the count EXACTLY unchanged is the
 * tell that the residue is below the source: the r0 position is decided after
 * expansion and does not respond to how the call is written.
 *
 * SCHED2 IS ON AND IS DOING THE ROM'S WORK, not fighting it.  --no-sched2 goes
 * to 45 differing and diverges at line 1, so the ROM order IS the scheduled
 * order; ours is scheduled too and lands one slot off.  Do not reach for
 * --no-sched2 on this one.
 *
 * THE CONTRADICTION TO SOLVE FIRST, if this is retried.  The sibling
 * OvlFunc_926_200a484 emits the shape we want -- `mov r1,#0x98 / lsl r1,#1 /
 * mov r2,#0xd8 / mov r0,#0` -- from a call written with PLAIN LITERALS at the
 * call site, which is exactly what we write and exactly what fails here.  Same
 * C idiom, same compiler, different output.  Whatever distinguishes those two
 * blocks is the lever; find it there, where a match already exists, rather than
 * by permuting this function again.  Both blocks hold three callee-saved values
 * live, so it is not register pressure.
 *
 * Structure, for whoever picks it up: signed-halfword read of gState + 0xe1*2,
 * early return through OvlFunc_common1_2c4 when it is 2; otherwise
 * __CutsceneStart, a three-way dispatch on OvlFunc_common1_4cc(param, 1), a long
 * arm for 0 (message 0x209e, camera setup, five OvlFunc_common1_15b8 walk steps
 * off a running r5, an anim change and a surprise), a two-line arm for 1, and a
 * shared tail of OvlFunc_common1_5e4(result, param, 1) and __CutsceneEnd.  All
 * of that is confirmed correct by the exact multiset -- only the order is wrong.
 */
