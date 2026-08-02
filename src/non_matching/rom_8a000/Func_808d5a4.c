/* Func_808d5a4 @ 0x0808d5a4 -- asm/rom_8a000/rom_8ba38_c_c.s
 *
 * SYMBOL+OFFSET FOLDING. Every instruction matches except the address:
 *
 *     rom    ldr r2, =0x24a / ldr r3, =gState / add r3, r2
 *     ours   ldr r3, =gState+586
 *
 * gcc-2.96 folds a constant offset into the literal-pool entry; the ROM keeps
 * the symbol and the offset as two pool words and adds them at runtime. Two
 * instructions shorter, and no formulation of the access as an array index, a
 * struct member, or a cast pointer avoids the fold.
 *
 * The ROM's shape is what you get when the offset is not a compile-time
 * constant relative to the symbol -- so gState is probably not the object the
 * original indexed. Revisit when whatever gState actually is has been
 * identified.
 *
 * Worth noting what DOES match: the second call passes the entity id in r1
 * without reloading it, because control only reaches there when the compared
 * halfword equals it. gcc-2.96 makes that substitution on its own from the
 * straightforward C below -- no help needed.
 */
#include "gba/types.h"

extern u8 gState[];
extern void *FindMapActorEvent(s32 kind, s32 entityId);

/* Fetches the kind 0 interaction record for the entity. When the entity is
 * also the active conversation target, a kind 7 record wins if one exists --
 * which is how a repeat conversation is told from a first one.
 */
void *Func_808d5a4(s32 entityId)
{
    void *record = FindMapActorEvent(0, entityId);

    if (*(s16 *)(gState + 0x24a) == entityId) {
        void *active = FindMapActorEvent(7, entityId);

        if (active != NULL)
            return active;
    }
    return record;
}
