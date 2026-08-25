/* OvlFunc_919_20082a0, the whole of goldensun/asm/overlays/rom_7a67d8/ovl_30_c_a_c_c.s.
 *
 * Total .text for this TU = 44 bytes (= 0x2c). The .s is replaced outright, so
 * no linker-script change was needed.
 *
 * UpdateRasterSplit -- the per-frame task feeding OvlFunc_26c. Reads the camera
 * record at [iwram_3001e70]+0x104: the halfword at +6 gives the horizon, stored
 * as .L610 = 0xc0 - it, and the halfword at +2 becomes the lower scroll .L614.
 * The upper scroll .L616 is that same value minus [iwram_3001e40] >> 2, so the
 * far layer moves at a quarter rate -- the parallax.
 *
 * UNPARKED BY -fno-strict-aliasing. This TU is built with it (see ALIAS_CFLAGS
 * in the Makefile). The park had it at 2 of 22 and called the residue
 * "address-load sinking":
 *
 *      rom   str r3, [r0] / ldr r3, =.L614 / mov r1, #2 / ldrsh r2, [r2, r1]
 *      ours  str r3, [r2] / mov r3, #2     / ldrsh r2, [r1, r3] / ldr r3, =.L614
 *
 * That reading was right about WHAT moved and wrong about why it was fixed.
 * The load is an address materialisation the post-reload scheduler is free to
 * sink past the preceding `str` only because strict aliasing puts the int store
 * to .L610 and the short load from the camera record in different alias sets.
 * Deny it that and the order stands. Nothing in the source changed.
 *
 * THE STATEMENT-ORDER LEVER STILL DID MOST OF THE WORK and the park's account
 * of it is kept: every constant and every destination address is assigned
 * BEFORE the load it feeds --
 *
 *      k = 0x82 << 1;          not   p = iwram_3001e70;
 *      p = iwram_3001e70;            k = 0x82 << 1;
 *
 * Three such swaps took the first screen from 16 of 22 to 2. The flag closed
 * the last two.
 */

extern unsigned char *iwram_3001e70;
extern unsigned int iwram_3001e40;
extern unsigned char L610[] __asm__(".L610");
extern unsigned char L614[] __asm__(".L614");
extern unsigned char L616[] __asm__(".L616");

void OvlFunc_919_20082a0(void)
{
    unsigned char *p;
    int *d0;
    short *d1;
    short *d2;
    unsigned int k;
    unsigned int o;
    int a;
    int b;
    unsigned int v;
    int t;

    k = 0x82 << 1;
    p = iwram_3001e70;
    p += k;
    o = 6;
    a = *(short *)(p + o);
    d0 = (int *)L610;
    t = 0xc0;
    t -= a;
    *d0 = t;
    d1 = (short *)L614;
    o = 2;
    b = *(short *)(p + o);
    *d1 = b;
    v = iwram_3001e40;
    d2 = (short *)L616;
    v >>= 2;
    b -= v;
    *d2 = b;
}
