/* Cluster OvlFunc_947_2009578..OvlFunc_947_2009578 extracted from goldensun/asm/overlays/rom_7d0e88/ovl_1528_a_a_a_c_c.s.
 *
 * The .s held this function and nothing else, so the C replaces the whole
 * translation unit and the linker script is untouched.
 *
 * Picks one of two three-word scroll-offset tables by a VCOUNT window and a
 * weighted random roll, then writes it to BG1HOFS/BG2HOFS/BG3HOFS.
 *
 * Both source and destination are walked with post-increment, which is what
 * gives the ROM's `ldmia r5!, {r3}` and `stmia r6!, {r3}`; the destination
 * pointer is RELOADED for BG2HOFS rather than walked from BG1HOFS, matching
 * `ldr r6, =REG_BG2HOFS` in the middle of the sequence.
 *
 * `x = 0x64; x *= r;` makes the constant the destination of the multiply,
 * matching `mov r3, #0x64 / mul r3, r0` -- see docs/elevation.md.
 *
 * OvlFunc_916_200836c in src/overlays/rom_7a37f0/ is the same function with a
 * different VCOUNT bound and different tables; it was elevated in the same
 * round from this source with the constants changed.
 */
#include "gba/io.h"

extern unsigned int iwram_3001ad4[];
extern unsigned int L3738[] __asm__(".L3738");
extern unsigned int L372c[] __asm__(".L372c");
extern int __Random(void);

void OvlFunc_947_2009578(void)
{
    unsigned int *s;
    unsigned int *d;
    int v;
    int r;
    unsigned int x;
    unsigned int lim;

    v = REG_VCOUNT;
    s = iwram_3001ad4;
    d = (unsigned int *)REG_ADDR_BG1HOFS;
    if (v != 0xe3 && (unsigned int)v > 0x2e)
        goto out;
    r = __Random();
    x = 0x64;
    x *= r;
    lim = *L3738;
    x >>= 16;
    if (x >= lim)
        goto out;
    s = L372c;
out:
    *d = *s++;
    d = (unsigned int *)REG_ADDR_BG2HOFS;
    *d++ = *s++;
    *d = *s;
}
