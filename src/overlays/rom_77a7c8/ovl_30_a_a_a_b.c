/* OvlFunc_881_2008030  --  0x02008030
 *
 * Cut out of goldensun/asm/overlays/rom_77a7c8/ovl_30_a_a_a.s.
 *
 * A fatigue check: if the counter in gState has reached nine tenths of the
 * limit stored in the map's script block, then either fire event 0x808 and
 * clear a second word, or top the counter up to the limit -- a coin flip.
 *
 * `__divsi3 = _divsi3_RAM;` WAS ADDED TO THIS OVERLAY'S LINKER SCRIPT. gcc-2.96
 * emits `__divsi3` for `/` and has no flag to rename it, while overlay code
 * calls the RAM-resident copy. The alias emits no bytes and is the established
 * fix -- see overlays/rom_7a5214/overlay.ld, where the reasoning and the call
 * counts are written out. That was the last differing instruction here.
 *
 * THE gState BASE IS A LOCAL, because otherwise gcc folds the offset into the
 * pool as `=gState+568` and the ROM's `mov r2, #0x8e / lsl r2, #2 / add` goes
 * away. Batch 94's rule, and this function needed it even though the base is
 * used once.
 *
 * NAMING THE LIMIT IS NOT NEEDED -- checked. While the gState fold was still in
 * place, hoisting `*r * 9 / 10` into a local looked like it might fix the order
 * the two pointers are computed in. It does not, and once the base is a local
 * the inline form matches on its own. Recorded because the intermediate state
 * made the hoist look load-bearing and it is not.
 *
 * `*r * 9` is `lsl #3 / add` -- gcc's own strength reduction, not something the
 * source spells.
 */
extern char *iwram_3001ebc;
extern unsigned char gState[];
extern unsigned int __Random(void);
extern void __Func_8091f14(int a, int b);

void OvlFunc_881_2008030(void)
{
    char *p;
    int *q;
    int *r;
    unsigned char *g;

    p = iwram_3001ebc;
    g = gState;
    q = (int *)(g + (0x8e << 2));
    r = (int *)(p + (0xd6 << 1));
    if (*q >= *r * 9 / 10) {
        if (__Random() < (0x80 << 8)) {
            __Func_8091f14(0x808, 3);
            *(int *)(p + (0xd4 << 1)) = 0;
        } else {
            *q = *r;
        }
    }
}
