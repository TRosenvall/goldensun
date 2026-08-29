/* Cluster OvlFunc_968_200aee4..OvlFunc_968_200aee4 extracted from goldensun/asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7f2f14/ovl_30_c_c_a_a_c_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7f2f14/overlay.ld.
 *
 * Two map edits behind a guard. The stack-arg-pair lever twice in one function,
 * with `m` and `n` REASSIGNED between the calls rather than given separate
 * names -- the ROM rebuilds both values for the second call, so one pair of
 * locals written twice is what it wants. Four locals would make gcc keep the
 * first pair alive across the second call.
 *
 * Note the guard: __CutsceneEnd and OvlFunc_968_200ab14 run either way, so they
 * sit outside the `if`. Written inside it the function is several instructions
 * longer and the join label moves -- the same trap as
 * src/overlays/rom_79c738/ovl_30_c_c_a_c_a_a_c.c, which was parked for several
 * rounds with two calls wrongly inside a flag guard.
 */
extern void __CutsceneStart(void);
extern void __CutsceneEnd(void);
extern int OvlFunc_968_2008cc8(void);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);
extern void OvlFunc_968_2008374(void);
extern void OvlFunc_968_200ab14(void);

void OvlFunc_968_200aee4(void)
{
    int m;
    int n;

    __CutsceneStart();
    if (!OvlFunc_968_2008cc8()) {
        m = 5;
        n = 0x30;
        __Func_8010704(0x45, 0x30, 4, 2, m, n);
        m = 9;
        n = 0x25;
        __Func_8010704(0x49, 0x25, 9, 0xd, m, n);
        OvlFunc_968_2008374();
    }
    __CutsceneEnd();
    OvlFunc_968_200ab14();
}
