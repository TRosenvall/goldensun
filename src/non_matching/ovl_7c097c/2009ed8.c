/* OvlFunc_936_2009ed8  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/overlays/rom_7c097c/ovl_30_c_c_c_a_c_c_c_c.s
 * Best screen: 11 instructions in disagreeing regions, of 18 (rom 18, ours 21).
 *
 * BLOCKER CLASS: constant-CSE across calls -- the INVERSE of the usual
 * complaint. Here gcc is doing MORE work to save instructions and ends up
 * three longer.
 *
 * Two consecutive calls take the same pair of pooled constants:
 *
 *      rom   mov r0, #0x14 / ldr r1, =0x19999 / ldr r2, =0xcccc / bl ...
 *            mov r0, #0x15 / ldr r1, =0x19999 / ldr r2, =0xcccc / bl ...
 *
 * The ROM simply reloads both from the literal pool for the second call. gcc
 * hoists them into two CALLEE-SAVED registers up front and then copies them
 * into r1/r2 before each call:
 *
 *      ours  ldr r5, =0x19999 / ldr r6, =0xcccc
 *            ... mov r1, r5 / mov r2, r6 / mov r0, #0x14 / bl ...
 *            ... mov r1, r5 / mov r0, #0x15 / mov r2, r6 / bl ...
 *
 * That also forces r5 and r6 into the prologue push, which the ROM does not
 * have (`push {lr}` only).
 *
 * WHAT WAS TRIED
 *  1. Both constants as plain literals in both calls (kept below).  11 of 18.
 *  2. The stored zero and the task period through named locals as well.
 *     WORSE, 14 of 18.
 *
 * Relevant prior finding: the LoadUIBanner note in an earlier batch established
 * that N identical pool entries in the ROM usually means N DISTINCT SOURCE
 * SYMBOLS, because one symbol collapses the loads. If that holds here, the two
 * speed constants are two different named things that happen to share a value,
 * and naming them separately would stop the hoist. They are unnamed today, so
 * that is a lead for docs/names.md rather than something to guess at.
 */
extern unsigned char L5144[] __asm__(".L5144");
extern void OvlFunc_936_2009f14(void);
extern void __MapActor_SetSpeed(int a, int x, int y);
extern void __StartTask(void (*fn)(void), int n);

void OvlFunc_936_2009ed8(void)
{
    int z;
    int k;

    z = 0;
    *(int *)L5144 = z;
    __MapActor_SetSpeed(0x14, 0x19999, 0xcccc);
    __MapActor_SetSpeed(0x15, 0x19999, 0xcccc);
    k = 0xc8 << 4;
    __StartTask(OvlFunc_936_2009f14, k);
}
