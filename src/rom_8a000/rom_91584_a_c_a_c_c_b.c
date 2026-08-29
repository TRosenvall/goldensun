/* Func_8091660  --  0x08091660
 *
 * The first function of goldensun/asm/rom_8a000/rom_91584_a_c_a_c_c.s;
 * CutsceneStart stays as assembly beside it.
 *
 * Resets the field actor the party is following: zeroes two of its motion
 * fields, plants four fixed constants in the others, and picks its idle
 * animation from a byte in gState.
 *
 * The gState base is READ ONCE INTO A LOCAL. The ROM keeps it in r5 across the
 * call to GetFieldActor and indexes it twice -- `add r3, r5, r2` for the actor
 * id at +0x1f4 and again for the byte at +0x1f2. A local `unsigned char *` is
 * what produces that; referring to `gState` directly at both sites reloads the
 * address.
 *
 * The four constants are all `mov` + `lsl` pairs (0x10000, 0x8000, and
 * 0x80000000 twice), and the last one is stored to both +0x38 and +0x40 from
 * one register -- ordinary CSE inside a basic block, nothing to spell.
 *
 * Matched on the first screen.
 */
struct Actor {
    unsigned char pad00[0x24];
    int f24;
    unsigned char pad28[4];
    int f2c;
    int f30;
    int f34;
    int f38;
    unsigned char pad3c[4];
    int f40;
};

extern unsigned char gState[];
extern struct Actor *GetFieldActor(int id);
extern void _Actor_SetAnim(struct Actor *a, int anim);

void Func_8091660(void)
{
    unsigned char *g;
    struct Actor *a;

    g = gState;
    a = GetFieldActor(*(int *)(g + 0x1f4));
    a->f30 = 0x10000;
    a->f34 = 0x8000;
    a->f38 = 0x80000000;
    a->f40 = 0x80000000;
    a->f24 = 0;
    a->f2c = 0;
    if (g[0x1f2] == 1)
        _Actor_SetAnim(a, 0xc);
    else
        _Actor_SetAnim(a, 1);
}
