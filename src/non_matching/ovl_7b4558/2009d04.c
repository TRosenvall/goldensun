/* OvlFunc_927_2009d04 -- 0x02009d04,
 * asm/overlays/rom_7b4558/ovl_30_c_c_c_a_a_c_a_a.s
 *
 * 81 of 81 lines, TWO differing.  Candidate at scratch/Nd04_best.c.
 *
 * The OvlFunc_927 cutscene template at actor slot 0xf, and a near-twin of
 * OvlFunc_927_2009244 (elevated, src/overlays/rom_7b4558/ovl_30_c_c_a_c_a_c_c_a.c)
 * -- same two __MapActor_GetActor(0) coordinate reads, same gState byte store
 * through a named base, same __Func_8091eb0 tail.  Everything the twin taught
 * carried over and the whole body came out right on the first screen.
 *
 * BLOCKER: argument-construction interleave at the SECOND OvlFunc_927_2008d90.
 *      rom   mov r3, #0xc0 / lsl r3, #0xb / mov r1, r5
 *      ours  mov r3, #0xc0 / mov r1, r5   / lsl r3, #0xb
 * The ROM keeps the constant's mov/lsl pair CONTIGUOUS and we split it with the
 * copy of the saved coordinate.  This is the same class as
 * src/non_matching/ovl_77a7c8/200b57c.c and it wants the interleave lever run
 * BACKWARDS -- less interleaving, not more -- and the only control over that is
 * where the constant is defined, in a function with no branch to define it in.
 *
 * WORTH NOTING FOR THE CLASS: the FIRST OvlFunc_927_2008d90 call in this same
 * function is interleaved in the ROM (`mov r1, #0xec / mov r3, r6 / lsl r1, #1`)
 * and we reproduce that one exactly.  So the two sites of one callee want
 * opposite treatment, which is the shape the per-call-site declaration lever was
 * written for -- and it does not reach this one.
 *
 * SCREENED AND UNCHANGED AT 2: the fourth argument as a local assigned just
 * before the call (that one is WORSE, 82 lines and 24 differing, because naming
 * it buys a callee-saved register); the second coordinate as a named local;
 * `short hx` instead of `int hx`; the two halfword reads as typed struct fields
 * -- which is what fixed 200b57c and does nothing here; swapping the
 * declaration order of the locals; hoisting `z` above the GetActor call;
 * deleting OvlFunc_927_2008d90's prototype; and an `__asm__` alias with a
 * different return type applied to the second call site, then to the first.
 */
