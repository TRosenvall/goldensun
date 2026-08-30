/* Cluster OvlFunc_946_200a700..OvlFunc_946_200a700 extracted from goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_c_c_a_c_c_a.s.
 *
 * Total .text for this TU = 328 bytes.
 * Preserves the original ROM layout when slotted among its split siblings in
 * goldensun/overlays/rom_7ced6c/overlay.ld.
 *
 * Another member of the map-dispatcher family (batch 152). The call goes in
 * EVERY arm and gcc cross-jumps the identical tails; arms sharing a constant
 * collapse into one block on their own.
 *
 * Two things differ from the earlier siblings: the range checks are `- 9` rather
 * than `- 6`, and the final chain arm is a RANGE test, `(unsigned int)a <= 9`,
 * not an equality test -- the ROM ends the chain with `cmp r7, #9 / bls`.
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_200a700(void)
{
    int w;
    int a;
    int b;
    int d;
    int c;
    int t;
    int n;

    w = *(int *)(__MapActor_GetActor(9) + 8) >> 20;
    a = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    b = *(int *)(__MapActor_GetActor(0x13) + 8) >> 20;
    d = *(int *)(__MapActor_GetActor(0xe) + 8) >> 20;
    c = *(int *)(__MapActor_GetActor(0x10) + 8) >> 20;
    if (a == 0x13) {
        if ((unsigned int)(b - 9) <= 2) {
            OvlFunc_946_2009774(9, 0, -0x10);
        } else if ((unsigned int)(d - 9) <= 2) {
            OvlFunc_946_2009774(9, 0, -0x40);
        } else if ((unsigned int)(c - 9) <= 2) {
            OvlFunc_946_2009774(9, 0, -0x70);
        } else {
            OvlFunc_946_2009774(9, 0, -0x50);
            OvlFunc_946_2009774(9, 0, -0x60);
        }
    } else if (a == 0x12) {
        if ((unsigned int)(b - 9) <= 2)
            return;
        if ((unsigned int)(d - 9) <= 2) {
            OvlFunc_946_2009774(9, 0, -0x30);
        } else if ((unsigned int)(c - 9) <= 2) {
            OvlFunc_946_2009774(9, 0, -0x60);
        } else {
            OvlFunc_946_2009774(9, 0, -0x60);
            OvlFunc_946_2009774(9, 0, -0x40);
        }
    } else if (a == 0xf) {
        if ((unsigned int)(d - 9) <= 2)
            return;
        if ((unsigned int)(c - 9) <= 2)
            OvlFunc_946_2009774(9, 0, -0x30);
        else
            OvlFunc_946_2009774(9, 0, -0x70);
    } else if (a == 0xe) {
        if ((unsigned int)(d - 9) <= 2)
            return;
        if ((unsigned int)(c - 9) <= 2)
            OvlFunc_946_2009774(9, 0, -0x20);
        else
            OvlFunc_946_2009774(9, 0, -0x60);
    } else if (a == 0xc) {
        if ((unsigned int)(c - 9) <= 2)
            return;
        OvlFunc_946_2009774(9, 0, -0x40);
    } else if (a == 0xb) {
        if ((unsigned int)(c - 9) <= 2)
            return;
        OvlFunc_946_2009774(9, 0, -0x30);
    } else if ((unsigned int)a <= 9) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(9) + 0x10) >> 20;
    n = w - 1;
    __Func_8010704(n, a, 3, 1, n, t);
    __Func_8010704(0, 0, 3, 1, n, a);
}
