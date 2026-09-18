// fakematch
/* Cluster NewActor..NewActor extracted from goldensun/asm/rom_9000/rom_c004_c_a_a_a_a_a_a.s.
 *
 * Total .text for this TU = 40 bytes (= 0x28).
 * Preserves the original ROM layout when slotted between
 * asm/rom_9000/rom_c004_b.o and asm/rom_9000/rom_c004_c_a_a_a_a_a_a_b.o in
 * goldensun/stage1.ld.
 *
 * BUILT WITH SCHED2_CFLAGS -- see the Makefile row. This is the THIRD file in the
 * tree to need -fno-schedule-insns2 and the first since batch 266 concluded that
 * none was warranted.
 *
 * TWO COSTS, both real: one register pin (fakematch row) and one Makefile row.
 * They are independent and BOTH are required -- unpinned under the flag is still
 * 6 differing, and pinned without the flag is 2.
 *
 * WHAT THE FLAG IS FOR. The ROM loads before it materialises the zero:
 *
 *     rom    ldr r3, [r2] / mov r0, #0 / mov r1, #0
 *     ours   mov r0, #0   / ldr r3, [r2]
 *
 * sched2 hoists the zero-cost `mov` above the load, and nothing at the source
 * level reaches it. Measured in batch 271: all twelve statement orders of the
 * four opening assignments (2,2,3,3,15,16,16,16,15,15,2,3 pinned; 7..18
 * unpinned), both loads as `volatile int`, a two-return shape instead of a result
 * variable (16 pinned / 14 unpinned), and a plain `while (*(int *)p != 0)` loop
 * (12 / 18, and 21 lines). The residue is the same two adjacent instructions
 * transposed in every one.
 *
 * THE PIN IS INTERCHANGEABLE, which is worth knowing: `v` in r3 as written, or
 * `p` in r2 instead, give IDENTICAL output -- both exact with the flag, both 2
 * without. One pin, either operand; a second buys nothing.
 *
 * The loop shape itself is the park's and is right: an unsigned counter forbids
 * gcc's loop reversal, and the `goto test` entry reproduces the ROM's
 * `b .Lc0e4` into the middle of the loop.
 */
extern unsigned char *iwram_3001e64;

unsigned char *NewActor(void)
{
    unsigned char *r;
    int i;
    unsigned char *p;
    register int v __asm__("r3");

    p = iwram_3001e64;
    v = *(int *)p;
    r = 0;
    i = 0;
    goto test;
    do {
        i++;
        p += 0x70;
        if (i > 0x3f)
            goto done;
        v = *(int *)p;
test:
        ;
    } while (v != 0);
    r = p;
done:
    return r;
}
