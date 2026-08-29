/* OvlFunc_957_2008f10 -- NON-MATCHING.
 * Blocker class: REGISTER-PRESSURE RESIDUE, "dead callee-saved register"
 * shape.  See HANDOFF.md, "Register-pressure residue: a category, not a set of
 * one-offs" -- that section already states the conclusion and this park is an
 * instance of it, not a new finding.
 *
 * 44 lines against the ROM's 43, 18 differing, and EVERY difference is in the
 * prologue and epilogue.  The body is exact.
 *
 *     rom    push {r5, r6, r7, r14} / mov r7,r11 / mov r6,r10 / mov r5,r9
 *            / push {r5, r6, r7} / mov r7,r8 / push {r7}
 *     ours   push {r5, r6, r14} / mov r6,r11 / mov r5,r10 / push {r5, r6}
 *            / mov r6,r9 / mov r5,r8 / push {r5, r6}
 *
 * Both save r8-r11.  The ROM ALSO saves r7 and uses it for one short-lived
 * constant (`mov r7, #0x90 / lsl r7, #16`) between the two position stores;
 * gcc puts that constant in a scratch register and never needs r7, so its
 * prologue is one register shorter and the save/restore sequences differ
 * throughout.  That is the "dead callee-saved register" shape: prologue and
 * epilogue bookkeeping for a value the C has no way to demand.
 *
 * WHAT WAS SOLVED getting from 43 differing to 18, and it is worth keeping:
 * THE TWO CONSTANTS MUST BE LIVE ACROSS THE CALL.  Written inline after
 * __MapActor_GetActor, gcc materialises them late, needs no high registers at
 * all, and the function comes out 34 lines against 43 -- NINE SHORT, because
 * all the r8-r11 save/restore bookkeeping is absent.  Naming them before the
 * call so four values are live across it is what makes gcc reach for r8-r11
 * and brings the length to within one.  The stream LENGTH is the tell here,
 * exactly as docs/elevation.md says for the `sub sp` case: nine missing lines
 * meant a missing register class, not a missing instruction.
 *
 * Tried after that, all 18 or 19 differing:
 *   - the 0x90 << 16 constant in a named local computed between the two stores,
 *     which is literally where the ROM computes it: 19, one WORSE
 *   - a named `int *p = v` so the vec base is a live pointer the way the ROM's
 *     `mov r5, sp` suggests: 19
 *   - both together: 19
 *
 * None of them creates demand for a fifth callee-saved register, which is what
 * the difference actually is.  Per the HANDOFF section, the diagnostic that
 * would settle it is a near-twin that DOES match; none is known for this shape.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *__MapActor_GetActor(int slot);
extern void __vec3_translate(int a, int b, int *v);

void OvlFunc_957_2008f10(int slot, int b, int c)
{
    struct Actor *a;
    int v[3];
    int k1, k2;

    k1 = 0xfc << 17;
    k2 = 0xc0 << 13;
    a = __MapActor_GetActor(slot);
    v[0] = k1;
    v[2] = k2;
    __vec3_translate(b, c, v);
    a->pos.x = v[0];
    a->pos.y = v[2];
    a->pos.z = 0x90 << 16;
}
