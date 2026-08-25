/* Cluster OvlFunc_946_2009b14..OvlFunc_946_2009b14 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * Slotted between ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_a.o and the rest
 * of the overlay.
 *
 * One of the "position triple on the stack" family: read slot 0`s x/y/z,
 * offset ONE coordinate, and pass the three by address. Same shape as
 * OvlFunc_964_2009348, which batch 46 unparked.
 *
 * The offset is written as an ADDITION of a constant, never a subtraction --
 * the ROM loads the value and adds, including when it is negative.
 * Near-twin of OvlFunc_946_2009b68 next door -- same two-argument call and the
 * same value-returning epilogue -- offsetting Z instead of X.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);
extern int OvlFunc_946_2009a44(struct Actor3 *a, int *pos);

int OvlFunc_946_2009b14(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x;
    pos[1] = actor->y;
    pos[2] = actor->z + 0xffe00000;
    return OvlFunc_946_2009a44(actor, pos);
}
