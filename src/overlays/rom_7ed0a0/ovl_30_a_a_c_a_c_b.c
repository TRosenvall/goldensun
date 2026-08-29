/* Cluster OvlFunc_964_2008df4..OvlFunc_964_2008df4 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_a_c_a_c.s.
 *
 * Slotted between ovl_30_a_a_c_a_c_a.o and the rest of the overlay.
 *
 * One of the "position triple on the stack" family: read slot 0`s x/y/z,
 * offset ONE coordinate, and pass the three by address. Same shape as
 * OvlFunc_964_2009348, which batch 46 unparked.
 *
 * The offset is written as an ADDITION of a constant, never a subtraction --
 * the ROM loads the value and adds, including when it is negative.
 * Offsets Z by -0x200000. POOLED, not mov/lsl -- so it is spelled 0xffe00000
 * here where its neighbour ovl_30_a_a_c_a_b.c uses a shift.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);

extern void OvlFunc_964_2008cd0(int *pos);

void OvlFunc_964_2008df4(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x;
    pos[1] = actor->y;
    pos[2] = actor->z + 0xffe00000;
    OvlFunc_964_2008cd0(pos);
}
