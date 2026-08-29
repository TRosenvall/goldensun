/* OvlFunc_959_2009980  --  0x02009980
 *
 * Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a.s.
 *
 * Is the given actor inside a 7-by-5 tile box around the camera target? The
 * sibling of OvlFunc_959_2009918 with a rectangular test instead of a
 * Manhattan one, and a different second actor.
 *
 * THE CONDITION IS SPELLED AS THE FAILING CASE. Written
 * `if (dx <= 7 && dz <= 5) return 1; return 0;` the two exit blocks come out in
 * the opposite order to the ROMs and the stream is one instruction short --
 * 10 differing of 56. Written `if (dx > 7 || dz > 5) return 0; return 1;` it is
 * exact.
 *
 * The tell is which block the ROM lays FIRST: its `bgt` and its `ble` both jump
 * forward past the `mov r0, #0`, so the zero block is the fallthrough of the
 * second test and therefore the `if` body. That is the same reading that
 * decided OvlFunc_917_200952c this round, on a two-clause condition rather than
 * a single test.
 *
 * The four `if (x < 0) x += 0xfffff;` / `asr #20` sequences are gccs own signed
 * division by 0x100000 -- see the sibling.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);
extern struct Actor *__Func_8093554(void);

int OvlFunc_959_2009980(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int ax;
    int az;
    int bx;
    int bz;
    int dx;
    int dz;

    a = __MapActor_GetActor(slot);
    b = __Func_8093554();
    ax = a->x / 0x100000;
    az = a->z / 0x100000;
    bx = b->x / 0x100000;
    bz = b->z / 0x100000;
    dx = ax - bx;
    if (dx < 0)
        dx = -dx;
    dz = az - bz;
    if (dz < 0)
        dz = -dz;
    if (dx > 7 || dz > 5)
        return 0;
    return 1;
}
