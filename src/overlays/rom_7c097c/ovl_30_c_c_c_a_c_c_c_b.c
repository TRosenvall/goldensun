/* Cluster OvlFunc_936_2009ea4..OvlFunc_936_2009ea4 extracted from goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_7c097c/overlay.ld, so the ROM layout does not move.
 *
 * Points an actor at a turn target, gives it a randomised tick offset, and
 * installs a script.
 *
 * `(r * 5) >> 12` is what the ROM's `lsl r3, r0, #2 / add r3, r0 / lsr r3, #12`
 * is: multiply by five, then a LOGICAL right shift. The shift being logical is
 * why the intermediate is unsigned here -- an int would emit `asr` and change
 * the result for a negative return from __Random.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern int __Random(void);
extern void __Actor_SetScript(Actor *a, void *script);
extern unsigned char gScript_936__0200beac[];

void OvlFunc_936_2009ea4(int slot)
{
    Actor *a;
    unsigned int r;

    a = __MapActor_GetActor(slot);
    a->goalFacing = slot;
    r = __Random();
    a->tickSlow = (r * 5) >> 12;
    __Actor_SetScript(a, gScript_936__0200beac);
}
