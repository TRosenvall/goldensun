/* Cluster OvlFunc_883_2008d98..OvlFunc_883_2008d98 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_a_c_c_a_c_a.o and the rest of the
 * overlay in goldensun/overlays/rom_780898/overlay.ld.
 *
 * Byte-identical to src/overlays/rom_77dd1c/ovl_30_c_c_a_a_c.c in a different
 * overlay.
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

void OvlFunc_883_2008d98(void)
{
    int m;
    int n;

    __ClearFlag(0x84 << 2);
    m = 0xa;
    n = 0x54;
    __Func_8010704(0x28, 0x59, 7, 4, m, n);
}
