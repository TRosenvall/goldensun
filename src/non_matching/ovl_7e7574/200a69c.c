/* OvlFunc_959_200a69c (0x0200a69c) -- NON-MATCHING.
 *
 * NON-MATCHING, 29 of 48 encodings PIN-FREE  (RE-MEASURED, batch 333 brief B).
 *   Thirty-three reference instructions against thirty-four, so the pin-free
 *   stream is one instruction long and this figure measures that offset.
 *   A REGISTER-PINNED BODY OF THIS FUNCTION IS AN EXACT MATCH -- objcmp reads
 *   `OK ... 124 bytes, 48 encodings and 13 relocations identical`.  It is
 *   RECORDED BELOW AND DELIBERATELY NOT SHIPPED, under the batch-319 pin
 *   policy: park at the pin-free figure, leave the pinned form for pass 3.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py \
 *     src/non_matching/ovl_7e7574/200a69c.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_a_c_c.s --func OvlFunc_959_200a69c
 *
 * This recipe was ADDED by the batch-319 backfill: the park had none, so
 * parkcheck.py could not report its figure and nothing had ever checked it.
 * Blocker class: duplicate-constant CSE. THIRD specimen this batch.
 *
 * A pure cutscene script -- thirteen calls, no branches, no loops. 47 lines
 * against the ROM's 46, and the single extra line is `push {r5, r6, r14}`
 * where the ROM pushes only lr.
 *
 * `__MapActor_SetSpeed` is called twice with the same two speeds,
 * `0x80 << 9` and `0x80 << 8`. The ROM rebuilds both with mov/lsl at each
 * site. gcc CSEs them, and because the values must survive the intervening
 * calls it puts them in r5 and r6 -- which the function otherwise does not
 * need, hence the wider push and pop.
 *
 * MEASURED (rom 46 lines):
 *   baseline                     47, 29
 *   -fno-gcse                    47, 29
 *   -fno-rerun-cse-after-loop    47, 29
 *   -fno-strength-reduce         47, 29
 *
 * THE THREE SPECIMENS THIS BATCH AGREE, and together they say something the
 * individual entries did not:
 *
 *   OvlFunc_970_20092ac  one constant twice across three calls  -> r8, +5 lines
 *   Func_801965c         a zero twice through halfword stores   -> +1 line
 *   this                 two constants twice across one call    -> r5/r6, +1
 *
 * -fno-gcse is inert on ALL THREE. So the pass responsible is cse.c's local
 * constant CSE, not the global one, and there is no flag that reaches it. That
 * is worth knowing because -fno-gcse is the obvious thing to reach for and it
 * has now cost four runs across three functions to rule out.
 *
 * The class also has a PREDICTABLE VICTIM. A cutscene script -- straight-line
 * calls with repeated coordinate or speed constants -- will hit this every
 * time, because the same literal appears at two call sites with a call between
 * them, which is exactly the shape that forces a callee-saved register.
 * pickable.py already rejects on a repeated expensive constant; the point here
 * is that a function can look trivial (no branches, no loops, thirteen calls in
 * a row) and still be unreachable.
 *
 * WHAT IS RIGHT: every call, every argument, and every constant BUILD --
 * `0x80 << 9`, `0x80 << 8`, `0xec << 1` and the pooled `ldr r0, =0x247c` all
 * come out exactly as the ROM has them. Only the sharing is wrong.
 *
 * NEXT: nothing. The class has no known source-level lever.
  *
 * BATCH 204 -- 29 DIFFERING DOWN TO 18, STILL ONE LINE LONG. Pinning r0-r2 at
 * both __MapActor_SetSpeed sites and assigning them in the ROM's order forces
 * the rebuild of `0x80 << 9` and `0x80 << 8` that gcc was CSEing across the
 * intervening call. That is the batch-193 rematerialisation lever and it
 * removes eleven differing lines.
 *
 * IT DOES NOT REMOVE THE EXTRA LINE. The function is still 47 against the ROM's
 * 46, and the surplus is still the prologue -- `push {r5, r6, lr}` where the
 * ROM pushes only lr. So gcc is holding something ELSE in a callee-saved
 * register besides the two speeds, and the CSE of those two was not the whole
 * cause. The remaining eighteen should be read against that: find what is still
 * live across a call before spending screens on the fills.
 *
 * The candidate below is the 18-differing pinned form, not the 29 the text
 * above was written against.
 *
 * ========================================================================
 * BATCH 333 BRIEF B -- THE PINNED FORM IS AN EXACT MATCH, AND THE PARK'S
 * BATCH-204 CONCLUSION WAS WRONG ABOUT WHAT WAS STILL LIVE
 * ========================================================================
 *
 * DIRECTION, STATED BACK: this park is OURS LONGER.  Note that the usual
 * phrasing of that direction -- "gcc failed an optimisation the original build
 * made" -- is BACKWARDS here.  gcc PERFORMED an optimisation the original
 * build did not: it common-subexpression-eliminated three repeated constants
 * into callee-saved registers.  The body's constant-materialisation count is
 * IDENTICAL on the two sides (twelve instructions each); the entire length
 * difference is the extra `pop` the callee-saved registers force.
 *
 * REFUTED.  The batch-204 note above reads "gcc is holding something ELSE in a
 * callee-saved register besides the two speeds, and the CSE of those two was
 * not the whole cause."  It was the whole cause.  The something else is the
 * THIRD repeated constant, the one at the two script calls that batch 204 did
 * not pin.  Pin all FOUR repeated-constant sites and the function matches
 * exactly.  The recipe, so it is not lost with the scratch directory:
 *
 *     #define PIN3 register int q0 __asm__("r0"); \
 *                  register int q1 __asm__("r1"); \
 *                  register int q2 __asm__("r2")
 *     { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xb; q1 <<= 9; q2 <<= 8; ...speed... }
 *     { PIN3; q1 = 0x80; q2 = 0x80; q0 = 0xf; q1 <<= 9; q2 <<= 8; ...speed... }
 *     { PIN3; q1 = 0xec; q0 = 0xb; q1 <<= 1; q2 = 0xb4; ...script... }
 *     { PIN3; q1 = 0xec; q2 = 0xb4; q0 = 0xf; q1 <<= 1; ...script... }
 *
 * Every other statement is the literal-argument form already below.  Note the
 * assignment ORDER differs between the two pairs; that is not decoration, it
 * is what the reference's scheduling needs.
 *
 * WHY THE PIN-FREE FORM CANNOT AVOID THE CALLEE-SAVED REGISTER, read out of
 * the vendored compiler rather than recalled:
 *   - calls.c:841-857, `precompute_register_parameters`: an argument that is
 *     not already a REG and whose `rtx_cost (value, SET)` exceeds two is forced
 *     into a FRESH PSEUDO by `copy_to_mode_reg` whenever
 *     `SMALL_REGISTER_CLASSES && *reg_parm_seen` -- both true of every Thumb
 *     call that passes anything in a register.
 *   - arm.c:2075-2082, `arm_rtx_costs`, TARGET_THUMB, CONST_INT with outer
 *     SET: under two hundred and fifty-six costs ZERO; `thumb_shiftable_const`
 *     costs COSTS_N_INSNS(2); anything else COSTS_N_INSNS(3).  rtl.h defines
 *     COSTS_N_INSNS(N) as N*4-2, so those are zero, six and ten.
 *     >> A CONSTANT ARGUMENT BELOW 256 NEVER GETS A PSEUDO; ONE AT OR ABOVE
 *     >> 256 ALWAYS DOES.  That is the whole entry condition for this class,
 *     >> and it is a one-line test on any call site.
 *   - cse pass one then unifies the per-site pseudos by value (visible in
 *     `<base>.c.03.cse`: the second site's two sets are simply gone and its
 *     argument copies read the first site's pseudos).
 *   - global.c:987-997: `calls_crossed` non-zero copies `used1` from
 *     `call_used_reg_set`, so the candidates are LO_REGS minus call-used minus
 *     fixed, which here is exactly two registers.  global.c:390-392 keeps an
 *     as-yet-unused callee-saved register out of pass zero only (":1012, we
 *     never allocate a register for the first time in pass 0"), so pass one
 *     hands them out anyway.
 *   - the interwork epilogue then needs a SEPARATE pop ahead of the one that
 *     reloads the return address, because the return address was pushed last
 *     and one ascending pop list cannot bring it back into a low register.
 *     THAT is the extra instruction.
 *
 * SO WHAT WOULD A PIN-FREE BODY NEED?  Enough simultaneously live constant
 * pseudos that NONE of them wins one of the two registers, at which point
 * reload substitutes each one's constant equivalent at its use and rebuilds it.
 * The corpus says that is reachable: one hundred and five generated -- hence
 * byte-matching -- functions under asm/ rebuild a shiftable constant at or
 * above two hundred and fifty-six two or more times while pushing only the
 * return address, and forty-four of those have pin-free, non-fakematch
 * sources.  The clearest model is
 * src/overlays/rom_7d95dc/ovl_30_c_c_c_c_a_c_b.c, `OvlFunc_953_200a820`:
 * SEVENTEEN single-use named constant locals assigned at the top of the
 * function, one per use even where two uses share a value, and the repeated
 * constant is rebuilt six times.
 * MEASURED HERE AND WORSE, which is the point: this function has only THREE
 * repeated constants, two of which fit the two available registers, so naming
 * them at the top makes it WORSE -- thirty-nine instructions, and gcc reaches
 * past the low registers to a high one.  Naming one local per use instead of
 * one per value is byte-identical to that, because cse unifies them before
 * local-alloc ever sees them.
 *   >> PRESSURE, NOT SPELLING, IS WHAT FORCES THE REMATERIALISATION.  A park in
 *   >> this class with FEW repeated constants is harder than one with many.
 *   >> That is the inverse of the usual intuition and it is why the park's
 *   >> "PREDICTABLE VICTIM" paragraph is right about the shape and wrong about
 *   >> the difficulty ordering.
 *
 * CORRECTION TO THE LAST LINE OF THE PREVIOUS PARK: it said the candidate
 * below was the pinned form.  It was not -- the body below is and always was
 * pin-free, and it measures twenty-nine.  The pinned form is the one recorded
 * above in this section.
 *
 * PIECE-MATES: this reference piece holds FOUR functions and all four are
 * parked, so nothing installs here from this function alone.  The next one in
 * the piece opens with the same two-speed script call as this one.
 *
 * NEXT: pass 3, depinning.  The pinned form is exact, so the whole question is
 * register pressure in a four-pseudo window, and `reports/pass3-depin.md` is
 * where it belongs.
*/
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __CutsceneWait(int n);
extern int __MessageID(int id);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_SetSpeed(int slot, int x, int y);
extern void __Func_809218c(int a, int b, int c);
extern void __SetCameraTarget(int slot, int a);
extern void __MapActor_WaitMovement(int slot);
extern void __MapActor_SetAnim(int slot, int anim);

void OvlFunc_959_200a69c(void)
{
    __Func_8093500(0xb, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __MessageID(0x247c);
    __ActorMessage(0xd, 0);
    __MapActor_SetSpeed(0xb, 0x80 << 9, 0x80 << 8);
    __MapActor_SetSpeed(0xf, 0x80 << 9, 0x80 << 8);
    __Func_809218c(0xb, 0xec << 1, 0xb4);
    __Func_809218c(0xf, 0xec << 1, 0xb4);
    __SetCameraTarget(0xb, 1);
    __MapActor_WaitMovement(0xb);
    __MapActor_SetAnim(0xb, 4);
    __CutsceneWait(0x1e);
}

/* The pinned form that matches exactly is written out in the batch-333 section
 * of the header above.  The path once cited here pointed into a scratch
 * directory, which is gitignored and no longer exists -- which is why the
 * recipe now lives in the header instead of behind a reference to it. */
