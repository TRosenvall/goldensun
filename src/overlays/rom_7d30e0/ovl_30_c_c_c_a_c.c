/* Cluster OvlFunc_948_2009df8..OvlFunc_948_2009df8 extracted from goldensun/asm/overlays/rom_7d30e0/ovl_30_c_c_c_a.s.
 *
 * Total .text for this TU = 92 bytes (= 0x5c).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7d30e0/ovl_30_c_c_c_a_b.o and asm/overlays/rom_7d30e0/ovl_30_c_c_c_b.o in
 * goldensun/overlays/rom_7d30e0/overlay.ld.
 */
#include "actor.h"

extern void OvlFunc_948_2009da0(void);
extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_948_2009df8(void)
{
    Actor *p;
    int t;
    int v;

    OvlFunc_948_2009da0();
    p = __MapActor_GetActor(0xb);
    v = p->pos.x / 0x100000;
    t = 0x37;
    __Func_8010704(0x35, 0x37, 1, 1, v, t);
    p = __MapActor_GetActor(0xc);
    v = p->pos.x / 0x100000;
    __Func_8010704(0x35, 0x37, 1, 1, v, t);
}
