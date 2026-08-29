/* Func_ea54 -- SetEntityHook
 *
 * Installs the per-frame update hook an entity runs.  The frame loop
 * (rom_92b8.s) and the script stepper (rom_ca6c.s) both call it as
 * hook(entity) when it is non-NULL, so this is how a caller attaches
 * behaviour to an entity without going through the script VM.
 *
 * rom_78ef88's prop spawner uses it to attach OvlFunc_common0_d4, the physics
 * integrator; rom_8a000's Func_9ad70 saves and restores it around an idle
 * animation.  Passing NULL detaches.
 *
 * STATUS: MATCHING.  Verify with
 *     tools/asmdiff.py Func_ea54 rom_9000/src/f_1_rom_ea54.c \
 *         --rom-offset 0xea54 --rom-size 0xc
 */

#include "types.h"
#include "entity.h"

void Func_ea54(Entity *entity, EntityHook hook)
{
    if (entity == NULL)
        return;

    entity->updateHook = hook;
}
