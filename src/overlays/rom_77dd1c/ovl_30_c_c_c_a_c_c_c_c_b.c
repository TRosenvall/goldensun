/* Cluster OvlFunc_882_20092f0..OvlFunc_882_20092f0 extracted from goldensun/asm/overlays/rom_77dd1c/ovl_30_c_c_c_a_c_c_c_c.s.
 *
 * Slotted between ovl_30_c_c_c_a_c_c_c_c_a.o and the rest of the overlay.
 *
 * TWO HELD VALUES AND A FOURTH THAT MUST NOT REUSE EITHER. r5 = 0x2a feeds
 * [sp, #4] for the first three calls and r6 = 2 feeds [sp] for the second and
 * fourth. The fourth call's [sp, #4] is 0x2b, and the ROM builds it in a FRESH
 * register (`mov r3, #0x2b`) rather than overwriting r5.
 *
 * Writing that last value into the same local as the first three is 10 of 39:
 * gcc reuses the held register, and the whole r5/r6 assignment swaps against
 * the ROM's. A separate local for it matches exactly.
 *
 * That is batch 57's "one local per independent operation" -- a local is a
 * statement that one value spans its uses, and this value spans none of them.
 * The tell is in the reference: a fresh `mov r3` rather than a re-store of a
 * held register.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_882_20092f0(void)
{
    int m;
    int n;
    int k;
    int n2;

    m = 3;
    n = 0x2a;
    __Func_8010704(0x1d, 0x16, 1, 1, m, n);
    k = 2;
    __Func_8010704(0x1d, 0x15, 1, 1, k, n);
    m = 4;
    __Func_8010704(0x1d, 0x15, 1, 1, m, n);
    n2 = 0x2b;
    __Func_8010704(0x17, 0x14, 3, 1, k, n2);
}
