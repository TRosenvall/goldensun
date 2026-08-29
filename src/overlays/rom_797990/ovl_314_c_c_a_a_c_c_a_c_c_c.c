/* Cluster OvlFunc_901_2008af0..OvlFunc_901_2008af0 extracted from goldensun/asm/overlays/rom_797990/ovl_314_c_c_a_a_c_c_a_c_c.s.
 *
 * The .s held ONLY this function and no data after the split above it, so no
 * further split was needed.
 *
 * A CROSS-OVERLAY TWIN of src/overlays/rom_793768/ovl_314_c_c_c_a_c_a_a_c_b.c
 * and its own sibling, found by tools/match_shapes.py: same instruction stream,
 * different overlay, different table label and constants. Overlay 901 has its
 * own copy of the helper (OvlFunc_901_2008a80 here, OvlFunc_898_2008ef4 there),
 * which is why the two did not group as twins under find_twins.py -- that tool
 * compares callee names, this one does not.
 *
 * The declaration of the helper is the lever and has to stay. Without it gcc
 * interleaves the second shift with the third argument and defers the first to
 * the end:
 *
 *     rom    mov r0,#0x9c / mov r1,#0x98 / lsl r0,#1 / lsl r1,#1 / mov r2,#6
 *     ours   mov r0,#0x9c / mov r1,#0x98 / lsl r1,#1 / mov r2,#6 / lsl r0,#1
 *
 * See the rom_793768 file for why naming the shifted values instead does not
 * work: gcc folds `0x9c << 1` at compile time, so the locals never become live.
 */
extern void OvlFunc_901_2008a80(int a, int b, int c);
extern unsigned char L1756[] __asm__(".L1756");
extern void __Func_8010560(void *p, int a, int b);

void OvlFunc_901_2008af0(void)
{
    __PlaySound(0x9e);
    __Func_8010560(L1756, 0x32, 0x12);
    OvlFunc_901_2008a80(0x9c << 1, 0x98 << 1, 6);
}
