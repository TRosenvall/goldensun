/* OvlFunc_898_2008e0c  [ovl_793768]
 *
 * Source asm: goldensun/asm/overlays/rom_793768/ovl_314_c_c_c_a_a_c_c.s
 *
 * NOT SPLIT, and it would not need one -- the .s holds only this function and
 * no data.
 *
 * A three-way conversation gated on two save flags, with a per-actor byte set
 * for the duration. Forty-one instructions against forty-one, THIRTY-NINE
 * IDENTICAL, and the two that differ are one `mov r0` in the wrong place:
 *
 *     rom    mov r0, #0x13 / mov r1, #0x0
 *     ours   mov r1, #0x0  / mov r0, #0x13
 *
 * Blocker: ARGUMENT FILL ORDER AT A CONTROL-FLOW JOIN, which is a bound on the
 * declaration lever that was not previously recorded.
 *
 * The misplaced `mov r0` is OUTSIDE any other argument's construction, so by
 * the tell in src/non_matching/ovl_794ac0/2008428.c this is the plain
 * fill-order class that the declaration lever retires -- and it is not
 * arg-interleave. But the lever does not reach it, in either direction and in
 * every combination:
 *
 *   1. __ActorMessage declared            -- 2 differ
 *   2. __ActorMessage implicit            -- 2 differ
 *   3. __MessageID declared as well       -- 2 differ
 *   4. __MessageID declared, __ActorMessage implicit -- 2 differ
 *   5. __MessageID and __CutsceneWait both declared, __ActorMessage implicit
 *                                          -- 2 differ
 *   6. all four declared                  -- 2 differ
 *   7. the actor slot as a named local rather than a repeated literal
 *                                          -- 2 differ
 *
 * All seven are byte-identical to each other.
 *
 * WHY IT PROBABLY RESISTS. The lever works by fixing whether r0 is live across
 * the PRECEDING call. This call sits immediately after a three-way join, and
 * the preceding call is different on each path -- __CutsceneWait on one arm and
 * __MessageID on the other two. There is no single predecessor whose return
 * type could decide the question, so nothing the declarations say can move it.
 *
 * BATCH 108: THE PLACEMENT RULE DOES NOT REACH IT EITHER. The rebuilt-vs-carried
 * rule says a value rematerialised at the call is emitted LAST, so levering the
 * ZERO should put `mov r1, #0` after `mov r0, #0x13` -- which is the ROM's
 * order. It does not: `int z = 0;` at the top of the function, used once at the
 * call, is the same 2 of 41. Levering the slot instead is also 2.
 *
 * That is worth having, because it says the join bound is stronger than the
 * placement rule rather than a special case of it.
 *
 * That predicts the class: a fill-order mismatch on the FIRST call after a join
 * with differing predecessors is not reachable by declarations. Worth checking
 * against before spending seven screens on one, as this cost.
 *
 * BATCH 194: THE PIN DOES NOT REACH IT EITHER, and this is a THIRD measured
 * boundary of that lever rather than another failed spelling. The pin closed
 * five parks in batch 193, three of them in this same blocker family, so it
 * was the obvious next thing. Six structurally distinct forms:
 *
 *   1. r0 and r1 both pinned, uninitialised, assigned in the ROM's order
 *                                          -- 2 differ, unchanged
 *   2. both pinned, INITIALISED at function scope, p0 declared first
 *                                          -- worse: both movs hoist above the
 *                                             call entirely
 *   3. r1 pinned ALONE, to force the zero  -- 2 differ, unchanged
 *   4. r0 pinned ALONE, uninitialised      -- 2 differ, unchanged
 *   5. r0 pinned alone, initialised at its declaration
 *                                          -- 2 differ, unchanged
 *   6. both pinned, the zero passed through `p1 | 0`
 *                                          -- 2 differ, unchanged
 *
 * Four of those six are BYTE-IDENTICAL to the seven declaration spellings
 * above. gcc emits `mov r1, #0 / mov r0, #19` no matter which register is
 * pinned, whether the pin carries an initialiser, or in which order the
 * assignments are written.
 *
 * ONE THING DOES MOVE IT, and it is worth recording precisely because it does
 * not finish the job. A real data dependence orders the pair:
 *
 *     p0 = 0x13;  p1 = p0 - 0x13;          -- gives `mov r0, #19 / mov r1, r0`
 *
 * The ORDER is now the ROM's. The second instruction is not: a dependence is
 * carried in a register, so gcc emits a register copy where the ROM has an
 * immediate. That is not a spelling problem, it is what a dependence IS. The
 * two requirements are in direct conflict -- the only construct found that
 * orders two independent movs is one that stops them being independent.
 *
 * SO THE BOUNDARY IS SHARPER THAN "the pin is inert here". The pin's knobs
 * move the PINNED REGISTER'S OWN mov relative to other instructions. They do
 * not decide which of two independent movs is emitted first, and pinning BOTH
 * does not help -- confirmed here on r0/r1 and independently in
 * src/non_matching/ovl_7ac2d8/200cf44.c, where seven forms including three
 * that pin both bases all leave the post-allocation scheduler to choose. This
 * park is now the second measurement of the same wall from a different angle.
 *
 * The join diagnosis above still stands and is not superseded: this call sits
 * after a three-way join whose predecessors differ, so there is no single
 * preceding call whose return type could decide r0's liveness. What batch 194
 * adds is that the pin does not route around that, and why.
 *
 * Everything else here needed no lever and is worth keeping: the branch
 * polarity falls out of writing the flag-clear case as the `if` body, and the
 * final `*p = 0` is a plain literal -- unlike src/overlays/rom_794ac0/
 * ovl_30_a_c_a_a_c_c_a_a.c, where the ROM caches the zero in a callee-saved
 * register across the calls and a named local is required. Same shape, opposite
 * answer, read off the ROM each time.
 */
#include "gba/types.h"
#include "actor.h"

extern void __MapActor_SetAnim(int slot, int anim);
extern void __ActorMessage(int actor, int b);
extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_898_2008e0c(void)
{
    Actor *a;
    u8 *p;

    a = __MapActor_GetActor(0x13);
    p = (u8 *)a + 0x5b;
    *p = 1;
    __CutsceneStart();
    if (__GetFlag(0x855) == 0) {
        __MessageID(0x1241);
        __MapActor_SetAnim(0x13, 0);
        __CutsceneWait(2);
    } else if (__GetFlag(0x858) != 0) {
        __MessageID(0x13ab);
    } else {
        __MessageID(0x134e);
    }
    __ActorMessage(0x13, 0);
    __CutsceneEnd();
    *p = 0;
}
