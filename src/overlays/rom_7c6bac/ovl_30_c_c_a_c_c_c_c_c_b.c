/* OvlFunc_942_20088cc  --  0x020088cc
 *
 * Cut out of goldensun/asm/overlays/rom_7c6bac/ovl_30_c_c_a_c_c_c_c_c.s.
 *
 * Three independent arrival fixups, each a nested pair of tests: two on the
 * area id and one on a flag pair.
 *
 * THIS FUNCTION SEPARATES THE TWO MECHANISMS CLEANLY, which is why it is worth
 * reading. It needs BOTH CSE_CFLAGS and the BASIC-BLOCK LEVER, and each one
 * fixes a different set of instructions:
 *
 *     plain literals, default flags               46 differing of 53
 *     plain literals, -fno-rerun-cse-after-loop    3
 *     lever, default flags                        48
 *     lever, -fno-rerun-cse-after-loop             0
 *
 * The flag fixes CONSTANT CSE: 0x8ac is read then written inside one block and
 * gcc holds it in a callee-saved register where the ROM issues `ldr r0, =0x8ac`
 * twice. The lever fixes ARGUMENT SCHEDULING: the two `__MapActor_SetPos`
 * coordinates are assigned inside the `if (__GetFlag(0x911))` block and used in
 * the nested `if`, so gcc rematerialises each as a split `mov`/`lsl` pair with
 * `mov r0, #0xc` in the gap. Neither reaches the other's three instructions.
 *
 * The area is read TWICE, once per test, and the ROM recomputes the offset both
 * times -- so `g = gState;` is cached in a local but the subscript is not.
 */
extern unsigned char gState[];
extern int __GetFlag(int id);
extern void __SetFlag(int id);
extern void __ClearFlag(int id);
extern void __MapActor_SetPos(int slot, int x, int y);
extern void __Func_8092adc(int a, int b, int c);
extern void OvlFunc_942_2008ba0(void);

void OvlFunc_942_20088cc(void)
{
    unsigned char *g;
    int x;
    int y;

    g = gState;
    if (*(short *)(g + (0xe1 << 1)) == 1) {
        if (__GetFlag(0x8ac) == 0) {
            __SetFlag(0x8ac);
            OvlFunc_942_2008ba0();
        }
    }
    if (*(short *)(g + (0xe1 << 1)) == 2) {
        if (__GetFlag(0x109) == 0)
            __ClearFlag(0x8a9);
    }
    if (__GetFlag(0x911)) {
        x = 0xb0 << 15;
        y = 0xa3 << 19;
        if (__GetFlag(0x8a9) == 0) {
            __MapActor_SetPos(0xc, x, y);
            __Func_8092adc(0xc, 0, 0);
        }
    }
}
