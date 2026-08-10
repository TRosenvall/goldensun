/* Cluster OvlFunc_883_200da94..OvlFunc_883_200da94 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_c_c_a_c_c_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed -- the .o
 * keeps its name and its slot in goldensun/overlays/rom_780898/overlay.ld is
 * unchanged.
 *
 * Picks one of two actors depending on a save flag and sets a byte on it to 3
 * or 1 depending on how far up the player is.
 *
 * Matched on the first screen. One thing is worth recording because it looks
 * like a mistake and is not:
 *
 *     .L5acc:  mov r2, r0 / add r2, #0x23 / mov r3, #1
 *     .L5ad2:  strb r3, [r2]
 *
 * The `+ 0x23` is computed SEPARATELY IN EACH ARM and only the store is shared.
 * Written the tidy way --
 *
 *     u8 *p = (u8 *)b + 0x23;  *p = cond ? 3 : 1;
 *
 * -- gcc hoists the add above the branch and the two arms become a single `mov`
 * each, which is two instructions shorter and does not match. Writing the whole
 * store out in both arms, redundantly, is what produces the ROM's shape: gcc
 * cross-jumps the common tail (`strb`) on its own and leaves the address
 * arithmetic where the source put it.
 *
 * So this is the pre-header load merge from
 * src/non_matching/preheader_load_merge.c running in our FAVOUR -- gcc's
 * tail-merging is exactly what is wanted here, and the job is only to not
 * pre-empt it by hoisting in the source.
 *
 * The comparison `> (0xc8 << 16)` is against a 16.16 coordinate, so it is a
 * world-space threshold of 200 units, and the branch polarity falls out of
 * writing the greater-than case as the `if` body.
 */
#include "gba/types.h"
#include "actor.h"

extern Actor *__MapActor_GetActor(int slot);

void OvlFunc_883_200da94(void)
{
    Actor *a;
    Actor *b;

    a = __MapActor_GetActor(0);
    if (__GetFlag(0x87a))
        b = __MapActor_GetActor(0x15);
    else
        b = __MapActor_GetActor(0x14);
    if (b != 0) {
        if (a->pos.y > (0xc8 << 16))
            *((u8 *)b + 0x23) = 3;
        else
            *((u8 *)b + 0x23) = 1;
    }
}
