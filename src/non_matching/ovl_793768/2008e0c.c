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
 * That predicts the class: a fill-order mismatch on the FIRST call after a join
 * with differing predecessors is not reachable by declarations. Worth checking
 * against before spending seven screens on one, as this cost.
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
