/* Cluster OvlFunc_946_200a984..OvlFunc_946_200a984 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_a.s.
 *
 * Total .text for this TU = 276 bytes.
 * Preserves the original ROM layout when slotted among its split siblings in
 * goldensun/overlays/rom_7ced6c/overlay.ld; the five functions of the original
 * chunk are consecutive.
 *
 * Another of the map-dispatcher family (see batch 152). Reads several actor
 * fields as `*(int *)(actor + off) >> 20`, dispatches on one of them through a
 * chain of equality tests, and calls OvlFunc_946_2009774 per arm.
 *
 * THE CALL GOES IN EVERY ARM and gcc cross-jumps the identical tails. Arms
 * that share a constant with another arm collapse into one block on their own;
 * do not try to write that sharing out by hand.
 *
 * This chunk's variant passes the varying value as the THIRD argument with the
 * second fixed at 0, where the batch-152 group varied the second. The two-call
 * arms and the negative constants are otherwise the same shape.
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200a984(void)
{
    int a;
    int w;
    int b;
    int d;
    int c;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0x13) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0x11) + 0x10) >> 20;
    d = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    if (a == 3)
        return;
    if (a == 0xd) {
        if (c == 0xf) {
            OvlFunc_946_2009774(0x13, -0x10, 0);
        } else if (d == 0xf) {
            OvlFunc_946_2009774(0x13, -0x40, 0);
        } else if (b == 0xf) {
            OvlFunc_946_2009774(0x13, -0x70, 0);
        } else {
            OvlFunc_946_2009774(0x13, -0x70, 0);
            OvlFunc_946_2009774(0x13, -0x30, 0);
        }
    } else if (a == 6) {
        if (b == 0xf)
            return;
        OvlFunc_946_2009774(0x13, -0x30, 0);
    } else if (a == 5) {
        OvlFunc_946_2009774(0x13, -0x20, 0);
    } else if (a == 8) {
        if (d == 0xf)
            return;
        if (b == 0xf)
            OvlFunc_946_2009774(0x13, -0x20, 0);
        else
            OvlFunc_946_2009774(0x13, -0x50, 0);
    } else if (a == 9) {
        if (d == 0xf)
            return;
        if (b == 0xf)
            OvlFunc_946_2009774(0x13, -0x30, 0);
        else
            OvlFunc_946_2009774(0x13, -0x60, 0);
    } else if (a == 0xc) {
        if (c == 0xf)
            return;
        if (d == 0xf)
            OvlFunc_946_2009774(0x13, -0x30, 0);
        else if (b == 0xf)
            OvlFunc_946_2009774(0x13, -0x60, 0);
        else
            OvlFunc_946_2009774(0x13, -0x90, 0);
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
