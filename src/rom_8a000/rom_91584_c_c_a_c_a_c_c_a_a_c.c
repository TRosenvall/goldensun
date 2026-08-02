/* Cutscene layer: start a looping sound.
 *
 * Whole-file conversion of asm/rom_8a000/rom_91584_c_c_a_c_a_c_c_a_a_c.s --
 * one function, so the ROM layout is preserved without splitting the
 * translation unit.
 */
#include "gba/types.h"

extern u8 *iwram_3001ebc;
extern void _PlaySound(s32 soundId);

/* Records the id at +0xCC8 so the sound can be stopped later, plays 0x12A as
 * the attack, then the sound itself. A soundId of -1 selects the 0x121
 * default -- and note the id is stored BEFORE that substitution, so the stored
 * value is -1 rather than what actually plays.
 *
 * The -1 test narrows to a signed halfword because that is the width the id is
 * stored at.
 */
void Func_8091ff0(s32 soundId)
{
    *(u16 *)(iwram_3001ebc + 0xcc8) = soundId;

    if ((s16)soundId == -1)
        soundId = 0x121;

    _PlaySound(0x12a);
    _PlaySound(soundId);
}
