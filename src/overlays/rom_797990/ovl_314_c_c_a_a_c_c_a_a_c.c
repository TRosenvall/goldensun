/* Overlay 901: hold an actor still while it delivers a line.
 *
 * Whole-file conversion of asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_a_c.s
 * -- it holds only this function, so no split was needed.
 *
 * The slot is resolved twice rather than cached across the inner call, which
 * is what the ROM does; caching it would be shorter.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __MessageID(int id);
extern void OvlFunc_901_20084b4(int slot);

void OvlFunc_901_2008754(void)
{
    __MessageID(0x1cbd);
    __MapActor_GetActor(0xb)->stop = 1;
    OvlFunc_901_20084b4(0xb);
    __MapActor_GetActor(0xb)->stop = 0;
}
