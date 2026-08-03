/* Overlay 932: branch on the player's height.
 *
 * Whole-file conversion of asm/overlays/rom_7b9cb4/ovl_30_a_c_c_c_a_c.s -- it
 * holds only this function, so no split was needed.
 *
 * The threshold is 0xC0 << 14 (0x300000 in 16.16, i.e. 48 units) and the
 * comparison is SIGNED -- the ROM ends the test with `ble`, so a player below
 * the origin takes the else branch.
 */
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8092b08(int slot, int arg);

void OvlFunc_932_200b428(void)
{
    if (__MapActor_GetActor(0)->pos.y > (0xc0 << 14)) {
        __MapActor_GetActor(0xb)->flags |= 2;
        __Func_8092b08(0xc, 3);
    } else {
        __Func_8092b08(0xc, 2);
    }
}
