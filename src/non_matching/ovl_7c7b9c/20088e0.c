/* OvlFunc_943_20088e0 -- asm/overlays/rom_7c7b9c/ovl_30_a_c_c.s
 *
 * BLOCKER: REGISTER ALLOCATION (three-way cyclic rotation)
 *
 * 11 of 46 differing.
 *
 * Same length.  ROM r5 = actor, r6 = &f62, r7 = copy of the byte; ours r6, r7, r5.
 * Plus ROM `ldrb r3 / mov r7, r3` against our `ldrb r5` and a later `mov r3, r5`.
 * * The structural half IS solved and is the reusable part: written with a shared
 * result variable and ONE store after the if -- including the goto spelling --
 * gcc speculates the cheap arm above the compare and inverts the branch, and the
 * small block vanishes: 45 of 46.  Writing the store INSIDE each arm and letting
 * cross-jumping merge the two identical strb's reproduces the ROM's layout: 11.
 * * MEASURED at 11: int vs unsigned int byte (45); a second local aliasing the byte;
 * a->f06 = 0 literal instead of = c; struct-field access with no pointer local;
 * -fno-rerun-cse-after-loop.
 */
