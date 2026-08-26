/* Func_80c0df4  --  0x080c0df4, cut from the tail of
 * goldensun/asm/rom_b5000/rom_bffb8_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted after
 * asm/rom_b5000/rom_bffb8_a_c_c.o and before asm/rom_b5000/rom_bffb8_a_c_c_b.o
 * in goldensun/stage1.ld.
 *
 * AimCameraAtCombatant: point the camera at the midpoint of two combatants.
 * `GetBattleActor` returns a pointer TO a pointer, so both lookups are
 * dereferenced twice, and the halving is a signed `/ 2` -- the ROM's
 * `lsr r3, r0, #31 / add r0, r3 / asr r0, #1` is gcc's round-toward-zero
 * sequence and not something to write by hand.
 *
 * THE ONE THING THAT HAD TO CHANGE was where the four coordinates are read.
 * Written as one expression,
 *
 *     Func_80c0cec((b->x + a->x) / 2, 0, (b->z + a->z) / 2, c);
 *
 * gcc loads the x pair, adds and halves it, and only then loads the z pair --
 * 12 of 30 lines differing. The ROM loads ALL FOUR first:
 *
 *     ldr r1, [r5, #8] / ldr r0, [r3, #8] / ldr r4, [r5, #0x10] / ldr r2, [r3, #0x10]
 *     add r0, r1 / add r2, r4
 *
 * Naming the four reads as locals gives exactly that, and the register
 * assignment falls into place with it -- the r5/r6 transposition in the
 * prologue was a consequence of the load order, not a separate problem.
 * Naming only the two SUMS is not enough (10 of 30); it has to be the loads.
 */
struct B {
    unsigned char pad00[8];
    int x;
    unsigned char pad0c[4];
    int z;
};

extern struct B **GetBattleActor(int id);
extern void Func_80c0cec(int x, int y, int z, int w);

void Func_80c0df4(int idA, int idB, int c)
{
    struct B *a;
    struct B *b;
    int ax, bx, az, bz;

    a = *GetBattleActor(idA);
    b = *GetBattleActor(idB);
    ax = a->x;
    bx = b->x;
    az = a->z;
    bz = b->z;
    Func_80c0cec((bx + ax) / 2, 0, (bz + az) / 2, c);
}
