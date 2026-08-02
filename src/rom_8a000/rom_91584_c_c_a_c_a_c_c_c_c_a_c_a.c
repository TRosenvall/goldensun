/* Cutscene layer: park a field actor where it stands.
 *
 * Whole-file conversion of
 * asm/rom_8a000/rom_91584_c_c_a_c_a_c_c_c_c_a_c_a.s -- one function, so the
 * ROM layout is preserved without splitting the translation unit.
 */
#include "entity.h"

extern Entity *GetFieldActor(s32 slot);
extern void _Actor_Stop(Entity *entity);

/* Sets bit 0 of +0x5A so the entity turns to face its heading, then stops it.
 * Silently does nothing for an empty slot.
 */
void MapActor_SetIdle(s32 slot)
{
    Entity *entity = GetFieldActor(slot);

    if (entity != NULL) {
        entity->unk_5a[0] |= 1;
        _Actor_Stop(entity);
    }
}
