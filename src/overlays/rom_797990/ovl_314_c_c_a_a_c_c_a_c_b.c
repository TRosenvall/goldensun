/* Cluster OvlFunc_901_20087d4..OvlFunc_901_20087d4 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c.s.
 *
 * Split out of that .s; the _c part stays as assembly and keeps its slot in
 * goldensun/overlays/rom_797990/overlay.ld, so the ROM layout does not move.
 *
 * The same shape as OvlFunc_901_2008754 in
 * src/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_c.c -- hold slot 0xd still,
 * run a talk routine, release it -- with a different message id and slot.
 *
 * The slot is resolved twice rather than cached across the inner call, which
 * is what the ROM does; caching it would be shorter.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern void OvlFunc_901_20084b4(int slot);

void OvlFunc_901_20087d4(void)
{
    __MessageID(0x1cbf);
    __MapActor_GetActor(0xd)->stop = 1;
    OvlFunc_901_20084b4(0xd);
    __MapActor_GetActor(0xd)->stop = 0;
}
