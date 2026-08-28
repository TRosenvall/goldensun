/* OvlFunc_952_20085a4 -- 0x020085a4,
 * asm/overlays/rom_7d768c/ovl_30_c_a_a_c_c_c.s
 *
 * 76 of 76 lines, 3 differing -- and one of those is only the symbol spelling,
 * so it is really 2.  Candidate at scratch/L85a4.c.
 *
 * A SYMBOL BASE MAKES GCC DERIVE WHERE AN INTEGER CONSTANT DOES NOT.
 * This is the useful result and it extends the batch-123 derived-constant rule.
 *
 * The ROM holds a message id in a callee-saved register and derives two more
 * from it:
 *      ldr r5, =0x2352 ... add r0, r5, #0x2 ... add r0, r5, #0x3
 *
 * Written with a plain `int m = 0x2352;` gcc emits three independent pool loads
 * (=0x2352, =0x2354, =0x2355) and does not even keep m alive -- rematerialising
 * a pool constant is cheaper than a push/pop pair, so it never spends r5.
 * Mutating the variable (`m += 2;`) is worse still: each arm then folds
 * independently, 73 differing.
 *
 * Declaring the base as an absolute SYMBOL --
 *      extern int _MSG_2352;
 *      m = (int)(&_MSG_2352);
 * -- produces `ldr r5, =_MSG_2352` and both `add r0, r5, #K`.  73 differing
 * -> 3.  gcc will spend a callee-saved register to hold a symbol address and
 * derive from it, where it will not do the same for an integer.
 *
 * `_MSG_2352` is NOT yet in message.sym; the screen was run with the extern
 * declared and unresolved, which tryc.py shows as a symbol-versus-number
 * difference on that one line.  Adding it is a one-line linker-fragment change
 * that emits no bytes -- worth doing when the rest of this function is
 * reachable, and worth trying on any function where the ROM derives message ids
 * from a common base.
 *
 * BLOCKER: `__ActorMessage(0xe, 0)` appears in both arms of the branch, and the
 * ROM emits its two arguments in DIFFERENT orders in the two arms -- r0 first in
 * the then-arm, r1 first in the else-arm.  Ours emits r0 first in both.  Neither
 * argument is a split build, so the interleave lever has nothing to work with;
 * this is the same bound as src/non_matching/overlays/20082b8.c.  Naming the
 * zero in the else arm alone changes nothing.
 */
