/* OvlFunc_964_20093b4 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * One of the "position triple on the stack" family: read slot 0`s x/y/z,
 * offset ONE coordinate, and pass the three by address. Same shape as
 * OvlFunc_964_2009348, which batch 46 unparked.
 *
 * The offset is written as an ADDITION of a constant, never a subtraction --
 * the ROM loads the value and adds, including when it is negative.
 * Offsets x by +0x200000, built as `0x80 << 14` because the ROM materialises
 * it with mov/lsl rather than pooling it.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);

extern void OvlFunc_964_2008cd0(int *pos);

void OvlFunc_964_20093b4(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x + (0x80 << 14);
    pos[1] = actor->y;
    pos[2] = actor->z;
    OvlFunc_964_2008cd0(pos);
}
