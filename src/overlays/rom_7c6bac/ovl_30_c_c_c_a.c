/* OvlFunc_942_2008af8  --  0x02008af8
 *
 * The whole of goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_c_a.s, which held
 * this function and no data.
 *
 * Repaints three tiles after two villagers move: one fixed strip, then the tile
 * each of them is standing on.
 *
 * ONE OF THE TWO STACK ARGUMENTS IS NAMED AND THE OTHER IS NOT. The sixth is
 * 0xb at all three call sites and the ROM keeps it in r6; the fifth is 5, then
 * one coordinate, then the other, and the ROM builds each fresh. So the C names
 * the sixth and passes the fifth inline -- the discriminator being that the two
 * SLOTS hold different values at each site, not that the value repeats across
 * calls.
 *
 * The two coordinates are read before the first call and shifted afterwards --
 * `mov r8, r3` holds one across it -- which falls out of reading both actors up
 * front. Matched on the first screen.
 */
struct A { unsigned char pad00[8]; int f8; };

extern struct A *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_942_2008b68(int slot);

void OvlFunc_942_2008af8(void)
{
    int y1;
    int y2;
    int f;

    y1 = __MapActor_GetActor(0xe)->f8 >> 20;
    y2 = __MapActor_GetActor(0xf)->f8 >> 20;
    f = 0xb;
    __Func_8010704(5, 0xc, 5, 1, 5, f);
    __Func_8010704(1, 0, 1, 1, y2, f);
    __Func_8010704(1, 0, 1, 1, y1, f);
    OvlFunc_942_2008b68(0xe);
    OvlFunc_942_2008b68(0xf);
}
