/* Cluster OvlFunc_974_2008f14..OvlFunc_974_2008f14, the whole of
 * goldensun/asm/overlays/rom_7fcd20/ovl_30_c_c_a_c_c_a_a.s -- a single-function TU, so no
 * split was needed; the .c replaces the .s at the same stem and the three
 * overlay scripts that reference the .o are unchanged.
 *
 * Total .text for this TU = 404 bytes (= 0x0194).
 *
 * Stocks the four party members and recomputes their stats.  Sibling of
 * OvlFunc_974_2008bb8 in ovl_30_c_c_a_c_a_c_b.c, which has the same shape.
 *
 * __GiveItemTo IS DELIBERATELY LEFT UNDECLARED -- do not add a prototype.
 * With one, gcc sets r0 before r1 at all forty-five call sites; the ROM sets
 * r1 first.  See the no-prototype lever in docs/elevation.md.
 */
extern void __Func_801776c(int a, int b);
extern void __CalcStats(int who);

void OvlFunc_974_2008f14(void)
{
    __Func_801776c(0xc1c, 1);
    __GiveItemTo(0, 0xb8);
    __GiveItemTo(0, 0xcc);
    __GiveItemTo(0, 0xdc);
    __GiveItemTo(0, 0xdd);
    __GiveItemTo(0, 0xde);
    __GiveItemTo(0, 0xdf);
    __GiveItemTo(0, 0xe0);
    __GiveItemTo(1, 0xe2);
    __GiveItemTo(1, 0xe3);
    __GiveItemTo(1, 0xe6);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe4);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe5);
    __GiveItemTo(1, 0xe8);
    __GiveItemTo(1, 0xe7);
    __GiveItemTo(1, 0xed);
    __GiveItemTo(2, 0xf2);
    __GiveItemTo(2, 0x81 << 1);
    __GiveItemTo(2, 0x10b);
    __GiveItemTo(2, 0x109);
    __GiveItemTo(2, 0xfc);
    __GiveItemTo(3, 0xbd);
    __GiveItemTo(3, 0xc8);
    __GiveItemTo(3, 0xc9);
    __GiveItemTo(3, 0xca);
    __GiveItemTo(3, 0xcb);
    __GiveItemTo(3, 0xcc);
    __GiveItemTo(3, 0xcf);
    __CalcStats(0);
    __CalcStats(1);
    __CalcStats(3);
    __CalcStats(2);
}
