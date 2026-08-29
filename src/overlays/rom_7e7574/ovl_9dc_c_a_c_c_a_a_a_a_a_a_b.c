/* OvlFunc_959_2009918  --  0x02009918
 *
 * Cut out of goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a_a_a.s.
 *
 * Is the given actor within four tiles of the player, measured one tile north?
 * Returns 1 or 0.
 *
 * `if (x < 0) x += 0xfffff;` FOLLOWED BY `asr #20` IS SIGNED DIVISION. That
 * five-instruction sequence appears four times here and is just
 * `coord / 0x100000` on an `int` -- gcc generates the bias-and-shift itself.
 * Writing the bias by hand would be four extra branches.
 *
 * THE RETURN POLARITY IS READ OFF WHICH VALUE IS PRESET. The ROM does
 * `mov r0, #1 / cmp r3, #4 / ble / mov r0, #0` -- it presets the TRUE value and
 * overwrites on failure, which is `if (sum <= 4) return 1; return 0;`. Written
 * the other way round (`if (sum > 4) return 0; return 1;`) the same three
 * instructions come out with the constants and the condition swapped, and
 * introducing a result variable costs an instruction.
 */
struct Actor {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct Actor *__MapActor_GetActor(int slot);

int OvlFunc_959_2009918(int slot)
{
    struct Actor *a;
    struct Actor *b;
    int az;
    int ax;
    int bz;
    int bx;
    int dx;
    int dz;

    a = __MapActor_GetActor(slot);
    b = __MapActor_GetActor(0);
    az = a->z / 0x100000;
    ax = a->x / 0x100000;
    bz = b->z / 0x100000;
    bx = b->x / 0x100000;
    dx = ax - bx;
    az++;
    if (dx < 0)
        dx = -dx;
    dz = az - bz;
    if (dz < 0)
        dz = -dz;
    if (dx + dz <= 4)
        return 1;
    return 0;
}
