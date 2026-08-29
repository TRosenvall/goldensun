/* OvlFunc_951_2008d70  --  0x02008d70
 *
 * Cut out of goldensun/asm/overlays/rom_7d6418/ovl_30_c_c_c_a_c.s.
 *
 * Picks a line of dialogue for one of six villagers, rotating through three
 * choices each so the same one is not repeated: the per-villager cursor lives
 * in gState at +0x134, is advanced by a random amount plus four modulo three,
 * written back, and used to index a table of message ids. Villager 5 is a
 * wildcard whose identity is itself rolled.
 *
 * `__modsi3 = _modsi3_RAM;` WAS ADDED TO THIS OVERLAY'S LINKER SCRIPT, which
 * was the last differing instruction. gcc-2.96 emits `__modsi3` for `%` and has
 * no flag to rename it; overlay code calls the RAM-resident copy. The alias
 * emits no bytes and is the same fix batch 96 applied for `__divsi3` in
 * overlays/rom_77a7c8/overlay.ld. This is the first `%` in an overlay to need
 * it, so the modulo helper now has the same treatment the division helper has.
 *
 * The two random scalings are `rand * 5 >> 16` and `rand * 2 >> 16` -- gcc
 * strength-reduces both (`lsl #2 / add` and `lsl #1`), so the source multiplies
 * and the assembly shifts. __Random returns UNSIGNED, which is what makes the
 * `>> 16` a `lsr`; the signed form would be an `asr` and would not match.
 *
 * The cursor is a SIGNED byte read out of gState -- `ldrsb r5, [r3, r6]` with
 * the base register first, so it is written as pointer arithmetic on a local
 * base rather than as a subscript. The gState base is a local for the usual
 * reason: written as `gState + idx` gcc would fold the symbol and the offset
 * into one pool entry.
 *
 * Matched on the first screen apart from the alias.
 */
extern unsigned char gState[];
extern int L2018[] __asm__(".L2018");
extern unsigned int __Random(void);

int OvlFunc_951_2008d70(int n)
{
    unsigned char *g;
    int idx;
    int v;

    if (n < 0)
        return 0;
    if (n == 5)
        n = __Random() * 5 >> 16;
    g = gState;
    idx = n + (0x9a << 1);
    v = *(signed char *)(g + idx);
    v = v + (__Random() * 2 >> 16);
    v = v + 4;
    v = v % 3;
    g[idx] = v;
    return L2018[n * 3 + v];
}
