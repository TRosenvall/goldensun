/* OvlFunc_959_200c794 -- 0x0200c794, asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c.s
 *
 * 144 of 144 lines, SIX differing -- but only FOUR of those are real.
 * Candidate: scratch/Tc794.c.
 *
 * TWO of the six are `ldr r5, =_MSG_244f` and `ldr r5, =_MSG_2455` against the
 * ROM's raw ids.  Those would assemble to identical bytes the moment the two
 * symbols are defined, exactly as they did for its sibling
 * OvlFunc_959_200cbfc (elevated).  They are NOT added to message.sym here,
 * because a symbol that does not complete a match is an entry that does not pay
 * for itself -- the same reasoning as the SetTextColor park.  Add them at the
 * moment the remaining four lines fall, not before.
 *
 * SOLVED AND WORTH KEEPING:
 *   - The message-id bases as SYMBOLS taken by address.  The ROM holds each in
 *     r5 and derives neighbours (`add r0, r5, #1..#5`, and `add r5, #2` for the
 *     mutating one); the symbol form reproduces all of it, including which of
 *     the two bases mutates and which only derives.
 *   - Both early-return arms, the nested __Func_8091c7c branch, and every one of
 *     the seven __Func_8092c40 calls come out right WITH the prototype in place.
 *     Note the contrast with the sibling, where dropping that same callee's
 *     prototype was required -- the lever is per function, not per callee.
 *
 * BLOCKER: the argument-setup interleave, at TWO __MapActor_Emote sites.
 *      rom   mov r1,#0x81 / mov r0,#0x19 / lsl r1,#1 / mov r2,#0x1e
 *      ours  mov r1,#0x81 / lsl r1,#1    / mov r0,#0x19 / mov r2,#0x1e
 * Both sites are the same single adjacent swap, and both follow conditional
 * branches, so the lever's precondition looks satisfied.  It is not enough.
 *
 * TRIED: naming the other two arguments immediately before each call, which is
 * the form that recovered OvlFunc_932_200a9dc -- no change, 6.  Hoisting the
 * first site's pair above the preceding `if`, into a genuinely dominating block
 * -- WORSE, 146 lines and 133 differing, because the values then have to survive
 * two __GetFlag calls and gcc spends callee-saved registers on them.
 * CSE_CFLAGS changes nothing here, unlike the sibling: the flag id 0x313 is read
 * and set in DIFFERENT branches, so gcc never commons it in the first place.
 *
 * That last point is the useful one.  Sibling functions in the same overlay, the
 * same flag-reuse shape, and only one of them needs CSE_CFLAGS -- because what
 * matters is whether the two uses share a basic block, not whether the id
 * repeats.  pool.py's flag2 column cannot see that distinction.
 */
