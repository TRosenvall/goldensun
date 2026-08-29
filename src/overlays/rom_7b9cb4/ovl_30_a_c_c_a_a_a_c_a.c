/* Cluster OvlFunc_932_20084cc..OvlFunc_932_20084cc extracted from goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * UNPARKED BY THE STACK-ARG-PAIR LEVER. This was parked at 27 against 27 with
 * twenty-three identical, on the ROM materialising both stack values into
 * separate registers where gcc reuses one.
 *
 * The park recorded named locals as TRIED AND WORSE -- thirteen differing
 * positions against four for plain literals. That attempt put the assignments
 * at the top of the function, before the __MapActor_GetActor call, and gcc
 * hoisted both materialisations above it.
 *
 * Moving the same two assignments to sit IMMEDIATELY BEFORE the call matches.
 * Making values live earlier is not the same as making them live
 * simultaneously, and the park drew the wrong conclusion from the right
 * experiment.
 *
 * See src/overlays/rom_7f2f14/ovl_30_c_a_c_c_c_c_b.c for the full account of
 * the lever and the three steps that found it.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_932_20084cc(void)
{
    Actor *a;
    s32 m;
    s32 n;

    a = __MapActor_GetActor(9);
    m = 0x11;
    n = 0xd;
    __Func_8010704(0x1d, 1, 3, 1, m, n);
    if (a != 0)
        *((u8 *)a + 0x55) = 2;
    __SetFlag(0x201);
}
