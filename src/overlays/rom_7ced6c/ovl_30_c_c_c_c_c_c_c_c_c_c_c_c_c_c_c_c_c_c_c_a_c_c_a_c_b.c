/* Cluster OvlFunc_946_200a2c8..OvlFunc_946_200a2c8 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_c.s.
 *
 * Total .text for this TU = 252 bytes.
 * Preserves the original ROM layout when slotted before
 * asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_a_c_c.o in goldensun/overlays/rom_7ced6c/overlay.ld.
 *
 * The fifth and largest of the map-dispatcher family in this overlay, and the
 * one that shows the cross-jumping rule most clearly: three of its arms make
 * TWO calls to OvlFunc_946_2009774, the first written out in full and the
 * second sharing the family's common `mov r2, #0` / `bl` tail. Writing every
 * call in every arm and letting gcc merge the identical tails is the whole
 * technique; it matched on the first screen.
 *
 * Two conditions read as short-circuit ORs rather than nested tests --
 * `b == 7 || c == 0x1f` and `a == 0x1e || c == 0x1f` -- which is what the
 * ROM's pairs of branches into a shared label are.
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200a2c8(void)
{
    int a;
    int w;
    int b;
    int c;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0xf) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0xa) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(0xd) + 8) >> 20;
    if (a == 0x18) {
        if (b == 7 || c == 0x1f) {
            OvlFunc_946_2009774(0xf, 0x60, 0);
        } else if (c == 0x22) {
            OvlFunc_946_2009774(0xf, 0x40, 0);
            OvlFunc_946_2009774(0xf, 0x50, 0);
        } else if (c == 0x23) {
            OvlFunc_946_2009774(0xf, 0x50, 0);
            OvlFunc_946_2009774(0xf, 0x50, 0);
        } else {
            OvlFunc_946_2009774(0xf, 0x50, 0);
            OvlFunc_946_2009774(0xf, 0x60, 0);
        }
    } else if (a == 0x1e || c == 0x1f) {
        if (b == 7)
            return;
        if (c == 0x22)
            OvlFunc_946_2009774(0xf, 0x30, 0);
        else if (c == 0x23)
            OvlFunc_946_2009774(0xf, 0x40, 0);
        else
            OvlFunc_946_2009774(0xf, 0x50, 0);
    } else if (a == 0x21) {
        if (c == 0x22)
            return;
        if (c == 0x23)
            OvlFunc_946_2009774(0xf, 0x10, 0);
        else
            OvlFunc_946_2009774(0xf, 0x20, 0);
    } else if (a == 0x22) {
        OvlFunc_946_2009774(0xf, 0x10, 0);
    } else if (a == 0x23) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xf) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
