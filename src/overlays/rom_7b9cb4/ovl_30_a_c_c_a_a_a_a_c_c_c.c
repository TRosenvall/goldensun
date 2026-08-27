/* OvlFunc_932_200840c  --  0x0200840c
 *
 * The whole of goldensun/asm/overlays/rom_7b9cb4/ovl_30_a_c_c_a_a_a_a_c_c_c.s,
 * which held this function and no data.
 *
 * Repaints a doorway and then one tile, choosing the tile from where the actor
 * is standing -- if it is on row 0x19 the actor's own row is used, otherwise a
 * fixed one.
 *
 * The two shared stack values (0x18 and 0x1a) are named locals because the two
 * slots hold different values; the `if` then passes the row in the first slot
 * on one path and the named 0x18 on the other, which is what the ROM's
 * `str r3, [sp]` against `str r7, [sp]` shows.
 *
 * Note that the row is provably 0x19 inside its branch, so passing `y` there
 * rather than the literal is not something the assembly can settle -- batch
 * 95's caution. It is written as `y` because that is what the function means.
 *
 * Matched on the first screen.
 */
struct A {
    unsigned char pad00[8];
    int f8;
    unsigned char pad0c[0x55 - 0xc];
    unsigned char f55;
};

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __Actor_SetSpriteFlags(struct A *a, int n);
extern void __Func_800fe9c(void);
extern void __WaitFrames(int n);

void OvlFunc_932_200840c(void)
{
    struct A *a;
    int e;
    int f;
    int y;

    a = __MapActor_GetActor(0xa);
    if (a != 0) {
        e = 0x18;
        f = 0x1a;
        __Func_8010704(0x18, 0x1b, 2, 1, e, f);
        y = a->f8 >> 20;
        if (y == 0x19)
            __Func_8010704(0, 0, 1, 1, y, f);
        else
            __Func_8010704(0, 0, 1, 1, e, f);
        __Actor_SetSpriteFlags(a, 0);
        a->f55 = 0;
        __Func_800fe9c();
        __WaitFrames(1);
    }
}
