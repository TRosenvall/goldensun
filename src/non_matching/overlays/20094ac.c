/* OvlFunc_964_20094ac -- 0x020094ac, asm/overlays/rom_7ed0a0/ovl_30_a_c_c_a_a.s
 *
 * 36 of 36 lines, FOUR differing.  Candidate at scratch/L94ac.c.
 *
 * SOLVED, and it is a new lever worth having:
 *   - `a[0x62] = 0;` makes gcc COPY the pointer before adding
 *     (`mov r3, r0 / add r3, #0x62`) where the ROM consumes it
 *     (`add r0, #0x62`).  Writing the mutation explicitly -- `a += 0x62;
 *     *a = 0;` -- consumes it.  28 differing -> 7 on that change alone.
 *     Offsets above 31 cannot use the immediate form, so this shape is common.
 *   - Naming the mask as its own local (`m = 0xf7; v = *p & m;`) took 7 -> 4.
 *
 * BLOCKER: the loaded byte and the mask constant occupy each other's registers.
 *      rom   ldrb r2, [r0] / mov r3, #0xf7 / and r3, r2
 *      ours  ldrb r3, [r0] / mov r2, #0xf7 / and r2, r3
 * Both are "constant as destination"; only the register assignment differs, and
 * it decides the register of the value stored at the join, so it costs four
 * lines.  Declaration order, `v = 0xf7; v &= *p;` versus `v = *p & 0xf7;`, and
 * a separately named mask were all tried; the last is the best at 4.
 */
