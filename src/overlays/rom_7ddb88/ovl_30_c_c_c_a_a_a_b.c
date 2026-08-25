/* Cluster OvlFunc_955_2008258..OvlFunc_955_2008258 extracted from goldensun/asm/overlays/rom_7ddb88/ovl_30_c_c_c_a_a_a.s.
 *
 * Slotted between ovl_30_c_c_c_a_a_a_a.o and the rest of the overlay.
 *
 * Stack-arg-pair lever in its SHARED form -- 0x11 is both the second argument
 * and the [sp,#4] value, so it is named once and used twice. Twin of
 * ovl_30_c_c_c_a_a_a_c_b.c, differing in the flag, the actor and one constant.
 */
extern void __SetFlag(int id);
extern void *__MapActor_GetActor(int slot);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_955_2008258(void)
{
    unsigned char *q;
    int m;
    int n;

    __SetFlag(0x331);
    q = (unsigned char *)__MapActor_GetActor(0x14) + 0x55;
    *q = 0;
    m = 0x2c;
    n = 0x11;
    __Func_8010704(0x2e, n, 1, 1, m, n);
}
