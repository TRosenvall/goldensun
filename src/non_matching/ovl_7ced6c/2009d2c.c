/* OvlFunc_946_2009d2c -- 0x02009d2c,
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a_c.s
 *
 * 83 lines against the ROM's 87.  Candidate at scratch/N9d2c_best.c.
 *
 * BLOCKER: THE SHARED CALL TAIL, third confirmed instance.  Four paths pick a
 * value and reach ONE call to OvlFunc_946_2009774; the ROM loads the value in
 * each arm and branches to the shared block:
 *
 *      rom   cmp r3, #2 / bhi .L1d66 / mov r2, #0x80 / b .L1d96
 *      ours  mov r2, #0x80 / cmp r3, #2 / bls .L1d96
 *
 * gcc hoists the load ABOVE its own compare and inverts the branch to fall into
 * the call -- exactly the if-conversion recorded on
 * src/non_matching/ovl_common/4cc.c and src/non_matching/ovl_7fb4a8/2008e10.c.
 * Four lines short and the whole chain shifts.
 *
 * NOT RE-SPENT HERE.  Those two files between them screen six spellings of this
 * shape -- separate calls per arm, a shared local for the value, explicit gotos
 * to the join, and the prototype lever on the callee -- and none of them
 * reaches it.  This function is filed as the third data point, not as another
 * six rounds of the same measurements.  When the class breaks, retry all three.
 *
 * The rest is believed right and screens clean up to the first arm: four
 * `>> 20` reads off __MapActor_GetActor into a, b, c, d; the two range tests
 * spelled `(unsigned)(x - 0x1f) <= 2`, which is what produces the ROM's
 * `sub r3, #0x1f / cmp r3, #2 / bhi`; the b == 7 arm returning without the
 * call; and the two six-argument __Func_8010704 tails sharing `s = a - 1` as
 * both the first register argument and the [sp] slot.
 */
