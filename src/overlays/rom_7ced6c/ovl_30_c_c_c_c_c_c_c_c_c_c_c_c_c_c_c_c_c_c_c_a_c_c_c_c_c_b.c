/* Cluster OvlFunc_946_200ac4c..OvlFunc_946_200ac4c extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_c.s.
 *
 * Total .text for this TU = 192 bytes.
 * Preserves the original ROM layout when slotted in overlay.ld between its
 * split siblings; the four functions of the original chunk are consecutive.
 *
 * One of a family of four near-identical map dispatchers in this overlay. Each
 * reads an actor field, dispatches on it through a chain of equality tests,
 * and calls OvlFunc_946_2009774 with a per-arm constant.
 *
 * THE CALL IS IN EVERY ARM. The ROM's arms set only the differing argument
 * registers and branch to a shared tail holding `mov r2, #0` and the `bl`
 * (and, where the constant is negative, the `neg`). That is gcc CROSS-JUMPING
 * identical call tails, not a single call after a join: hoisting the call out
 * and assigning the argument per arm is nine instructions short, because gcc
 * then materialises the common first argument once instead of per arm.
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200ac4c(void)
{
    int a;
    int w;
    int b;
    int c;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0xe) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0xe) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0x12) + 0x10) >> 20;
    c = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    if (a == 6) {
        if ((unsigned int)(c - 0xc) <= 2)
            OvlFunc_946_2009774(0xe, 0x20, 0);
        else if ((unsigned int)(b - 0xc) <= 2)
            OvlFunc_946_2009774(0xe, 0x40, 0);
        else
            OvlFunc_946_2009774(0xe, 0x70, 0);
    } else if (a == 8) {
        if ((unsigned int)(c - 0xc) <= 2)
            return;
        OvlFunc_946_2009774(0xe, 0x50, 0);
    } else if (a == 9) {
        if ((unsigned int)(c - 0xc) <= 2)
            return;
        OvlFunc_946_2009774(0xe, 0x40, 0);
    } else if (a == 0xc) {
        OvlFunc_946_2009774(0xe, 0x10, 0);
    } else if (a == 0xd) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xe) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
