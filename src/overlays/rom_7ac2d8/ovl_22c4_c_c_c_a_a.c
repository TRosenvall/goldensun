/* OvlFunc_924_200a648 -- 0x0200a648, 60 bytes, 26 encodings and 1 relocation
 * identical.  Cut out of asm/overlays/rom_7ac2d8/ovl_22c4_c_c_c_a_a.s, the
 * file's only function; tools/datacheck.py prints nothing, export list empty,
 * no split.  ZERO pins, no devices, no per-file flag group.
 *
 * THIS IS THE TWIN OF OvlFunc_924_200adcc, and the two landed together on one
 * edit.  Same routine at 0x5000050/0x500005e/0x5000052 with a bound of six
 * instead of 0x50000c2/0x50000ce/0x50000c4 with a bound of five.  Both parks
 * stood at nine differing of twenty-six with the same first differing index,
 * and the full mechanism is written up once, in
 * src/overlays/rom_7ac2d8/ovl_2dcc_a.c.  READ IT THERE.
 *
 * The two-sentence version, so this file is not useless alone: the residue was
 * two independent register-allocation runs, and each fell to a dimension the
 * park had never varied.
 *   - A one-member aggregate for the halfword temp keeps it HImode, because
 *     promote_mode's switch (explow.c:897-901) covers INTEGER_TYPE and friends
 *     and lets RECORD_TYPE fall through `default:` at :911 unpromoted -- so the
 *     temp carries no subreg copy and no lsl/asr pair, its local-alloc
 *     QTY_CMP_PRI (local-alloc.c:1496-1498) drops below the save pointer's, and
 *     the save pointer takes r3 as the ROM has it.  Worth five of the nine.
 *   - Hoisting `i = 0` to the top of the guarded arm lengthens `i`'s
 *     live_length, the denominator of allocno_compare, so global-alloc ranks
 *     `s` above `i` again.  Worth the other four.  DECLARATION order is inert
 *     here, all six of them; it is the ASSIGNMENT's position that moves.
 *
 * ONE ARTIFACT, FOR PASS 3/4: `struct H` exists only to dodge promote_mode.  A
 * human would have written a plain `unsigned short`, which measures
 * twenty-seven instructions against twenty-six.
 */
extern int iwram_3001e40;

struct H { unsigned short v; };

void OvlFunc_924_200a648(void)
{
    volatile unsigned short *d;
    volatile unsigned short *s;
    unsigned int i;

    if ((iwram_3001e40 & 7) == 0) {
        struct H t;
        i = 0;
        d = (volatile unsigned short *)0x5000050;
        t.v = *d;
        *(volatile unsigned short *)0x500005e = t.v;
        s = (volatile unsigned short *)0x5000052;
        do {
            *d = *s;
            i++;
            s++;
            d++;
        } while (i <= 6);
    }
}
