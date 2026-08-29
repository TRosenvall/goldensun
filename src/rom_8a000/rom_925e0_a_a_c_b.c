/* Cutscene layer: turn two field actors to face each other.
 *
 * Split out of asm/rom_8a000/rom_925e0_a_a_c.s, which also holds
 * Func_8092878; the remainder is asm/rom_8a000/rom_925e0_a_a_c_c.s and the
 * two are listed in that order in stage1.ld, so the ROM layout is unchanged.
 */
#include "actor.h"

extern Actor *GetFieldActor(s32 slot);
extern void Func_8092878(Actor *a, Actor *b);
extern void CutsceneWait(s32 frames);

/* The slot-addressed wrapper. Func_8092878 does the actual turn and takes
 * entities directly; this resolves both slots first and does nothing unless
 * BOTH are live.
 */
void Func_8092848(s32 slotA, s32 slotB, s32 frames)
{
    Actor *a = GetFieldActor(slotA);
    Actor *b = GetFieldActor(slotB);

    if (a != NULL && b != NULL) {
        Func_8092878(a, b);
        CutsceneWait(frames);
    }
}
