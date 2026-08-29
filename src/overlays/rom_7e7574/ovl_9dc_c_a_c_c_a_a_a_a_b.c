/* OvlFunc_959_2009be4  --  0x02009be4, cut from
 * goldensun/asm/overlays/rom_7e7574/ovl_9dc_c_a_c_c_a_a_a_a.s.
 *
 * Dispatches on two bits of an overlay word to one of four handlers.
 *
 * A PLAIN `switch` REPRODUCES THE ROM'S DECISION TREE -- `== 1`, then `> 1`,
 * then `== 0`, then `== 2` and `== 3` on the far side -- which is gcc's
 * balanced tree over the case values, not a chain. Same rule as batch 78's
 * OvlFunc_882_2008064.
 *
 * THE `default` ARM IS REAL. The ROM has a fifth block at .L1c3a calling the
 * same handler as case 1, reached by the two `b` fallthroughs from the tree.
 * `case 1` and `default` are written as separate arms with the same call in
 * each; gcc does NOT cross-jump them, and writing one arm with a `goto` would
 * be a different function.
 *
 * The dispatch word is reached with gcc's asm-label extension.
 */
extern int L5fa4 __asm__(".L5fa4");
extern void __Func_809280c(int a, int b, int c);
extern void OvlFunc_959_2009c4c(int a);
extern void OvlFunc_959_2009ca4(int a);
extern void OvlFunc_959_2009cf0(int a);
extern void OvlFunc_959_2009d60(int a);

void OvlFunc_959_2009be4(int a)
{
    __Func_809280c(a, 0, 0);
    __Func_809280c(0, a, 0);
    switch (L5fa4 & 3) {
    case 0:
        OvlFunc_959_2009c4c(a);
        break;
    case 1:
        OvlFunc_959_2009ca4(a);
        break;
    case 2:
        OvlFunc_959_2009cf0(a);
        break;
    case 3:
        OvlFunc_959_2009d60(a);
        break;
    default:
        OvlFunc_959_2009ca4(a);
        break;
    }
}
