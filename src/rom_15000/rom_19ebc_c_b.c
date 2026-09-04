/* LoadIcon -- 0x0801a5a4
 *
 * The twin of DecompressIcon (src/rom_15000/rom_20198_c_c_c_c_a_a.c), and the
 * second user of size.sym. It runs the same copy-the-decoder-into-RAM trick
 * three times: the 0x278-byte nibble decoder first, then EITHER the 0x9c-byte
 * or the 0x7c-byte packer depending on the second argument, calling each
 * through the gPtrs slot at +0xc4 and freeing the scratch after.
 *
 * ALL THREE SIZES ARE SYMBOLS, and this function is why size.sym exists rather
 * than three more const.sym entries. 0x9c and 0x7c both fit an eight-bit `mov`
 * and the ROM pools them anyway, which is the strongest form of that tell; and
 * all three are the gaps between consecutive ARM routines in rom_15430.s, so
 * the id space is identified rather than guessed. See size.sym.
 *
 * FOUR REGISTERS ARE PINNED, and they are what took this from 81 lines of 77
 * to exact. The function is tight enough that gcc's own allocation misses:
 *
 *  - `a` to r7, `b` to r6, the size to r5. Unpinned, gcc spills the second
 *    argument to r10 and puts the size in r7, and the whole body shifts.
 *  - THE DMA CONTROL WORD TO r8. The ROM builds 0x84000000 once and keeps it in
 *    a HIGH register across all three transfers (`mov r8, r2` in the prologue,
 *    `mov r2, r8` at the later two). include/dma.h's DMA3_COPY rebuilds it per
 *    call, which is six instructions SHORTER than the ROM and needs no
 *    r8 save/restore at all -- so the pin is not a tidying-up, it is what makes
 *    gcc reach for the high register and emit the `mov r7, r8 / push {r7}`
 *    prologue the ROM has. DMA3_SET takes the whole control word, so the shared
 *    part can be hoisted by hand while the size is ORed in per site.
 *
 * TWO ORDERING FIXES, both ordinary:
 *  - gPtrs is fetched into its OWN local at each of the two call sites. One
 *    shared local stays live across the middle of the function and costs a
 *    `mov r2, r0` copy; inlining it instead folds base and offset into a single
 *    `=gPtrs+196` pool word. Two locals, one per site, is the shape.
 *  - `n >>= 2` is its own statement so the size shift lands BETWEEN the control
 *    word's `mov` and its `lsl`, which is where the ROM issues it.
 *
 * The barriers on the two parameter copies are ordering only: without them gcc
 * issues the size's pool load before `mov r6, r1`.
 */
#include "dma.h"
extern unsigned char *galloc_iwram(int tag, int size);
extern void gfree(int tag);
extern unsigned char gPtrs[];
extern int _SIZE_8015afc;
extern int _SIZE_8015d74;
extern int _SIZE_8015e10;
extern unsigned char Func_8015afc[];
extern unsigned char Func_8015d74[];
extern unsigned char Func_8015e10[];

void LoadIcon(unsigned char *arg, int arg2)
{
    register unsigned char *a __asm__("r7");
    register int b __asm__("r6");
    unsigned char *p;
    unsigned char *g;
    unsigned char *g2;
    register unsigned int n __asm__("r5");
    register unsigned int ctl __asm__("r8");

    a = arg;
    __asm__ volatile ("" : : "r" (a));
    b = arg2;
    __asm__ volatile ("" : : "r" (b));
    n = (unsigned int)(int)&_SIZE_8015afc;
    p = galloc_iwram(0x31, n);
    n >>= 2;
    ctl = 0x84 << 24;
    DMA3_SET(Func_8015afc, p, ctl | n);
    g = gPtrs;
    (*(void (**)(int, unsigned char *))(g + 0xc4))(*(int *)(a + 0x604), a);
    gfree(0x31);
    if (b != 0) {
        n = (unsigned int)(int)&_SIZE_8015d74;
        p = galloc_iwram(0x31, n);
        n >>= 2;
        DMA3_SET(Func_8015d74, p, ctl | n);
    } else {
        n = (unsigned int)(int)&_SIZE_8015e10;
        p = galloc_iwram(0x31, n);
        n >>= 2;
        DMA3_SET(Func_8015e10, p, ctl | n);
    }
    g2 = gPtrs;
    (*(void (**)(unsigned char *, unsigned char *, int, int))(g2 + 0xc4))(
        a, a + (0x80 << 3), *(unsigned short *)(a + (0xc0 << 3)),
        *(unsigned short *)(a + (0xc0 << 3) + 2));
    gfree(0x31);
}
