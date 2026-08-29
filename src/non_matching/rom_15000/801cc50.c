/* Func_801cc50 -- asm/rom_15000/rom_1ca1c_a_a_c.s
 *
 * BLOCKER: REGISTER ALLOCATION around a THREE-SITE .call_via
 *
 * 53 of 57, ours 56 lines (one short).  Semantics are settled: three calls to
 * the IWRAM helper Func_8000888 on p[0]/p[1]/p[2] << 16, each result >> 16,
 * each clamped to [0, 0x1f], combined as r + (b << 10) + (g << 5).
 *
 * ONE FIX FOUND HERE THAT TRANSFERS TO THE WHOLE .call_via CLASS:
 * do NOT pass the callee as a parameter to the inline-asm helper.  Doing so
 * costs an extra instruction, because gcc materialises the address in some
 * register and then copies it into the bound one:
 *
 *     ours  ldr r5, =Func_8000888 / mov r4, r5      <- helper takes (f, a, b)
 *     rom   ldr r4, =Func_8000888
 *
 * Assigning the symbol straight to the register-bound variable inside the
 * helper -- `register int (*_f)(int,int) __asm__("r4") = Func_8000888;` --
 * emits the ROM's single `ldr r4, =...` and also freed r5 for the struct
 * pointer, fixing `mov r5, r0` at the same time.
 *
 * WHAT IS LEFT: the ROM copies argument 2 out of r2 into r6 before the first
 * call and keeps the first result in r7; we keep the first result in r6 and
 * never free r2.  The reason the ROM moves r2 is NOT that the call clobbers it
 * -- argument 3 survives all three calls in r3, which proves it does not -- but
 * that r2 is wanted as the INDEX register for `ldrsh r0, [r5, r2]`.  We use r4
 * then r1 for that index instead.
 *
 * MEASURED:
 *   helper takes (f, a, b)                                    49 of 57
 *   helper binds the symbol directly (kept)                   53, ours 56
 *   ... plus "r2","r3" added to the clobber list       61, ours 62 (worse --
 *       the extra clobbers buy saves that the ROM does not have, and they are
 *       wrong: r3 demonstrably survives)
 *
 * Best C: scratch/s1cc50.c.
 */
