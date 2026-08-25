/* Cluster OvlFunc_917_20092b4..OvlFunc_917_20092b4 extracted from goldensun/asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a.s.
 *
 * Preserves the original ROM layout when slotted between
 * asm/overlays/rom_7a4370/ovl_30_c_c_c_c_a_a_a.o and the rest of the overlay in
 * goldensun/overlays/rom_7a4370/overlay.ld.
 *
 * A thirty-frame cycle counter in an overlay-local word: two events at frames 0
 * and 0x14, then increment and wrap.
 *
 * BUILT AT -O1, AND IT IS NOT THE USUAL -O1 CASE. At -O2 gcc CROSS-JUMPS the
 * two `bl __Func_8091254` calls into one shared tail; the ROM keeps them
 * separate. Seven positions out.
 *
 * The Makefile's comment on the other -O1 rules says they verify "only at -O1
 * (equivalently -O2 -fno-schedule-insns2)". THIS ONE IS A COUNTER-EXAMPLE to
 * that equivalence: -fno-schedule-insns2 leaves it at 7, and only real -O1
 * matches. Cross-jumping is a jump-pass decision, not a scheduling one, so the
 * two flags are not interchangeable and the comment overstates.
 *
 * The counter is read ONCE into a local and compared twice -- `t = *p;` then
 * two tests on `t`. Reloading `*p` for each test is what gcc does if you write
 * the comparisons against the memory, and it costs an extra `ldr` even at -O1.
 */
extern int L1dd4 __asm__(".L1dd4");
extern void OvlFunc_917_20098b8(int a);
extern void __Func_8091254(int a);

void OvlFunc_917_20092b4(void)
{
    int *p;
    int t;

    p = &L1dd4;
    t = *p;
    if (t == 0) {
        OvlFunc_917_20098b8(0);
        __Func_8091254(0x14);
    } else if (t == 0x14) {
        OvlFunc_917_20098b8(1);
        __Func_8091254(8);
    }
    t = *p + 1;
    *p = t;
    if (t == 0x1e)
        *p = 0;
}
