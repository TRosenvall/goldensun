/* OvlFunc_931_2008b2c -- 0x02008b2c, asm/overlays/rom_7b8cb0/ovl_30_c_c_c_c_c_c_c_c_c_c_a_c.s
 *
 * 90 ROM lines against 91 of ours, 42 differing.  Candidate: scratch/Lb2c_B.c.
 *
 * WRITTEN TYPED FROM THE START, following the two park recoveries in batch 136,
 * and that part worked: the three `interactFlags |= 4` sites at actor offset
 * 0x59 and the two `facing` stores at 0x06 all reproduce, where raw pointer
 * arithmetic on those bytes is what parked OvlFunc_932_200a5c0.  Field names
 * taken from include/actor.h.
 *
 * Consuming the GetActor result INLINE where no null test is needed also helped
 * -- 68 differing to 42 -- because a named pointer reused across sites takes a
 * callee-saved register the ROM never spends.
 *
 * BLOCKER: constant reuse.  0x2c60000 is passed to two __MapActor_SetPos calls
 * in the same arm; gcc loads it once into r5 and adds `push {r5}`, where the ROM
 * reloads it from the pool at each call.  That single decision is the remaining
 * 42 positions and the one extra line.
 *
 * THIRD COUNTEREXAMPLE TO THE TWO-REMEDY RULE.  docs/elevation.md records that
 * this tell yields either to CSE_CFLAGS or to separate named locals.  Both fail
 * here, as they did on OvlFunc_881_2009c08 and OvlFunc_939_2008eb0:
 *
 *      CSE 42    GCSE 42    two named locals 42    named locals + CSE 42
 *
 * With three independent counterexamples the rule should be read as a diagnosis
 * with two things worth trying, not as a fix.
 *
 * Worth noting for selection: pool.py predicted this exactly with reuse = 1, and
 * the reuse column again pointed at the right constant.  The column identifies
 * the blocker reliably; it is the remedy that is missing.
 */
