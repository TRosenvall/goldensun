/* Cluster OvlFunc_892_2008054..OvlFunc_892_2008054 extracted from goldensun/asm/overlays/rom_78dc80/ovl_30_c_c.s.
 *
 * SPLIT BY HAND, after getting it wrong. The .s held this function AND a .data
 * section of four .incbin blobs, and I deleted it having checked only the
 * function count. `tools/asmfacts.py --orphans` reported clean, because it
 * checks that every .o a linker script names still has a source -- not that a
 * source still has its data. The link failed with three undefined references.
 *
 * Restored from git and split properly: the function is ovl_30_c_c_a.o and the
 * data is ovl_30_c_c_b.o, listed separately in the .ld as .text and .data.
 * `tools/split_s.py` refuses this shape for exactly this reason and would have
 * said so; it was never run, because the file looked like a one-function file.
 *
 * Writes an interaction word, sets a flag, and on one save bit shakes the
 * screen. TWO LEVERS, and both were found in the last two rounds:
 *
 * 1. THE THREE ARGUMENTS TO __Func_8012330 ARE THE SAME VALUE, and the ROM
 *    builds all three separately. Written as three literals gcc builds one and
 *    copies it into the other two registers. Three SEPARATE LOCALS assigned
 *    before the `if` -- a different basic block from the call -- make gcc
 *    rematerialise each. That is the basic-block lever defeating constant-CSE;
 *    see reports/arg-interleave.md.
 *
 * 2. THE IWRAM ADDRESS IS ONE EXPRESSION, NOT A WALK. `base = iwram + off`
 *    written in a single statement gives the ROM's register roles; written as
 *    `base = iwram; base += off;` the base and offset registers swap and it is
 *    six positions out. The ROM's `add r3, r2` is destructive, which would
 *    normally say "walk" -- but here the offset variable is REUSED as the
 *    stored value (`off += 0x44`), and that is what decides it. See
 *    docs/elevation.md.
 */
extern unsigned int iwram_3001ebc;
extern void __SetFlag(int id);
extern int __GetFlag(int id);
extern void __Func_8091ff0(int a);
extern void __Func_8012330(int a, int b, int c);
extern void __StartEarthquake(void);

int OvlFunc_892_2008054(void)
{
    unsigned char *base;
    unsigned int off;
    int a;
    int b;
    int c;

    a = 0x80 << 9;
    b = 0x80 << 9;
    c = 0x80 << 9;
    off = 0xe0;
    off <<= 1;
    base = (unsigned char *)iwram_3001ebc + off;
    off += 0x44;
    *(unsigned int *)base = off;
    __SetFlag(0xa2 << 1);
    if (__GetFlag(0x814)) {
        __Func_8091ff0(0x8d);
        __Func_8012330(a, b, c);
        __StartEarthquake();
    }
    return 0;
}
