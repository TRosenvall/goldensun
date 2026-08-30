/* Cluster OvlFunc_946_200a5f0..OvlFunc_946_200a5f0 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_a.s.
 *
 * Total .text for this TU = 272 bytes.
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

void OvlFunc_946_200a5f0(void)
{
    int w;
    int a;
    int b;
    int c;
    int t;
    int n;

    w = *(int *)(__MapActor_GetActor(0x12) + 8) >> 20;
    a = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    c = *(int *)(__MapActor_GetActor(0xe) + 8) >> 20;
    if (a == 9) {
        if ((unsigned int)(c - 6) <= 2) {
            OvlFunc_946_2009774(0x12, 0, 0x20);
        } else if ((unsigned int)(b - 6) <= 2) {
            OvlFunc_946_2009774(0x12, 0, 0x50);
        } else {
            OvlFunc_946_2009774(0x12, 0, 0x40);
            OvlFunc_946_2009774(0x12, 0, 0x60);
        }
    } else if (a == 0xb) {
        if ((unsigned int)(c - 6) <= 2)
            return;
        if ((unsigned int)(b - 6) <= 2)
            OvlFunc_946_2009774(0x12, 0, 0x30);
        else
            OvlFunc_946_2009774(0x12, 0, 0x80);
    } else if (a == 0xc) {
        if ((unsigned int)(b - 6) <= 2)
            OvlFunc_946_2009774(0x12, 0, 0x20);
        else
            OvlFunc_946_2009774(0x12, 0, 0x70);
    } else if (a == 0xe) {
        if ((unsigned int)(b - 6) <= 2)
            return;
        OvlFunc_946_2009774(0x12, 0, 0x50);
    } else if (a == 0xf) {
        OvlFunc_946_2009774(0x12, 0, 0x40);
    } else if (a == 0x12) {
        OvlFunc_946_2009774(0x12, 0, 0x10);
    } else if (a == 0x13) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    n = w - 1;
    __Func_8010704(n, a, 3, 1, n, t);
    __Func_8010704(0, 0, 3, 1, n, a);
}
