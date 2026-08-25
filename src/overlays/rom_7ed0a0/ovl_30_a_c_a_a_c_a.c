/* OvlFunc_964_2009348 extracted from goldensun/asm/overlays/rom_7ed0a0/ovl_30_a_c_a_a_c_a.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * PREVIOUSLY PARKED AS A SCHEDULING BLOCKER, AND THAT WAS WRONG. This C is
 * unchanged from the note in src/non_matching/ovl_7ed0a0/2009348.c; only the
 * compiler flags changed. The rule `ovl_30_a_c_a_a%` was written for the other
 * half of this .s's split and captured this half by name prefix. At -O1 the
 * two streams sit at 6 of 18 with two pairs transposed, which reads exactly
 * like post-reload scheduling. At -O2 it matches.
 *
 * The park's own warning -- "screened at -O2 this reports a clean match and
 * then fails the build" -- was true when written and is what makes this case
 * instructive: the -O2 screen was RIGHT about the code and was overruled by a
 * Makefile rule that did not belong to this TU.
 *
 * The offset is still written as an ADDITION of 0xffe00000 rather than a
 * subtraction of 0x200000; the ROM loads the negative constant and adds.
 */
struct Actor3 {
    unsigned char pad_00[8];
    int x, y, z;
};

extern struct Actor3 *__MapActor_GetActor(int slot);
extern void OvlFunc_964_2008cd0(int *pos);

/* Builds a copy of slot 0's position on the stack with x lowered by 0x20.0000
 * and passes the triple by address.
 */
void OvlFunc_964_2009348(void)
{
    struct Actor3 *actor = __MapActor_GetActor(0);
    int pos[3];

    pos[0] = actor->x + 0xffe00000;
    pos[1] = actor->y;
    pos[2] = actor->z;
    OvlFunc_964_2008cd0(pos);
}
