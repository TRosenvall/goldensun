/* Func_80a65e4  --  NOT MATCHING
 *
 * Source asm: goldensun/asm/rom_a1000/rom_a5534_c_c_a.s
 * Best screen: 7 instructions in disagreeing regions, of 20 (rom 20, ours 19).
 *
 * BLOCKER CLASS: the address add is folded into the store's addressing mode.
 *
 *      rom   add r3, r2 / strh r0, [r3, #0x0]
 *      ours  strh r0, [r2, r3]
 *
 * Two branch arms each produce a base and an offset, and they join before the
 * store. The ROM adds them in the join block and stores at offset zero; gcc
 * keeps both live and uses Thumb's register-offset store, one instruction
 * shorter. Everything after inherits the register naming.
 *
 * WHAT WAS TRIED
 *   Naming the sum -- `q = g + k; *(short *)q = v;` -- which is the lever that
 *   fixed exactly this shape in
 *   src/overlays/rom_7c460c/ovl_314_a_c_c_a_c_a_a.c. BYTE-IDENTICAL here.
 *
 * THE DIFFERENCE IS WHERE THE SUM LIVES. In OvlFunc_939_2008ac4 the base and
 * offset are both produced in straight-line code, so naming the sum gives gcc a
 * value it has to materialise. Here the two operands arrive from opposite arms
 * of a branch and the sum is only needed by the store, so gcc sinks it into the
 * addressing mode no matter what the source calls it. That is a real limit on
 * the named-pointer lever and is why this park exists rather than a one-line
 * note.
 *
 * The bit-packing at the top is right: `m = 0x3fff; m &= b;` makes the constant
 * the AND's destination, matching `ldr r3, =0x3fff / and r3, r1`, and the arms
 * are written with the `c == 0` case first so it is the fall-through as in the
 * ROM.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;

int Func_80a65e4(int a, int b, int c)
{
    unsigned char *g;
    unsigned int k;
    int v;
    int m;

    m = 0x3fff;
    v = a << 10;
    m &= b;
    v |= m;
    if (c == 0) {
        g = (unsigned char *)&gState;
        k = 0x88 << 2;
    } else {
        g = (unsigned char *)&gState;
        k = 0x222;
    }
    *(short *)(g + k) = v;
    return 1;
}
