/* Cutscene layer: park a field actor where it stands.
 *
 * Whole-file conversion of
 * asm/rom_8a000/rom_91584_c_c_a_c_a_c_c_c_c_a_c_a.s -- one function, so the
 * ROM layout is preserved without splitting the translation unit.
 */
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void _Actor_Stop(Actor *actor);

/* Sets bit 0 of +0x5A so the actor turns to face its heading, then stops it.
 * Silently does nothing for an empty slot.
 */
void MapActor_SetIdle(s32 slot)
{
    Actor *actor = GetFieldActor(slot);

    if (actor != NULL) {
        actor->walkFlags |= 1;
        _Actor_Stop(actor);
    }
}
