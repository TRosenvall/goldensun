/* Cluster OvlFunc_883_2009244..OvlFunc_883_2009244 extracted from goldensun/asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_780898/ovl_30_c_c_c_a_a_a_c_c_c_a_c_a.o and the rest of the
 * overlay in goldensun/overlays/rom_780898/overlay.ld.
 *
 * A cutscene: one map edit, one local four-argument call, one flag set. Its
 * near-twin two .o slots later undoes the same three things in the opposite
 * order -- see ovl_30_c_c_c_a_a_a_c_c_c_a_c_c_b.c.
 *
 * TWO LEVERS, pulling in opposite directions, which is why this pair is worth
 * reading together:
 *
 *   - `__Func_8010704` is DECLARED and its two stack values are named `m` and
 *     `n`, assigned in the order the ROM stores them and immediately before the
 *     call -- the stack-arg-pair lever. The assignments sit AFTER
 *     __CutsceneStart, because that is where the ROM builds them; moved above
 *     it they are hoisted and the prologue differs.
 *   - `OvlFunc_883_200b2b0` is deliberately NOT declared. The ROM fills its r0
 *     LAST (`mov r1 / mov r2 / mov r3 / mov r0`), and withholding the prototype
 *     is what produces that order.
 *
 * So one call in this function needs a declaration and the next one needs the
 * absence of one. The lever is not "declare the callees"; it is "declare the
 * ones whose r0 comes first".
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void __SetFlag(int id);

void OvlFunc_883_2009244(void)
{
    int m;
    int n;

    __CutsceneStart();
    m = 0x14;
    n = 0x32;
    __Func_8010704(0x31, 0x35, 8, 4, m, n);
    OvlFunc_883_200b2b0(0, 0xa, 0xb, 1);
    __SetFlag(0x81 << 2);
    __CutsceneEnd();
}
