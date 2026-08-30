/* OvlFunc_926_2008afc -- 0x02008afc,
 * asm/overlays/rom_7b2078/ovl_314_c_c_a_c_c_a.s (SINGLE-function file)
 *
 * 76 lines against the ROM's 79, and the three missing lines are ONE cause.
 * Candidate at scratch/N8afc_best.c.
 *
 * SOLVED -- TWO WAYS TO DEFEAT THE HALFWORD POOL, AND THEY COST DIFFERENTLY.
 * `*(short *)(e + 6) = 0x80 << 7` compiles to `ldr r3, =0x4000`: gcc pools a
 * constant that is the operand of a HALFWORD store even when it could build it
 * with mov/lsl.  Two spellings defeat that and they are not equivalent:
 *
 *   - NAMING the value in an int local gives `mov r3, #0x80 / lsl r3, #7`, the
 *     ROM's shape, but the local takes a CALLEE-SAVED register (r5 here) and
 *     this function needs r5 for something else.  17 differing -> 10, with the
 *     register wrong.
 *   - A TYPED SHORT FIELD gives the same instructions in a SCRATCH register --
 *     `mov r3` where the named version gets `mov r5` -- and costs nothing.
 *     17 -> 12, and every line before the tail is then exact.
 *
 * Prefer the struct.  This is the same lever as
 * src/non_matching/ovl_77a7c8/200b57c.c, and between the two the rule is now:
 * a narrow store whose value is a literal goes through the pool unless the
 * DESTINATION is a typed field.  Casting the address does not do it; naming the
 * value does it at the price of a register.
 *
 * ALSO SOLVED: the zero stored to three fields is ONE named local, which is the
 * exception to "do not name zeros" -- the ROM keeps it in r5 across four
 * __MapActor_GetActor calls, and writing three literal zeros gives three movs.
 *
 * BLOCKER: RELOAD REMATERIALISATION.  The ROM does not keep that zero live
 * across the message branches; it rematerialises it at the last use, and the
 * remat is `ldr r5, =0x0` -- a POOL LOAD, because a constant materialised by
 * reload after register allocation never goes through the thumb mov/lsl
 * splitter.  We keep the pseudo in r5 for the whole function and store it
 * directly, which is one instruction shorter, and the missing pool entry then
 * lets our pool sit past the epilogue where the ROM has to jump over it -- so
 * one absent reload costs three lines.
 *
 * That is a decision of the register allocator with a register to spare, and no
 * spelling reaches it.  SCREENED, all worse: a second zero pseudo assigned at
 * the top and used only for the last store (15 differing); the same with the
 * first byte store included (15); one zero used for all four stores (62); the
 * whole tail written into each of the three message arms so gcc could
 * cross-jump them, which is where docs/elevation.md says the pooled zero comes
 * from -- gcc 2.96 does NOT merge them and the function goes to 107 lines.
 *
 * To move this one you need to make the allocator SPEND r5 on something else in
 * the middle of the function, and there is nothing else that wants it.
 */
