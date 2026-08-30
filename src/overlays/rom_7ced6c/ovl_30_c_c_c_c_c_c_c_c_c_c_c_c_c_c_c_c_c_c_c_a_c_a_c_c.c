/* Cluster OvlFunc_946_2009f78..OvlFunc_946_2009f78 -- the whole of
 * goldensun/asm/overlays/rom_7ced6c/ovl_30_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_c_a_c_a_c_c.s, which held this
 * function alone with no data, so no split was needed.
 *
 * Total .text for this TU = 140 bytes.
 *
 * The smallest member of the map-dispatcher family: only two actor values and
 * a three-arm chain, but the same rule applies -- the call goes in EVERY arm
 * and gcc cross-jumps the identical tails.
 *
 * Unlike its neighbours this one is confirmed data-free by tools/split_s.py,
 * which reports "holds only OvlFunc_946_2009f78 and no data; convert it
 * directly". Read that tool's whole output before deleting any .s: a sibling
 * in this same overlay carries 21 .incbin blobs behind its single function and
 * deleting it breaks the link.
 */
extern unsigned char *__MapActor_GetActor(unsigned int slot);
extern void __WaitFrames(int n);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_946_2009774(int a, int b, int c);

void OvlFunc_946_2009f78(void)
{
    int a;
    int w;
    int t;
    int n;

    a = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    w = *(int *)(__MapActor_GetActor(0xc) + 0x10) >> 20;
    if (a == 0x24) {
        OvlFunc_946_2009774(0xc, -0x60, 0);
        OvlFunc_946_2009774(0xc, -0x60, 0);
    } else if (a == 0x22) {
        OvlFunc_946_2009774(0xc, -0x60, 0);
        OvlFunc_946_2009774(0xc, -0x40, 0);
    } else if (a == 0x18) {
        return;
    }
    __WaitFrames(2);
    t = *(int *)(__MapActor_GetActor(0xc) + 8) >> 20;
    n = w - 1;
    __Func_8010704(a, n, 1, 3, t, n);
    __Func_8010704(0, 0, 1, 3, a, n);
}
