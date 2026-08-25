/* Cluster Func_8091858..Func_8091858 extracted from goldensun/asm/rom_8a000/rom_91584_c_a_c_c_c_a.s.
 *
 * Slotted between rom_91584_c_a_c_c_c_a_a.o and the rest of stage1.ld.
 *
 * Two identical guards over two gState halfwords: hand each to Func_8091814
 * and clear it if that returns non-zero.
 *
 * EACH ZERO IS A NAMED int, and both are needed. Written as the literal 0,
 * gcc-2.96 puts the constant in a LITERAL POOL and loads it with `ldrh` --
 * plus an inline pool and a `b` over it -- for 26 instructions against 24.
 * Same inverted narrow_constant tell as Func_808e118 in batch 48: where gcc
 * pools what the ROM builds with a `mov`, the source had a variable.
 *
 * The two offsets are spelled differently on purpose. 0x220 is built by the
 * ROM as `mov r3,#0x88 / lsl r3,#2` and 0x222 is pooled, which is just what
 * fits an eight-bit immediate; both are literals in the source.
 */
typedef struct { unsigned char _bytes[704]; } GlobalState;
extern GlobalState gState;
extern int Func_8091814(int v);

void Func_8091858(void)
{
    unsigned char *g;
    unsigned short *p;
    int z;

    g = (unsigned char *)&gState;
    p = (unsigned short *)(g + (0x88 << 2));
    if (Func_8091814(*p)) {
        z = 0;
        *p = z;
    }
    p = (unsigned short *)(g + 0x222);
    if (Func_8091814(*p)) {
        z = 0;
        *p = z;
    }
}
