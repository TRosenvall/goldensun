/* Cluster OvlFunc_959_2008ee0..OvlFunc_959_2008ee0 extracted from goldensun/asm/overlays/rom_7e7574/ovl_9dc_a_c_c_a_a_c_c.s.
 *
 * Slotted between ovl_9dc_a_c_c_a_a_c_c_a.o and the rest of the overlay.
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
 * ITS THIRD CALL GOES TO A DIFFERENT CALLEE than its two twins -- __Func_8010704
 * rather than __Func_80105d4 -- with the same argument shape. Read off the `bl`,
 * not assumed from the family.
 */
extern unsigned char L7764[] __asm__(".L7764");
extern void __Func_80105d4(int a, int b, int c, int d, int e, int f);
extern void __Func_8010704(int a, int b, int c, int d, int e, int f);

void OvlFunc_959_2008ee0(int i)
{
    unsigned char *t;
    unsigned int off;
    int a;
    int b;

    t = L7764;
    off = i << 3;
    a = *(int *)(t + off);
    off += 4;
    b = *(int *)(t + off);
    __Func_80105d4(1, 0x50, 1, 3, a, b);
    __Func_80105d4(2, 0x50, 1, 1, a + 1, b);
    __Func_8010704(a, b - 0x3f, 1, 1, a, b - 0x3e);
}
