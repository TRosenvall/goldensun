/* Cluster OvlFunc_959_2008c90..OvlFunc_959_2008c90 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_a.s.
 *
 * Slotted between ovl_9dc_a_c_c_a_a_a_a.o and the rest of the overlay.
 *
 * A TABLE OF PAIRS INDEXED BY THE PARAMETER, read with an ADVANCING OFFSET:
 * `off = i << 3;` then `off += 4;` reproduces the ROM`s `lsl r0,#3 / ldr r6,[r3,r0]
 * / add r0,#4 / ldr r5,[r3,r0]`. The table stays the load`s BASE and the offset
 * walks -- indexing a two-int struct array instead folds the base in.
 *
 * The third call derives BOTH of its varying values from the second table entry
 * by subtraction, and the ROM makes the [sp, #4] copy DESTRUCTIVE on it
 * (`sub r5, #0x3e`) while the register argument gets its own (`mov r1, r5 /
 * sub r1, #0x3f`). Two different subtractions of the same value, so two
 * expressions.
 *
 * One of three near-twins in this overlay with OvlFunc_959_2008e30 and
 * OvlFunc_959_2008ee0, differing in the table, the constants, and -- for the
 * last of them -- the third callee.
 */
extern unsigned char L7714[] __asm__(".L7714");
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008c90(int i)
{
    unsigned char *t;
    unsigned int off;
    int a;
    int b;

    t = L7714;
    off = i << 3;
    a = *(int *)(t + off);
    off += 4;
    b = *(int *)(t + off);
    __Func_80105d4(0, 0x4d, 1, 3, a, b);
    __Func_80105d4(1, 0x4d, 1, 1, a + 1, b);
    __Func_80105d4(a, b - 0x30, 1, 1, a, b - 0x2e);
}
