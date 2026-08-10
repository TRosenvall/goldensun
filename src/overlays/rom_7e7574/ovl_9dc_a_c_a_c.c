/* Cluster OvlFunc_959_2008b4c..OvlFunc_959_2008b4c extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_7e7574/overlay.ld is
 * unchanged.
 *
 * Two six-argument setup calls, then an actor is fetched and two of its bytes
 * are written if it exists.
 *
 * THE SECOND POINTER IS DERIVED FROM THE FIRST BY SUBTRACTION:
 *
 *     mov r2, r5 / add r2, #0x55 / mov r3, #0 / strb r3, [r2]
 *     sub r2, #0x32 / mov r3, #2 / strb r3, [r2]
 *
 * 0x55 - 0x32 = 0x23, so the ROM walks the pointer backwards rather than
 * recomputing from the actor. Written as two independent `(u8 *)a + 0x55` and
 * `(u8 *)a + 0x23` expressions gcc emits two adds from the base and the `sub`
 * never appears. `p -= 0x32;` on the live pointer is what produces it.
 *
 * That is the same lesson as src/overlays/rom_780898/ovl_30_c_c_c_c_c_a_c_c_a_c.c
 * from the other direction: match the ROM's arithmetic, not the tidy form. There
 * the tidy version hoisted an address gcc should have left in both arms; here it
 * would recompute one gcc should have derived.
 *
 * 0xf is a named local because it is the fifth argument of BOTH calls and
 * therefore survives the first -- that is a repetition across a call, which is
 * the constant-CSE shape, and a literal would be rebuilt. The 0xf that is also
 * the first call's r0 is left as a literal, because that repetition is inside a
 * single argument block where gcc rebuilds anyway.
 *
 * __Actor_SetSpriteFlags needs no `mov r0` -- the actor is still in r0 from
 * __MapActor_GetActor, and the ROM does not reload it.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008b4c(void)
{
    Actor *a;
    s32 n;
    u8 *p;

    n = 0xf;
    __Func_8010704(0xf, 0x14, 1, 1, n, 0x16);
    __Func_8010704(0x11, 0x17, 1, 3, n, 0x17);
    a = __MapActor_GetActor(0xc);
    if (a != 0) {
        __Actor_SetSpriteFlags(a, 0);
        p = (u8 *)a + 0x55;
        *p = 0;
        p -= 0x32;
        *p = 2;
    }
}
