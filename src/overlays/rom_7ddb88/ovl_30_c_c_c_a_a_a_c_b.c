/* Cluster OvlFunc_955_200828c..OvlFunc_955_200828c extracted from goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a.s.
 *
 * Slotted between ovl_30_c_c_c_a_a_a_c_a.o and the rest of the overlay.
 * Twin of ovl_30_c_c_c_a_a_a_b.c.
 */
extern void __SetFlag(int id);
extern void *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_200828c(void)
{
    unsigned char *q;
    int m;
    int n;

    __SetFlag(0x332);
    q = (unsigned char *)__MapActor_GetActor(0x15) + 0x55;
    *q = 0;
    m = 0x32;
    n = 0x11;
    __Func_8010704(0x2e, n, 1, 1, m, n);
}
