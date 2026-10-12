/* OvlFunc_959_200c794 -- NON-MATCHING, 6 differing STREAM encodings of 147.
 *
 * BODY RECOVERED FROM scratch/ (batch 338).  This park was PROSE -- a header,
 * a diagnosis, and no compilable body -- so it had no figure, was NO CLAIM in
 * parkcheck, and was unrankable in every band.  But its own header named a
 * candidate at scratch/Tc794.c, that file still existed, and it
 * compiles.  The work had been done and left in a GITIGNORED directory instead
 * of installed, so nothing in the tree could see it.
 *
 * ref 147 / ours 147 stream encodings; 92/92 16-bit instructions -- IN PHASE, so a true DISTANCE.
 *
 * Verify with:
 *   docker run --rm --security-opt seccomp=unconfined -v "$PWD:/work" -w /work \
 *     goldensun-build python3 tools/objcmp.py src/non_matching/ovl_7e7574/200c794.c \
 *     asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c_c.s --func OvlFunc_959_200c794
 *
 * 70 of the 133 prose parks were in this state; tools/scratchfill.py found them.
 * THE FIGURE IS EVIDENCE; the prose below it is a HYPOTHESIS, and across pass
 * two a park's stated blocker has been wrong roughly 40 times in 42.  The
 * original park text is preserved underneath.
 */
extern int _MSG_244f;
extern int _MSG_2455;

extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __MessageID(int id);
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __CutsceneWait(int n);
extern void __ActorMessage(int slot, int n);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MapActor_DoAnim(int slot, int anim);
extern void __Func_809280c(int a, int b, int c);
extern void __Func_8093500(int a, int b);
extern void __Func_8093530(void);
extern void __Func_80925cc(int a, int b);
extern int __Func_8091c7c(int a, int b);

void OvlFunc_959_200c794(void)
{
    int m;

    __CutsceneStart();
    if (__GetFlag(0x941)) {
        __MessageID(0x2566);
        __ActorMessage(0x12, 0);
        __CutsceneEnd();
        return;
    }
    if (__GetFlag(0x313)) {
        __MessageID(0x2457);
        __Func_8092c40(0x19, 0);
        __CutsceneEnd();
        return;
    }
    __MapActor_Emote(0x19, 0x81 << 1, 0x1e);
    __Func_809280c(0x19, 0, 0);
    m = (int)&_MSG_244f;
    __MessageID(m);
    __ActorMessage(0x19, 0);
    __Func_809280c(0x19, 0x18, 0);
    __Func_8093500(0x18, 1);
    __Func_8093530();
    __CutsceneWait(0x3c);
    __Func_8093500(0, 1);
    __CutsceneWait(0x14);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    __MessageID(m + 1);
    __ActorMessage(0x19, 0);
    __MapActor_Emote(0x19, 0x107, 0x3c);
    __MessageID(m + 2);
    __ActorMessage(0x19, 0);
    __CutsceneWait(0x46);
    __MapActor_Emote(0x19, 0x80 << 1, 0x3c);
    __Func_809280c(0x19, 0, 0);
    __MessageID(m + 3);
    __Func_8092c40(0x19, 0);
    if (__Func_8091c7c(0, 0) == 0) {
        __MessageID(m + 4);
        __Func_8092c40(0x19, 0);
    } else {
        __MessageID(m + 5);
        __Func_8092c40(0x19, 0);
    }
    __CutsceneWait(0x3c);
    __MapActor_Emote(0x19, 0x105, 0x3c);
    m = (int)&_MSG_2455;
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __Func_80925cc(0x19, 1);
    __MessageID(m + 1);
    __Func_8092c40(0x19, 0);
    m += 2;
    __MapActor_DoAnim(0x19, 3);
    __MessageID(m);
    __Func_8092c40(0x19, 0);
    __SetFlag(0x313);
    __CutsceneEnd();
}

#if 0
/* ===== ORIGINAL PROSE PARK, PRESERVED =====
(* OvlFunc_959_200c794 -- 0x0200c794, asm/overlays/rom_7e7574/ovl_9dc_c_c_a_a_c.s
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
 *)

*/
#endif
