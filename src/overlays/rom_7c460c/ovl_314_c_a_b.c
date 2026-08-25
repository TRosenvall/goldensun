/* Cluster OvlFunc_939_2008fa0..OvlFunc_939_2008fa0 extracted from goldensun/asm/overlays/rom_7c460c/ovl_314_c_a.s.
 *
 * Slotted between ovl_314_c_a_a.o and the rest of the overlay.
 *
 * Three calls with the same four register arguments and the same SECOND stack
 * slot, differing only in the first stack value (7, 8, 9).
 *
 * WHICH SLOT IS SHARED IS READ OFF THE `str r5` OFFSET -- here [sp, #4], so it
 * is the second local that is held in the callee-saved register across all
 * three calls. See batch 49; three of that batch's members shared [sp] instead.
 *
 * The 0xb appearing as both the second argument and the shared stack value is
 * NOT one value: the ROM rebuilds the argument with a fresh `mov r1, #0xb` each
 * time and only the stack copy lives in r5. Same reading as
 * OvlFunc_935_2008410 in batch 31 -- a repeated constant is shared only if the
 * ROM reuses the register.
 *
 * The third call fills r0 LAST where the first two fill it first. Nothing in
 * the C differs between them; gcc does that on its own at the end of the run.
 */
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);

void OvlFunc_939_2008fa0(void)
{
    int m;
    int n;

    n = 0xb;
    m = 7;
    __Func_8010704(6, 0xb, 1, 1, m, n);
    m = 8;
    __Func_8010704(6, 0xb, 1, 1, m, n);
    m = 9;
    __Func_8010704(6, 0xb, 1, 1, m, n);
    __SetFlag(0x241);
}
