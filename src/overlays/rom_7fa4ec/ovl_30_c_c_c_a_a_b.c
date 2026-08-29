/* Cluster OvlFunc_970_2008168..OvlFunc_970_2008168 extracted from goldensun/asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a.s.
 *
 * Total .text for this TU = 54 bytes (= 0x36).
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_a.o and
 * asm/overlays/rom_7fa4ec/ovl_30_c_c_c_a_a_c.o in
 * goldensun/overlays/rom_7fa4ec/overlay.ld.
 *
 * A LEAF FUNCTION, matched on the first screen. Ticks a counter and, every 0x28
 * ticks, decrements a second value down to a floor of 4 and resets the counter.
 *
 * Nothing clever is needed -- two early returns rather than nested ifs, and the
 * stored zero through a named local so it is a value rather than a literal at
 * the store. This is what most leaf functions look like, and 123 of them went
 * unscreened because tools/pool_candidates.py required at least one call.
 */
extern unsigned char L17f4[] __asm__(".L17f4");
extern unsigned char L17f0[] __asm__(".L17f0");

void OvlFunc_970_2008168(void)
{
    int *a;
    int *b;
    int v;
    int w;
    int z;

    a = (int *)L17f4;
    v = *a;
    v = v + 1;
    *a = v;
    if (v != 0x28)
        return;
    b = (int *)L17f0;
    w = *b;
    if (w <= 4)
        return;
    w = w - 1;
    *b = w;
    z = 0;
    *a = z;
}
