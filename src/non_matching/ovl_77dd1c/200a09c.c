/* OvlFunc_882_200a09c -- asm/overlays/rom_77dd1c/ovl_30_c_c_c_c_a_a_a_c_c_a.s
 *
 * BLOCKER: REGISTER ALLOCATION (which register holds a computed address)
 *
 * 3 of 49 differing.
 *
 * Identical instructions in identical order; the only difference is which register
 * holds (char*)p + 0x27:
 *   rom  mov r3,r12 / add r3,#0x27 / ldrb r3,[r3]
 *   ours mov r2,r12 / add r2,#0x27 / ldrb r3,[r2]
 * * Got here from 23 differing via the double-read lever (see below), which is the
 * part worth keeping: the ROM has `ldrb r3,[r3] / cmp r3,#0 / ... / mov r1,r3`.
 * `n = p->f27; if (n != 0) {...}` coalesces the copy away; writing the guard on the
 * field and the body on a local -- two textual reads that gcc CSEs -- reproduces it.
 * * MEASURED at 3: unsigned char / cast / signed char field and counter types;
 * ((unsigned char*)p)[0x27]; a named unsigned char *cp; statement-order swaps.
 * -fno-schedule-insns2 12; -O1 worse; -fno-rerun-cse-after-loop 3.
 */
