/* OvlFunc_946_2009b68 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * One of the "position triple on the stack" family: read slot 0`s x/y/z,
 * offset ONE coordinate, and pass the three by address. Same shape as
 * OvlFunc_964_2009348, which batch 46 unparked.
 *
 * The offset is written as an ADDITION of a constant, never a subtraction --
 * the ROM loads the value and adds, including when it is negative.
 *
 * TWO DIFFERENCES FROM THE rom_7ed0a0 MEMBERS, both read off the epilogue and
 * the call:
 *
 *   The callee takes the ACTOR AS WELL as the triple. The ROM never rewrites
 *   r0 before the `bl`, so the pointer returned by __MapActor_GetActor is
 *   still there and is the first argument; the array goes in r1.
 *
 *   THE FUNCTION RETURNS THE CALL`S VALUE. The ROM pops into r1 rather than
 *   r0, which only happens when r0 is still live across the epilogue. As a
 *   void function returning nothing it is 2 of 17, differing in exactly that
 *   pop and the `bx` after it.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);
extern int OvlFunc_946_2009a44(struct Actor3 *a, int *pos);

int OvlFunc_946_2009b68(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x + 0xffe00000;
    pos[1] = actor->y;
    pos[2] = actor->z;
    return OvlFunc_946_2009a44(actor, pos);
}
