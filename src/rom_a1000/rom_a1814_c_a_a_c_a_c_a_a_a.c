/* Cluster Func_a1c6c..Func_a1c6c extracted from
 * goldensun/asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_a.s.
 *
 * Total .text for this TU = 68 bytes (= 0x44).  The .s holds this one function
 * and nothing else -- no .data, no .rodata, no pool -- so the file converts
 * WHOLE and the stage1.ld line
 *
 *      asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_a.o(.text)
 *
 * stays verbatim, with its asm/ prefix, between
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_b.o and
 * asm/rom_a1000/rom_a1814_c_a_a_c_a_c_a_a_b.o.
 *
 * Places a menu cursor/sprite on a grid: clamps a slot index, then writes the
 * two halfword screen coordinates of the object at *pp and hands it to
 * Func_80a17c4 to commit.  The cell pitch is 24 px across and 16 px down,
 * which is why the column term is built as (n*2 + n) << 3 rather than a shift.
 *
 * This is the same shape as the already-elevated Func_80a9bd8 in
 * src/rom_a1000/rom_a8604_c_c_a_b.c -- same clamp/div/mod/strh/call skeleton,
 * differing only in the clamp bound (0xf here, 0x1f there), the indirection on
 * the first argument, and the 24-px pitch.  Writing it straight from that twin
 * matched byte-for-byte on the first screen; no lever was needed.
 *
 * Naming note: offset 6 is X and offset 8 is Y.  Func_80a9cbc parks objects at
 * (0xf8, 0xa8) = (248, 168), just past the 240x160 screen in both axes, which
 * only reads as an offscreen park with 6 = X.  rom_a8604_c_c_a_b.c has the two
 * base arguments labelled the other way round; the codegen is identical either
 * way, so that file is not wrong, just mislabelled.
 */
extern void Func_80a17c4(unsigned char *p);

void Func_a1c6c(unsigned char **pp, int idx, int xbase, int ybase, int cols) {
    unsigned char *p;

    if (idx > 0xf)
        idx = 0;
    p = *pp;
    *(unsigned short *)(p + 8) = ((idx / cols) << 4) + ybase;
    *(unsigned short *)(p + 6) = ((idx % cols) * 24) + xbase;
    Func_80a17c4(p);
}
