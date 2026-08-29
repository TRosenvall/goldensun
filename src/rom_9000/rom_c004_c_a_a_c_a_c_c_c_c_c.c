/* Behaviour: install the 0x135F0 script, optionally retuning the movement.
 *
 * Whole-file conversion of asm/rom_9000/rom_c004_c_a_a_c_a_c_c_c_c_c.s -- one
 * function, so the ROM layout is preserved without splitting.
 */
#include "actor.h"

/* The script lives in asm/rom_9000/rom_c004_c_c_c.s, which already exports it.
 * `.L135f0` is not a C identifier, so the name is attached with an asm label.
 */
extern u8 Data_80135f0[] __asm__(".L135f0");

extern void Actor_SetScript(Actor *actor, void *script);

/* A zero parameter installs the script and nothing else. Non-zero also
 * overrides the movement tuning for it -- half acceleration, a much higher
 * speed cap -- parks the parameter itself at +0x68 for the script to read, and
 * clears the turn target.
 */
void Camera_SetTarget(Actor *actor, s32 param)
{
    Actor_SetScript(actor, Data_80135f0);

    if (param != 0) {
        actor->accel = 0x8000;
        actor->speed = 0x40000;
        actor->unk_68 = param;
        actor->goalFacing = 0;
    }
}
