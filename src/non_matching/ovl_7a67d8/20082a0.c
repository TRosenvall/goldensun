/* OvlFunc_919_20082a0  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7a67d8/ovl_30_c_a_c_c.s
 * Best screen: 2 instructions in disagreeing regions, of 22 (streams same length).
 *
 * BLOCKER CLASS: address-load sinking.
 *
 * The whole remaining difference is where the SECOND destination address is
 * loaded:
 *
 *      rom   str r3, [r0] / ldr r3, =L614 / mov r1, #2 / ldrsh r2, [r2, r1]
 *      ours  str r3, [r2] / mov r3, #2    / ldrsh r2, [r1, r3] / ldr r3, =L614
 *
 * gcc sinks the address load to just before its use. The ROM materialises it
 * first.
 *
 * WHAT WAS TRIED
 *   1. The pointer named and assigned immediately after the previous store,
 *      which is the ROM's position (kept below). 2 of 22.
 *   2. Assigned one statement EARLIER, before that store. WORSE, 12 of 22 --
 *      it perturbs the first block as well.
 *   3. Assigned one statement LATER, after the offset. Byte-identical to (1).
 *
 * THE STATEMENT-ORDER LEVER DID ALMOST ALL THE WORK HERE and is worth reading
 * as a positive result rather than a park. The first screen was 16 of 22. Every
 * constant and every destination address in this function has to be assigned
 * BEFORE the load it feeds:
 *
 *      k = 0x82 << 1;          not   p = iwram_3001e70;
 *      p = iwram_3001e70;            k = 0x82 << 1;
 *
 * Three such swaps took it from 16 to 2. Same lever as batch 61's
 * OvlFunc_908_20084c8; this function is the strongest single demonstration of
 * it so far, and the residue is one instruction gcc schedules for itself.
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
