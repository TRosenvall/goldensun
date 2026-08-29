/* Cluster OvlFunc_956_200824c..OvlFunc_956_200824c extracted from goldensun/asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7e0928/ovl_30_a_c_c_a_c_c_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7e0928/overlay.ld.
 *
 * The one member that SETS its flag rather than clearing it. Here 0x3d is both
 * the second argument and the value at [sp,#4]; writing the second argument as
 * `n` and as the literal 0x3d produce IDENTICAL output, because a value small
 * enough for an eight-bit `mov` is rematerialised either way. The sharing only
 * becomes visible -- and only then needs naming -- when the value is expensive
 * enough that gcc would rather keep it.
 *
 * ANOTHER MEMBER of the nine-function family headed by
 * src/overlays/rom_77dd1c/ovl_30_c_c_a_a_b.c: set or clear one save flag, then
 * one six-argument map-rect call whose last two arguments go on the stack.
 * That family was parked as "stack-argument register reuse" and unparked by the
 * stack-arg-pair lever, and all of it applies here unchanged --
 *
 *   `m` is the value stored at [sp] and `n` the one at [sp,#4], assigned in
 *   THAT order, immediately before the call. Written as literals at the call
 *   site gcc builds one, stores it, and reuses the register for the other.
 *
 * These five were found by tools/match_shapes.py rather than by reading
 * candidates, and all five matched on the first screen. See that tool's
 * docstring for why matching against the SOLVED corpus beats ranking the
 * unsolved one.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_956_200824c(void)
{
    int m;
    int n;

    __SetFlag(0xd8 << 2);
    m = 0x31;
    n = 0x3d;
    __Func_8010704(0x2f, 0x3d, 1, 4, m, n);
}
