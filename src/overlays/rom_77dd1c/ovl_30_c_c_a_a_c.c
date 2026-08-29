/* Cluster OvlFunc_882_2008134..OvlFunc_882_2008134 extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_a_a_c.s.
 *
 * The .s held ONLY this function and no data, so no split was needed.
 *
 * The immediate neighbour of the exemplar: it CLEARS the flag its sibling one
 * .o earlier sets, and re-runs the same map edit one column over.
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

void OvlFunc_882_2008134(void)
{
    int m;
    int n;

    __ClearFlag(0x84 << 2);
    m = 0xa;
    n = 0x54;
    __Func_8010704(0x28, 0x59, 7, 4, m, n);
}
