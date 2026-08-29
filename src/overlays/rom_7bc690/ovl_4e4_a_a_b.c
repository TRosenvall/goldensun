/* Cluster OvlFunc_933_2008c38..OvlFunc_933_2008c38 extracted from goldensun/asm/overlays/rom_7bc690/ovl_4e4_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7bc690/ovl_4e4_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7bc690/overlay.ld.
 *
 * `__Func_8091f90` TAKES AN AREA ID, which is why 0x5b is written as
 * `(int)(&_AREA_5b)`. The ROM pools it although it would fit in a `mov`, and
 * that is the pool tell. Two earlier files pass `_AREA_51` and `_AREA_4d` to
 * the same callee.
 *
 * Note the contrast three lines later: `__Func_8091eb0(0x35, 5)` has 0x35 in a
 * plain `mov`, so THAT one is a literal. The same number is an area symbol in
 * one call and an ordinary constant in the next, and only the ROM's choice of
 * `ldr` versus `mov` distinguishes them.
 *
 * BUILT AT -O1. At -O2 the two pool loads come out in the wrong order --
 * `ldr r2,=0x22b` before `ldr r3,=gState` where the ROM has them the other way.
 * Both operands are pool loads, so this is NOT the pool-loads-first class,
 * which is about a pool load jumping ahead of a `mov`. It is the post-reload
 * scheduler ordering two loads, and -fno-schedule-insns2 fixes it equally well;
 * -O1 is used to match the rules already in the Makefile.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int _AREA_5b;
extern void __Func_80925cc(int a, int b);
extern void __Func_8091f90(int id, int b);
extern void __Func_8091eb0(int a, int b);

void OvlFunc_933_2008c38(void)
{
    unsigned char *g;

    __Func_80925cc(8, 2);
    __Func_8091f90((int)(&_AREA_5b), 5);
    g = (unsigned char *)&gState;
    g += 0x22b;
    *g = 3;
    __Func_8091eb0(0x35, 5);
}
