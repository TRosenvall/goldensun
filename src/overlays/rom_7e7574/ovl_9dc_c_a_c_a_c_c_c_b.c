/* OvlFunc_959_200981c  --  0x0200981c
 *
 * Cut out of goldensun/asm//overlays/rom_7e7574/ovl_9dc_c_a_c_a_c_c_c_b.s.
 *
 * Is the actor within six tiles vertically and exactly aligned horizontally?
 *
 * The vertical test is a SIGNED RANGE: `az - bz >= -6 && az - bz <= 6` becomes
 * the ROMs `add r3, #6 / cmp r3, #0xc / bhi`, one unsigned compare. Writing it
 * as two signed compares gives two branches.
 *
 * The horizontal test is written as the ROM spells it -- `ax - 1 < bx &&
 * ax + 1 > bx` -- rather than as the `ax == bx` it is equivalent to.
 *
 * The passing case is the `if` body: the ROMs `mov r0, #1` block is laid before
 * the `mov r0, #0`, so the success path is the fallthrough.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_959_200981c(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int az;
    int ax;
    int bz;
    int bx;

    a = __MapActor_GetActor(slot);
    b = __MapActor_GetActor(0);
    az = a->z / 0x100000;
    ax = a->x / 0x100000;
    bz = b->z / 0x100000;
    bx = b->x / 0x100000;
    if (az - bz >= -6 && az - bz <= 6 && ax - 1 < bx && ax + 1 > bx)
        return 1;
    return 0;
}
