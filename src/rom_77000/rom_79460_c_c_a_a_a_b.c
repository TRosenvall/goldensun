/* Cluster Func_8079754..Func_8079754 extracted from goldensun/asm/rom_77000/rom_79460_c_c_a_a_a.s.
 *
 * Total .text for this TU = 40 bytes (= 0x28).
 * Preserves the original ROM layout when slotted immediately before
 * asm/rom_77000/rom_79460_c_c_a_a_a_c.o in goldensun/stage1.ld.
 *
 * A LEAF FUNCTION, matched on the first screen. Adds a delta to the signed byte
 * at gState+0x11c, clamps it to 0..0x1c, stores it back and returns it.
 *
 * The two clamps are separate `if` statements rather than a min/max chain,
 * which is what gives the ROM's two independent compare-and-move pairs. The
 * read is `*(signed char *)(g + o)` with a named zero offset because Thumb
 * `ldrsb` has no immediate-offset form, and the write is a plain `*g` at offset
 * zero -- the ROM's `strb r2, [r3]`.
 *
 * The offset 0x8e << 1 is assigned before the global's address is taken,
 * matching the ROM's `mov r2, #0x8e / lsl r2, #1 / add r3, r2`.
 */
typedef struct { unsigned char _b[704]; } GlobalState;
extern GlobalState gState;

int Func_8079754(int d)
{
    unsigned char *g;
    unsigned int k;
    unsigned int o;
    int v;

    k = 0x8e << 1;
    g = (unsigned char *)&gState + k;
    o = 0;
    v = *(signed char *)(g + o);
    v = v + d;
    if (v > 0x1c)
        v = 0x1c;
    if (v < 0)
        v = 0;
    *g = v;
    return v;
}
