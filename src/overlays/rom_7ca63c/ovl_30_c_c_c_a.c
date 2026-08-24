/* Cluster OvlFunc_944_200915c..OvlFunc_944_200915c extracted from goldensun/asm/overlays/rom_7ca63c/ovl_30_c_c_c.s.
 *
 * THE THIRD AND LAST of the three byte-identical copies named in
 * src/overlays/rom_77a7c8/ovl_30_c_a_c_c_a_c_c_b.c -- one per overlay,
 * differing only in the name and in which overlay-local `.L` table they read.
 * Read that file for the finding: the selector is UNSIGNED, and gcc's if-chain
 * for a small switch reports the type. The ROM has `bcc`; a signed selector
 * gives `bgt` and grows the chain by four instructions.
 *
 * SPLIT BY HAND, like src/overlays/rom_7d0e88/ovl_1440_c_c_a.c: the .s held
 * this function plus a .data section of script tables and a .bss section, and
 * tools/split_s.py refuses that. The function became ovl_30_c_c_c_a.o and both
 * data sections stayed in asm/overlays/rom_7ca63c/ovl_30_c_c_c_b.s; .text,
 * .data and .bss are listed separately in the linker script, so the layout is
 * unchanged. One `.global .L18f8` was added to the data half, beside the two
 * that were already exported.
 */
extern int L18f8[] __asm__(".L18f8");
extern int __GetFlag(int id);

int OvlFunc_944_200915c(unsigned int kind)
{
    int base;
    unsigned int i;

    base = 0;
    switch (kind) {
    case 0: base = 0x92c; break;
    case 1: base = 0x935; break;
    case 2: base = 0x917; break;
    case 3: base = 0x99 << 4; break;
    }
    for (i = 0; i <= 8; i++) {
        if (__GetFlag(base + i) != 0)
            return L18f8[i];
    }
    return 0;
}
