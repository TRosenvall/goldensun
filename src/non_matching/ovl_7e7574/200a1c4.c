/* OvlFunc_959_200a1c4 (0x0200a1c4) -- NON-MATCHING.
 * Blocker class: DUPLICATE-CONSTANT CSE, straight-line variant.
 * Second instance this batch; see src/non_matching/ovl_7d30e0/200938c.c for
 * the pair that established the boundary.
 *
 * 60 lines against the ROM's 60, 22 differing, and the whole residue is one
 * hoist. The function calls __MapActor_Emote twice with the same second
 * argument, 0x80 << 1. The ROM rebuilds it at each site; gcc builds it once
 * into r5 and copies:
 *
 *     rom    mov r1, #0x80 / lsl r1, #1   ... mov r1, #0x80 / lsl r1, #1
 *     ours   mov r5, #0x80 / lsl r5, #1   ... mov r1, r5 ... mov r1, r5
 *
 * Note the instruction COUNT is identical -- four either way -- so this does
 * not show up as a length difference, only as a register-role divergence that
 * then displaces r5's real occupant (the 0x240d message base) later on. A
 * duplicate-constant hoist is not always a push the ROM does not have.
 *
 * MEASURED, all 60 lines and 22 differing, byte-identical to each other:
 *   -fno-rerun-cse-after-loop
 *   -fno-gcse
 *   -fno-cse-follow-jumps
 *   separate named locals per use site (`e1 = 0x80 << 1; e2 = 0x80 << 1;`)
 *
 * The last is the recorded remedy for repeated constants and it is inert here
 * for the reason the 200938c park established: that lever is the
 * dominating-block mechanism under another name, and it needs a branch between
 * the assignments and the uses. This function is straight-line from
 * __CutsceneStart to __SetFlag -- there is no boundary anywhere in it.
 *
 * WHY THE EXEMPLAR DID NOT HIT THIS, which is the useful contrast. The solved
 * OvlFunc_959_200a134 in the same overlay has the identical two-Emote shape,
 * but its two calls pass 0x80 << 1 and 0x81 << 1 -- DIFFERENT values -- so
 * there is nothing to common and it matched cold. The shapes are the same; the
 * constants are what decides whether the function is reachable.
 *
 * WHAT IS RIGHT AND SHOULD BE KEPT: everything else. The message base as a
 * named int incremented in place (`base = 0x240d; ... base += 1;`) gives the
 * ROM's `ldr r5, =0x240d` and `add r5, #1`; the exemplar's spelling took the
 * base from a symbol address, and a plain pooled constant is correct here.
 *
 * NEXT: nothing source-level. Two specimens of this class now sit in the tree
 * with the same measurement table.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_809228c(int a, int b, int c);
extern void __MapActor_SetBehavior(int slot, int b);
extern void __MapActor_SetAnim(int slot, int a);
extern void __Func_809280c(int a, int b, int c);
extern void __PlaySound(int id);
extern void __MapActor_Emote(int slot, int a, int b);
extern void __MessageID(int id);
extern void __ActorMessage(int actor, int b);
extern void __MapTransitionOut(void);
extern void __CutsceneWait(int n);
extern void __Func_8091e9c(int n);
extern void __SetFlag(int id);

void OvlFunc_959_200a1c4(void)
{
    int base;

    __CutsceneStart();
    __Func_809228c(0, 0, 0);
    __MapActor_SetBehavior(0, 1);
    __MapActor_SetAnim(0, 1);
    __PlaySound(0x71);
    __MapActor_Emote(0x15, 0x80 << 1, 0);
    __MapActor_Emote(0xd, 0x80 << 1, 0x3c);
    __Func_809280c(0x15, 0, 0);
    __Func_809280c(0xd, 0, 0);
    base = 0x240d;
    __MessageID(base);
    __ActorMessage(0xd, 0);
    __MapActor_Emote(0, 0x81 << 1, 0x1e);
    base += 1;
    __MessageID(base);
    __ActorMessage(0xd, 0);
    __MapTransitionOut();
    __CutsceneWait(0x3c);
    __Func_8091e9c(0x3c);
    __CutsceneEnd();
    __SetFlag(0x225);
}
