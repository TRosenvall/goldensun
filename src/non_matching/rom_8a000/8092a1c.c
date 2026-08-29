/* Func_8092a1c -- NON-MATCHING.  Blocker class: BRANCH-OVER-POOL.
 *
 * 41 lines against the ROM's 40, first difference at line 35, 6 differing --
 * every one of them in the tail.  The body is byte-identical.
 *
 * The ROM keeps its literal pool INSIDE the function: the code branches
 * forward over a `.pool_aligned` block and lands on the epilogue after it.
 * old_agbcc only ever emits a pool at `.func_end`, so our epilogue and the
 * ROM's are in the opposite order and no spelling of the C moves them.  This
 * is the 312-function class measured by tools/poolblocked.py -- 14.0% of what
 * remains -- and it is a toolchain ceiling, not a missing lever.
 *
 * The C below is believed CORRECT and is what the body compiles from.  Every
 * offset it touches is documented in include/actor.h, and the ROM's own
 * annotation names them the same way independently: the turn rate at 0x64,
 * the acceleration at 0x34 doubled from the target's, the max speed at 0x30
 * copied outright, and the collision flags at 0x59 cleared.  That agreement
 * is why this is parked as blocked rather than as unsolved -- there is no
 * open question about what it does.
 *
 * Tried: the natural form below.  Nothing else was attempted, because the
 * difference is not in the body and no source-level change reaches pool
 * placement.  Re-attack only if agbcc gains in-function pool emission.
 */
#include "gba/types.h"
#include "actor.h"

extern struct Actor *GetFieldActor(int slot);
extern void _Actor_SetScript(struct Actor *a, void *script);

void Func_8092a1c(int slot, int packed, void *script)
{
    struct Actor *a;
    struct Actor *t;

    a = GetFieldActor(slot);
    t = GetFieldActor(packed & 0xff);
    if (a != 0 && t != 0) {
        a->unk_68 = (u32)t;
        if ((packed & (0x80 << 9)) == 0) {
            a->goalFacing = 0x28;
            a->accel = t->accel << 1;
            a->speed = t->speed;
            a->interactFlags = 0;
        }
        _Actor_SetScript(a, script);
    }
}
